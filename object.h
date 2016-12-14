#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <vector>
#include "transformation.h"

using namespace std;

typedef enum _object_type{SPHERE, POLYHEDRON, TRIANGLEMESH} Object_type;
typedef enum _intersect_type{NO_INTERSECTION, YES_INTERSECTION} Intersect_type;

class Object {
  public:
    int id, pigment, finish;
    vector<int> trans;
    Object_type type;
    void transform(vector<Transformation>& transformations);
};

class Sphere: public Object{
  public:
    glm::vec3 origin;
    glm::vec3 radius;
    Sphere(Object& obj);
    void transform(vector<Transformation>& transformations);
};

class Polyhedron: public Object {
  public:
    vector<glm::vec4> planes;
    Polyhedron(Object& obj);
    void transform(vector<Transformation>& transformations);
};

class VERTEX{
  public:
    glm::vec3 pos;
    glm::vec3 normal;
    vector<int> face_indices;
};

class FACE{
  public:
    glm::vec3 A;
    glm::vec3 B;
    glm::vec3 C;
    glm::vec3 normal;
};

class Trianglemesh: public Object {
  public:
    string off_file;
    Trianglemesh(Object& obj);
    void transform(vector<Transformation>& transformations);
    /* reading from file */
    int num_v;
    int num_f;
    vector<VERTEX> vertices;
    vector<FACE> faces;
    void compute_normal();
};

class Intersect_status{
  public:
    Intersect_type type;
    int object_id;
    int plane_id;
    bool reverse_normal = false;
};

glm::vec3 compute_normal(Object* object, int plane_id, glm::vec3& point);

#endif
