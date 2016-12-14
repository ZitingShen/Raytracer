#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <glm/glm.hpp>

typedef enum _transformation_type{SCALE, TRANSLATE} Transformation_type;

class Transformation{
  public:
  	int id;
    Transformation_type type;
    glm::mat4 description;
    Transformation(Transformation_type type, float x, float y, float z, int id);
};

#endif
