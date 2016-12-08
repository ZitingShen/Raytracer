#include "ray.h"
#include <iostream>

using namespace std;

Ray::Ray() {
	origin = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, 0);
	t = FLT_MAX;
}

void compute_ray(View& view, int i, int j, Ray& ray) {
  ray.origin = view.camera;
  //cout << glm::to_string((view.pixel*j - view.w*0.5f)*view.camera_x
  //+ (view.h*0.5f - view.pixel*i)*view.camera_y
  //+ (-1.0f)*view.camera_z + view.camera) << endl;
  ray.direction = glm::normalize((view.pixel*j - view.w*0.5f)*view.camera_x
  + (view.h*0.5f - view.pixel*i)*view.camera_y
  + (-1.0f)*view.camera_z);
}

void set_up_camera_frame(View& view, Output& output) {
  view.camera_z = -glm::normalize(view.at-view.camera);
  view.camera_x = glm::normalize(glm::cross(view.up, view.camera_z));
  view.camera_y = glm::normalize(glm::cross(view.camera_z, view.camera_x));
  view.h = 2*glm::tan(view.fovy*0.5f);
  view.w = view.h/output.height*output.width;
  view.pixel = view.h/output.height;
}

glm::vec3 trace(Ray& ray, int depth, 
  vector<Object*>& objects, vector<Light>& lights, 
  vector<Finish>& finishes, vector<Pigment>& pigments) {
  if (depth > TRACE_DEPTH_MAX) return BACKGROUND_COLOR;

  glm::vec3 local_color(0, 0, 0);
  glm::vec3 reflected_color(0, 0, 0);
  glm::vec3 transmitted_color(0, 0, 0);
  Intersect_status status;
  glm::vec3 point = intersect(ray, objects, status);
  if (status.type == NO_INTERSECTION) return BACKGROUND_COLOR;
  glm::vec3 normal = compute_normal(objects[status.object_id], status.plane_id,
   point);
  
  float specular = finishes[objects[status.object_id]->finish].specular;
  float transmission = finishes[objects[status.object_id]->finish].transmission;
  for(int i = 0; i < lights.size();i++) {
    if (is_visible(point, lights[i], objects))
      local_color += phong(lights[i], point, normal, pigments);
    /*
    if (specular > 0) {
      Ray Rr = reflect(ray, point, normal);//TODO
      reflected_color = trace(Rr, depth+1);
    } 
    if (transmission > 0) {
      Ray Rt = transmit(ray, point, normal);//TODO
      transmitted_color = trace(Rt, depth+1);
    }
    */
  }
  return (local_color + specular*reflected_color
                      + transmission*transmitted_color);
}

void write_pixel(ofstream& output_file, glm::vec3& color, 
  Output_format& format){
  color *= 255;
  color[0] = color[0] > 255 ? 255 : color[0];
  color[1] = color[1] > 255 ? 255 : color[1];
  color[2] = color[2] > 255 ? 255 : color[2];
  if (format == P3) {
    output_file << color[0] << ' '
                << color[1] << ' '
                << color[2] << ' ';
  } else if (format == P6) {
    output_file << (unsigned char) color[0] << ' '
                << (unsigned char) color[1] << ' '
                << (unsigned char) color[2] << ' ';
  }
}

glm::vec3 intersect(Ray& ray, vector<Object*>& objects, 
  Intersect_status& status) {
  status.type = NO_INTERSECTION;
  glm::vec3 point(0, 0, 0);
  for (int i = 0; i < objects.size(); i++) {
    if (objects[i]->type == SPHERE) {
      Sphere* object = static_cast<Sphere*>(objects[i]);
      cout << glm::to_string(ray.origin) << endl;
      glm::vec3 co = ray.origin - object->origin;
      float co_length = glm::length(co);
      float a = 1;
      float b = 2*glm::dot(co, ray.direction);
      float c = co_length*co_length 
        - object->radius*object->radius;
      float delta = b*b - 4*a*c;
      if (delta == 0) {
        float t = -b/(2*a);
        if (t >= 0 && t < ray.t) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t*ray.direction;
        }
      } else {
        float base = -b/(2*a);
        float sqrt = glm::sqrt(delta)/(2*a);
        float t1 = base-sqrt;
        float t2 = base+sqrt;
        if (t1 >= 0 && t1 < ray.t) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t2*ray.direction;
        } else if (t2 >= 0 && t2 < ray.t) {
          status.type = YES_INTERSECTION;
          status.object_id = object->id;
          point = ray.origin + t2*ray.direction;
        }
      }
    } else if (objects[i]->type == POLYHEDRON) {
      //TODO
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
  Intersect_status status;
  intersect(light_ray, objects, status);
  return (status.type == NO_INTERSECTION);
}

glm::vec3 phong(Light& light, glm::vec3& point, glm::vec3& normal, 
  vector<Pigment>& pigments) {
  return glm::vec3(1, 0, 0); //TODO
}