#ifndef RAY_H
#define RAY_H

#include <vector>
#include <cfloat>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "view.h"
#include "output.h"
#include "object.h"
#include "light.h"
#include "finish.h"
#include "pigment.h"

#define TRACE_DEPTH_MAX           1
#define RADIAN_TO_DEGREE          57.29578f // 180 over pi
#define DEGREE_TO_RADIAN          0.00873f
const glm::vec3 BACKGROUND_COLOR = glm::vec3(0, 0, 0);

class Ray{
	public:
		glm::vec3 origin;
    glm::vec3 direction;
    float t;
};

void compute_ray(View& view, float i, float j, Ray& ray);
void set_up_camera_frame(View& view, Output& output);
glm::vec3 trace(Ray& ray, int depth, 
	vector<Object*>& objects, vector<Light>& lights, 
	vector<Finish>& finishes, vector<Pigment*>& pigments);
void write_pixel(ofstream& output_file, glm::vec3& color, 
	Output_format& format);
glm::vec3 intersect(Ray& ray, vector<Object*>& objects, 
  Intersect_status& status);
bool is_visible(glm::vec3& point, Light& light,
  vector<Object*>& objects);
glm::vec3 phong(glm::vec3& point, glm::vec3& normal, Light& light, Ray& ray,
  Finish& finish, Pigment* pigment);
Ray reflect(Ray& ray, glm::vec3& point, glm::vec3& normal);
#endif