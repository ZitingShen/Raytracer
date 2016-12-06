#ifndef RAY_H
#define RAY_H

#include <vector>
#include <cfloat>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "view.h"
#include "output.h"

class Ray{
	public:
		glm::vec3 origin;
    glm::vec3 direction;
    float t;
    Ray();
};

void compute_ray(View& view, int i, int j, Ray& ray);
void set_up_camera_frame(View& view, Output& output);
void trace(glm::vec3& color, Ray& ray, int depth);
void write_pixel(ofstream& output_file, glm::vec3& color, 
	Output_format& format);
#endif