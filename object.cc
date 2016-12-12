#include "object.h"

Sphere::Sphere(Object& obj) {
	id = obj.id;
	pigment = obj.pigment;
	finish = obj.finish;
	trans = obj.trans;
}

Polyhedron::Polyhedron(Object& obj) {
	id = obj.id;
	pigment = obj.pigment;
	finish = obj.finish;
	trans = obj.trans;
}

Trianglemesh::Trianglemesh(Object& obj) {
	id = obj.id;
	pigment = obj.pigment;
	finish = obj.finish;
	trans = obj.trans;
}

void Trianglemesh::compute_normal(){
  glm::vec3 face_normal;
  // Assuming all vertices are given in counter-clock order
  // Computing face normals
  for (int count=0; count<this->num_f; count++){
    face_normal = glm::cross(this->faces[count].C - this->faces[count].B,
                             this->faces[count].A - this->faces[count].B);
    this->faces[count].normal = glm::normalize(face_normal);
  }
  // Computing vertex normals
  vector<vector<int> > faces_per_vertex(this->num_v, vector<int>());
  glm::vec3 normal;
  int count = 0;
  for (int i=0; i<this->num_f; i++){  // which face we are looking at
    for (int j=0; j<3; j++){  // all vertices in this face
      faces_per_vertex[this->indices[count]].push_back(i);
      count++;
    }
  }
  count = 0;
  for (int i=0; i<(int)this->num_v; i++){
    normal = glm::vec3(0, 0, 0);
    for (int j=0; j<(int)faces_per_vertex[i].size(); j++){
      normal += this->faces[faces_per_vertex[i][j]].normal;
    }
    normal = glm::normalize(normal * (1.0f / (float)faces_per_vertex[i].size()));
    this->vertices[i].normal[0] = normal[0];
    this->vertices[i].normal[1] = normal[1];
    this->vertices[i].normal[2] = normal[2];
  }
}

glm::vec3 compute_normal(Object* object, int plane_id, glm::vec3& point) {
	if (object->type == SPHERE) {
		Sphere* sphere = static_cast<Sphere*>(object);
		return glm::normalize(point - sphere->origin);
	} else if (object->type == POLYHEDRON) {
		Polyhedron* poly = static_cast<Polyhedron*>(object);
		return glm::normalize(glm::vec3(poly->planes[plane_id].x, 
																		poly->planes[plane_id].y,
																		poly->planes[plane_id].z));
	} else if (object->type == TRIANGLEMESH) {
    //TODO will need to return interpolated vertex normal for phong
    //     currently returning face normal (flat-shading)
    Trianglemesh* tria = static_cast<Trianglemesh*>(object);
    return tria->faces[plane_id].normal;
  }
	return glm::vec3(0, 0, 0);
}
