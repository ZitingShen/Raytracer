#ifndef OBJECT_H
#define OBJECT_H

#include <string>

using namespace std;

typedef enum _object_type{
  SPHERE, POLYHEDRON, TRIANGLEMESH;
} Object_type;

class Object {
  public:
    int id, pigment, finish;
    vector<int> trans;
    Object_type type;
}

class Sphere: public Object{
  public:
    glm::vec3 origin;
    float radius;
}

class Polyhedron: public Object {
  public:
    vector<glm::vec4> planes;
}

class Trianglemesh: public Object {
  public:
    string off_file;
}

#endif