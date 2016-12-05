#ifndef PIGMENT_H
#define PIGMENT_H

typedef enum _color_type{
	SOLID, CHECKER;
} Pigment_type;

class Pigment {
  public:
  	int id;
    Pigment_type type;
}

class Solid_pigment: public solid_pigment{
  public:
    glm::vec3 color;
}

class Checker_pigment: public solid_pigment{
  public:
    glm::vec3 color1, color2;
    unsigned float size;
}

#endif