#include "ray.h"
#include <iostream>

using namespace std;

void compute_ray(View& view, float i, float j, Ray& ray) {
  ray.origin = view.camera;
  //cout << glm::to_string((view.pixel*j - view.w*0.5f)*view.camera_x
  //+ (view.h*0.5f - view.pixel*i)*view.camera_y
  //+ (-1.0f)*view.camera_z + view.camera) << endl;
  ray.direction = glm::normalize((view.pixel*j - view.w*0.5f)*view.camera_x
  + (view.h*0.5f - view.pixel*i)*view.camera_y
  + (-1.0f)*view.camera_z);
  ray.t = FLT_MAX;
}

void set_up_camera_frame(View& view, Output& output) {
  view.camera_z = -glm::normalize(view.at-view.camera);
  view.camera_x = glm::normalize(glm::cross(view.up, view.camera_z));
  view.camera_y = glm::normalize(glm::cross(view.camera_z, view.camera_x));
  view.h = 2*glm::tan(view.fovy*DEGREE_TO_RADIAN*0.5f);
  view.w = view.h/output.height*output.width;
  view.pixel = view.h/output.height;
}

glm::vec3 trace(Ray& ray, int depth, 
  vector<Object*>& objects, vector<Light>& lights, 
  vector<Finish>& finishes, vector<Pigment*>& pigments) {
  if (depth > TRACE_DEPTH_MAX) return BACKGROUND_COLOR;

  glm::vec3 local_color(0, 0, 0);
  glm::vec3 reflected_color(0, 0, 0);
  glm::vec3 transmitted_color(0, 0, 0);
  Intersect_status status;
  glm::vec3 point = intersect(ray, objects, status);
  if (status.type == NO_INTERSECTION) return BACKGROUND_COLOR;
  glm::vec3 normal = compute_normal(objects[status.object_id], status.plane_id,
   point);
  if (status.reverse_normal) normal *= -1.0f;
  
  Finish& finish = finishes[objects[status.object_id]->finish];
  Pigment* pigment = pigments[objects[status.object_id]->pigment];
  
  local_color += phong(point, normal, lights[0], ray,
        finish, pigment);

  for(unsigned int i = 1; i < lights.size(); i++) {
    if (is_visible(point, lights[i], objects)) {
      local_color += phong(point, normal, lights[i], ray,
        finish, pigment);
    }
  }
  
  if (finish.reflectivity > 0) {
    Ray Rr = reflect(ray, point, normal);
    reflected_color = trace(Rr, depth+1, objects, lights, finishes, 
      pigments);
  }
  if (finish.transmission > 0) {
    float eta;
    if (status.reverse_normal) eta = finish.refraction;
    else eta = 1.0f/finish.refraction;
    Ray Rt = transmit(ray, point, normal, eta);
    transmitted_color = trace(Rt, depth+1, objects, lights, finishes, 
      pigments);
  }
  return (local_color + finish.reflectivity*reflected_color
                      + finish.transmission*transmitted_color);
}

void write_pixel(ofstream& output_file, glm::vec3& color, 
  Output_format& format){
  color[0] = color[0] > 1 ? 1 : color[0];
  color[1] = color[1] > 1 ? 1 : color[1];
  color[2] = color[2] > 1 ? 1 : color[2];
  color[0] = color[0] < 0 ? 0 : color[0];
  color[1] = color[1] < 0 ? 0 : color[1];
  color[2] = color[2] < 0 ? 0 : color[2];
  color *= 255;
  color[0] = (int) color[0];
  color[1] = (int) color[1];
  color[2] = (int) color[2];
  if (format == P3) {
    output_file << color[0] << ' '
                << color[1] << ' '
                << color[2] << ' ';
  } else if (format == P6) {
    output_file << (unsigned char) color[0]
                << (unsigned char) color[1]
                << (unsigned char) color[2];
  }
}

glm::vec3 intersect(Ray& ray, vector<Object*>& objects, 
  Intersect_status& status) {
  status.type = NO_INTERSECTION;
  glm::vec3 point(0, 0, 0);
  for (unsigned int i = 0; i < objects.size(); i++) {
    if (objects[i]->type == SPHERE) {
      Sphere* object = static_cast<Sphere*>(objects[i]);
      glm::vec3 co = ray.origin - object->origin;
      glm::vec3 inverse_radius = glm::vec3(1.0f/object->radius.x, 
                                           1.0f/object->radius.y, 
                                           1.0f/object->radius.z);
      glm::vec3 co_times_inverse_radius = co*inverse_radius;
      float inverse_length = glm::length(ray.direction*inverse_radius);
      float co_length = glm::length(co_times_inverse_radius);
      float a = inverse_length*inverse_length;
      float b = 2*glm::dot(co_times_inverse_radius, ray.direction*inverse_radius);
      float c = co_length*co_length - 1;
      float delta = b*b - 4*a*c;
      if (delta == 0) {
        float t = -b/(2*a);
        if (t >= 0 && t < ray.t-0.01) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t*ray.direction;
          ray.t = t;
        }
      } else if (delta > 0) {
        float base = -b/(2*a);
        float sqrt = glm::sqrt(delta)/(2*a);
        float t1 = base-sqrt;
        float t2 = base+sqrt;
        if (t1 >= 0 && t1 < ray.t-0.01) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t1*ray.direction;
          ray.t = t1;
          status.reverse_normal = false;
        } else if (t2 >= 0 && t2 < ray.t-0.01) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t2*ray.direction;
          ray.t = t2;
          status.reverse_normal = true;
        }
      }
    } else if (objects[i]->type == POLYHEDRON) {
      Polyhedron* object = static_cast<Polyhedron*>(objects[i]);
      float min_t = 0;
      float max_t = FLT_MAX;
      int min_plane = -1;
      int max_plane = -1;
      for (unsigned int j = 0; j < object->planes.size(); j++) {
        glm::vec3 normal = glm::normalize(glm::vec3(object->planes[j].x, 
                                                    object->planes[j].y,
                                                    object->planes[j].z));
        float denominator = glm::dot(ray.direction, normal);
        float nominator = -(object->planes[j].w + glm::dot(ray.origin, normal));
        if (denominator == 0) {
          if (nominator < 0) {
            // No intersection with the polygon.
            min_t = -1;
            max_t = -1;
            j = object->planes.size();
            continue;
          } else { // ignore the face
            continue;
          }
        }
        float t = nominator / denominator;
        if (denominator < 0) { // entering
          if (t > min_t) {
            min_t = t;
            min_plane = j;
            if (min_t > max_t) {
              // No intersection with the polygon.
              min_t = -1;
              max_t = -1;
              j = object->planes.size();
              continue; 
            }
          }
        } else { // leaving
          if (t < max_t) {
            max_t = t;
            max_plane = j;
            if (min_t > max_t) {
              // No intersection with the polygon.
              min_t = -1;
              max_t = -1;
              j = object->planes.size();
              continue; 
            }
          }
        }
      }
      if (min_t > 0 && min_t < ray.t - 0.01) {
        status.type = YES_INTERSECTION;
        status.object_id = object->id;
        status.plane_id = min_plane;
        point = ray.origin + min_t*ray.direction;
        ray.t = min_t;
        status.reverse_normal = false;
      } else if (max_t > 0 && max_t < ray.t - 0.01) {
        status.type = YES_INTERSECTION;
        status.object_id = object->id;
        status.plane_id = max_plane;
        point = ray.origin + max_t*ray.direction;
        ray.t = max_t;
        status.reverse_normal = true;
      }
    } else if (objects[i]->type == TRIANGLEMESH) {
      Trianglemesh* object = static_cast<Trianglemesh*>(objects[i]);
      float min_t = 0;
      float max_t = FLT_MAX;
      float t = 0;
      int min_plane = -1; // plane id
      int max_plane = -1; // plane id
      float dir = 0;
      for (int j=0; j<object->num_f; j++){
        if (glm::intersectRayTriangle(ray.origin,
                                      ray.direction,
                                      object->faces[j].A,
                                      object->faces[j].B,
                                      object->faces[j].C,
                                      point)){ // intersect with triangle
          t = glm::length(point - ray.origin);
          dir = glm::dot(ray.direction, object->faces[j].normal);
          if (dir < 0){ // entering
            if(t > min_t){
              min_t = t;
              min_plane = j;
            }
          }else{ // leaving
            if (t > 0 && t < max_t){
              max_t = t;
              max_plane = j;
            } 
          }
        }
      }
      if (min_t > 0 && min_t < ray.t - 0.01) {
        status.type = YES_INTERSECTION;
        status.object_id = object->id;
        status.plane_id = min_plane;
        point = ray.origin + min_t*ray.direction;
        ray.t = min_t;
        status.reverse_normal = false;
      } else if (max_t > 0 && max_t < ray.t - 0.01) {
        status.type = YES_INTERSECTION;
        status.object_id = object->id;
        status.plane_id = max_plane;
        point = ray.origin + max_t*ray.direction;
        ray.t = max_t;
        status.reverse_normal = true;
      }
    }
  }
  return point;
}

bool is_visible(glm::vec3& point, Light& light,
  vector<Object*>& objects) {
  Ray light_ray;
  light_ray.origin = light.pos;
  light_ray.direction = glm::normalize(point - light.pos);
  light_ray.t = glm::length(point - light.pos);
  for (unsigned int i = 0; i < objects.size(); i++) {
    if (objects[i]->type == SPHERE) {
    Sphere* object = static_cast<Sphere*>(objects[i]);
     glm::vec3 co = light_ray.origin - object->origin;
      glm::vec3 inverse_radius = glm::vec3(1.0f/object->radius.x, 
                                           1.0f/object->radius.y, 
                                           1.0f/object->radius.z);
      glm::vec3 co_times_inverse_radius = co*inverse_radius;
      float inverse_length = glm::length(light_ray.direction*inverse_radius);
      float co_length = glm::length(co_times_inverse_radius);
      float a = inverse_length*inverse_length;
      float b = 2*glm::dot(co_times_inverse_radius, light_ray.direction*inverse_radius);
      float c = co_length*co_length - 1;
      float delta = b*b - 4*a*c;
      if (delta == 0) {
        float t = -b/(2*a);
        if (t >= 0 && t < light_ray.t-0.01) return false;
      } else if (delta > 0) {
        float base = -b/(2*a);
        float sqrt = glm::sqrt(delta)/(2.0f*a);
        float t1 = (base-sqrt);
        float t2 = (base+sqrt);
        if (t1 >= 0 && t1 < light_ray.t-0.01) return false;
        if (t2 >= 0 && t2 < light_ray.t-0.01) return false;
      }
    } else if (objects[i]->type == POLYHEDRON) {
      Polyhedron* object = static_cast<Polyhedron*>(objects[i]);
      float min_t = 0;
      float max_t = FLT_MAX;
      for (unsigned int j = 0; j < object->planes.size(); j++) {
        glm::vec3 normal = glm::normalize(glm::vec3(object->planes[j].x, 
                                                    object->planes[j].y,
                                                    object->planes[j].z));
        float denominator = glm::dot(light_ray.direction, normal);
        float nominator = -(object->planes[j].w + glm::dot(light_ray.origin, normal));
        if (denominator == 0) {
          if (nominator < 0) {
            // No intersection with the polygon.
            min_t = -1;
            max_t = -1;
            j = object->planes.size();
            continue;
          } else { // ignore the face
            continue;
          }
        }
        float t = nominator / denominator;
        if (denominator < 0) { // entering
          if (t > min_t) {
            min_t = t;
            if (min_t > max_t) {
              // No intersection with the polygon.
              min_t = -1;
              max_t = -1;
              j = object->planes.size();
              continue; 
            }
          }
        } else { // leaving
          if (t < max_t) {
            max_t = t;
            if (min_t > max_t) {
              // No intersection with the polygon.
              min_t = -1;
              max_t = -1;
              j = object->planes.size();
              continue; 
            }
          }
        }
      }
      if (min_t > 0 && min_t < light_ray.t - 0.01f) {
        return false;
      } else if (max_t > 0 && max_t < light_ray.t - 0.01f) {
        return false;
      }
    } else if (objects[i]->type == TRIANGLEMESH) {
      Trianglemesh* object = static_cast<Trianglemesh*>(objects[i]);
      float min_t = 0;
      float max_t = FLT_MAX;
      float t = 0;
      float dir = 0;
      for (int j=0; j<object->num_f; j++){
        if (glm::intersectRayTriangle(light_ray.origin,
                                      light_ray.direction,
                                      object->faces[j].A,
                                      object->faces[j].B,
                                      object->faces[j].C,
                                      point)){ // intersect with triangle
          t = glm::length(point - light_ray.origin);
          dir = glm::dot(light_ray.direction, object->faces[j].normal);
          if (dir < 0){ // entering
            if (t > min_t && t < light_ray.t) return false;
          } else{ // leaving
            if ((t > 0 && t < max_t) && t < light_ray.t) return false;
          }
        }
      }
    }
  }
  return true;
}

glm::vec3 phong(glm::vec3& point, glm::vec3& normal, Light& light, Ray& ray,
  Finish& finish, Pigment* pigment) {
  glm::vec3 ambient_light(0, 0, 0);
  glm::vec3 diffuse_light(0, 0, 0);
  glm::vec3 specular_light(0, 0, 0);

  if (light.id == 0) { // ambient light
    ambient_light = finish.ambient * light.color;
  } else { // non-ambient light
    float d = glm::length(point - light.pos);
    glm::vec3 l = glm::normalize(light.pos - point);
    glm::vec3 v = glm::normalize(ray.origin - point);
    glm::vec3 h = glm::normalize((l + v) * 0.5f);

    diffuse_light = finish.diffuse * light.color
      * glm::dot(l, normal)
      /(light.a + light.b*d + light.c*d*d);
    specular_light = finish.specular * glm::vec3(1, 1, 1) 
      * glm::pow(glm::dot(h, normal), finish.shininess)
      /(light.a + light.b*d + light.c*d*d);
  }
  glm::vec3 color(0, 0, 0);
  if (pigment->type == SOLID) {
    Solid_pigment* solid = static_cast<Solid_pigment*>(pigment);
    color = solid->color;
  } else if (pigment->type == CHECKER) {
    Checker_pigment* checker = static_cast<Checker_pigment*>(pigment);
    if ((int) (glm::floor(point.x/checker->size) 
       + glm::floor(point.y/checker->size)
       + glm::floor(point.z/checker->size)) % 2 == 0)
      color = checker->color1;
    else color = checker->color2;
  } else if (pigment->type == IMAGE) {
    
  }
  return (ambient_light + diffuse_light)*color + specular_light;
}

Ray reflect(Ray& ray, glm::vec3& point, glm::vec3& normal) {
  Ray reflected_ray;
  // thank god someone posted a similar question on stackoverflow and I don't have to be stuck for hours
  reflected_ray.origin = point + normal*0.01f;
  reflected_ray.direction = glm::normalize(glm::reflect(ray.direction, normal));
  reflected_ray.t = FLT_MAX;
  return reflected_ray;
}

Ray transmit(Ray& ray, glm::vec3& point, glm::vec3& normal, float refraction) {
  Ray transmitted_ray;
  transmitted_ray.origin = point - normal*0.01f;
  transmitted_ray.direction = glm::normalize(glm::refract(ray.direction, normal, 
    refraction));
  transmitted_ray.t = FLT_MAX;
  return transmitted_ray;
}