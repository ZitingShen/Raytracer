#include "ray.h"
#include <iostream>

using namespace std;

Ray::Ray() {
	origin = glm::vec3(0, 0, 0);
	direction = glm::vec3(0, 0, 0);
	t = FLT_MAX;
}

void compute_ray(View& view, glm::vec3& left_up, vector<glm::vec3>& pixel_size,
 int i, int j, Ray& ray) {
  ray.origin = view.camera;
  //cout << glm::to_string(left_up - i*1.0f*pixel_size[1] 
  //  + j*1.0f*pixel_size[0]) << endl;
  ray.direction = glm::normalize(left_up - i*1.0f*pixel_size[1] 
    + j*1.0f*pixel_size[0] - view.camera);
}

glm::vec3 get_left_up(View& view, Output& output, 
  vector<glm::vec3>& pixel_size) {
  glm::vec3 camera_z = -glm::normalize(view.at-view.camera);
  glm::vec3 camera_x = glm::normalize(glm::cross(view.up, camera_z));
  glm::vec3 camera_y = glm::normalize(glm::cross(camera_z, camera_x));
  float h = 2*glm::tan(view.fovy*0.5f);
  float w = h/output.height*output.width;
  float pixel = h/output.height;
  pixel_size.push_back(pixel*camera_x);
  pixel_size.push_back(pixel*camera_y);
  return view.at + h/2*camera_y - w/2*camera_x;
}

void trace(glm::vec3& color, Ray& ray, int depth) {

}

void write_pixel(glm::vec3& color) {

}