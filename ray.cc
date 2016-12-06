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

void trace(glm::vec3& color, Ray& ray, int depth) {

}

void write_pixel(ofstream& output_file, glm::vec3& color, 
  Output_format& format){
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