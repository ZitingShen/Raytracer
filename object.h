#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

using namespace std;

typedef enum _object_type{SPHERE, POLYHEDRON, TRIANGLEMESH} Object_type;

class Object {
  public:
    int id, pigment, finish;
    vector<int> trans;
    Object_type type;
};

class Sphere: public Object{
  public:
    glm::vec3 origin;
    float radius;
    Sphere(Object& obj);
};

class Polyhedron: public Object {
  public:
    vector<glm::vec4> planes;
    Polyhedron(Object& obj);
};

class Trianglemesh: public Object {
  public:
    string off_file;
    Trianglemesh(Object& obj);
};

#endif