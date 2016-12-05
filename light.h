#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light {
  public:
  	int id;
	  glm::vec3 pos;
	  glm::vec3 color;
	  float a, b, c;
};

#endif