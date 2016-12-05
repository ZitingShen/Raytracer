#ifndef RAY_H
#define RAY_H

#include <vector>
#include <cfloat>
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

void compute_ray(View& view, glm::vec3& left_up, vector<glm::vec3>& pixel_size,
 int i, int j, Ray& ray);
glm::vec3 get_left_up(View& view, Output& output, 
  vector<glm::vec3>& pixel_size);
void trace(glm::vec3& color, Ray& ray, int depth);
void write_pixel(glm::vec3& color);
#endif