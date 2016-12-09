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
    Ray Rt = transmit(ray, point, normal, finish.refraction);
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
      float co_length = glm::length(co);
      float a = 1;
      float b = 2*glm::dot(co, ray.direction);
      float c = co_length*co_length 
        - object->radius*object->radius;
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
        } else if (t2 >= 0 && t2 < ray.t-0.01) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t2*ray.direction;
          ray.t = t2;
        }
      }
    } else if (objects[i]->type == POLYHEDRON) {
      Polyhedron* object = static_cast<Polyhedron*>(objects[i]);
      for (unsigned int j = 0; j < object->planes.size(); j++) {
        glm::vec3 normal = glm::normalize(glm::vec3(object->planes[j].x, 
                                                    object->planes[j].y,
                                                    object->planes[j].z));
        float t = -(object->planes[j].w + glm::dot(ray.origin, normal))
          /glm::dot(ray.direction, normal);
        if (t >= 0 && t < ray.t-0.01) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          status.plane_id = j;
          point = ray.origin + t*ray.direction;
          ray.t = t;
        }
      }
    } else if (objects[i]->type == TRIANGLEMESH) {
      //TODO

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
    
  for (unsigned int j = 0; j < objects.size(); j++) {
    if (objects[j]->type == SPHERE) {
    Sphere* object = static_cast<Sphere*>(objects[j]);
      glm::vec3 co = light_ray.origin - object->origin;
      float co_length = glm::length(co);
      float a = 1;
      float b = 2*glm::dot(co, light_ray.direction);
      float c = co_length*co_length 
        - object->radius*object->radius;
      float delta = b*b - 4*a*c;
      if (delta == 0) {
        float t = -b/(2*a);
        if (t >= 0 && t < light_ray.t-0.01) return false;
      } else if (delta > 0) {
        float base = -b/(2*a);
        float sqrt = glm::sqrt(delta)/(2.0f*a);
        float t1 = base-sqrt;
        float t2 = base+sqrt;
        if (t1 >= 0 && t1 < light_ray.t-0.01) return false;
        if (t2 >= 0 && t2 < light_ray.t-0.01) return false;
      }
    } else if (objects[j]->type == POLYHEDRON) {
      Polyhedron* object = static_cast<Polyhedron*>(objects[j]);
      for (unsigned int k = 0; k < object->planes.size(); k++) {
        glm::vec3 normal = glm::normalize(glm::vec3(object->planes[k].x, 
                                                    object->planes[k].y,
                                                    object->planes[k].z));
        float t = -(object->planes[j].w + glm::dot(light_ray.origin, normal))
          /glm::dot(light_ray.direction, normal);
        if (t >= 0 && t < light_ray.t-0.01) {
          return false;
        }
      }
    } else if (objects[j]->type == TRIANGLEMESH) {
      //TODO
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
      * glm::max(glm::dot(l, normal), 0.0f)
      /(light.a + light.b*d + light.c*d*d);
    specular_light = finish.specular * glm::vec3(1, 1, 1) 
      * glm::pow(glm::max(glm::dot(h, normal), 0.0f), finish.shininess)
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
  }
  return (ambient_light+diffuse_light)*color+ specular_light;
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
  transmitted_ray.origin = point + normal*0.01f;
  transmitted_ray.direction = glm::normalize(glm::refract(ray.direction, normal, 
    refraction));
  transmitted_ray.t = FLT_MAX;
  return transmitted_ray;
}