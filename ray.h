#ifndef RAY_H
#define RAY_H

#include <vector>
#include <cfloat>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "view.h"
#include "output.h"

#define TRACE_DEPTH_MAX  2
const glm::vec3 BACKGROUND_COLOR = glm::vec3(0, 0, 0);

class Ray{
	public:
		glm::vec3 origin;
    glm::vec3 direction;
    float t;
    Ray();
};

void compute_ray(View& view, int i, int j, Ray& ray);
void set_up_camera_frame(View& view, Output& output);
glm::vec3 trace(Ray& ray, int depth);
void write_pixel(ofstream& output_file, glm::vec3& color, 
	Output_format& format);
#endif