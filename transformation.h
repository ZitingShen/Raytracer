#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

typedef enum _transformation_type{
 SCALE, TRANSLATE;
} Transformation_type;

class Transformation{
  public:
  	int id;
    Transformation_type type;
    glm::vec3 description;
}

#endif