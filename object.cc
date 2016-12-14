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

void Sphere::transform(vector<Transformation>& transformations) {
	for (int i = 0; i < trans.size(); i++) {
		if (transformations[trans[i]].type == SCALE) {
			radius *= transformations[trans[i]].description;
		} else if (transformations[trans[i]].type == TRANSLATE) {
			origin *= transformations[trans[i]].description;
		}
	}
}

void Polyhedron::transform(vector<Transformation>& transformations) {
	for (int i = 0; i < trans.size(); i++) {
		for (int j = 0; j < planes.size(); j++) {
			if (transformations[trans[i]].type == TRANSLATE) {
				planes[j].w += glm::dot(transformations[trans[i]].description, 
					glm::normalize(glm::vec3(planes[j])));
			} else if (transformations[trans[i]].type == SCALE) {
				// TODO
			}
		}
	}
}

void Trianglemesh::transform(vector<Transformation>& transformations) {
	for (int i = 0; i < trans.size(); i++) {
		for (int j = 0; j < faces.size(); j++) {
			if (transformations[trans[i]].type == TRANSLATE) {
				faces[j].A += transformations[trans[i]].description;
				faces[j].B += transformations[trans[i]].description;
				faces[j].C += transformations[trans[i]].description;
			} else if (transformations[trans[i]].type == SCALE) {
				// TODO
			}
		}
	}
}

void Trianglemesh::compute_normal(){
  glm::vec3 face_normal;
  // Assuming all vertices are given in counter-clock order
  // Computing face normals
  for (int i = 0; i < num_f; i++){
    face_normal = glm::cross(faces[i].C - faces[i].B,
                             faces[i].A - faces[i].B);
    faces[i].normal = glm::normalize(face_normal);
  }
  // Computing vertex normals
  for (unsigned int i = 0; i< num_v; i++){
    vertices[i].normal = glm::vec3(0, 0, 0);
    for (unsigned int j = 0; j < vertices[i].face_indices.size(); j++){
      vertices[i].normal += faces[j].normal;
    }
    vertices[i].normal = glm::normalize(vertices[i].normal 
      * (1.0f / vertices[i].face_indices.size()));
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
