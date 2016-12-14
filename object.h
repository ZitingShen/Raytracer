#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <fstream>
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
    ~Sphere();
    void transform(vector<Transformation>& transformations);
};

class Polyhedron: public Object {
  public:
    vector<glm::vec4> planes;
    Polyhedron(Object& obj);
    ~Polyhedron();
    void transform(vector<Transformation>& transformations);
};

//class VERTEX{
//  public:
//    ~VERTEX();
struct VERTEX{
    glm::vec3 pos;
    glm::vec3 normal;
    vector<int> face_indices;
};

//class FACE{
//  public:
//    ~FACE();
struct FACE{
    int A; // index in vertices
    int B; // index in vertices
    int C; // index in vertices
    glm::vec3 normal;
};

class Trianglemesh: public Object {
  public:
    string off_file;
    Trianglemesh(Object& obj);
    ~Trianglemesh();
    void transform(vector<Transformation>& transformations);
    /* reading from file */
    int num_v;
    int num_f;
    vector<VERTEX> vertices;
    vector<FACE> faces;
    glm::vec3 center;
    void compute_normal();
    void compute_center();
    void generate_off();
};

class cylinder : public Object{
  public:
    cylinder(Object& obj);
    ~cylinder();
    glm::vec3 origin_A;
    glm::vec3 origin_B;
    float radius;
    float height;
    void transform(vector<Transformation>& transformations);
};

class Intersect_status{
  public:
    ~Intersect_status();
    Intersect_type type;
    int object_id;
    int plane_id;
    bool reverse_normal = false;
};

glm::vec3 compute_normal(Object* object, int plane_id, glm::vec3& point);

bool read_triangle_mesh(Trianglemesh* new_obj);

#endif
