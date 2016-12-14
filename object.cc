#include "object.h"
#include <iostream>

Sphere::Sphere(Object& obj) {
	id = obj.id;
	pigment = obj.pigment;
	finish = obj.finish;
	trans = obj.trans;
}

Sphere::~Sphere(){
  /* Do nothing */
}

Polyhedron::Polyhedron(Object& obj) {
	id = obj.id;
	pigment = obj.pigment;
	finish = obj.finish;
	trans = obj.trans;
}

Polyhedron::~Polyhedron(){
  /* Do nothing */
}

Trianglemesh::Trianglemesh(Object& obj) {
	id = obj.id;
	pigment = obj.pigment;
	finish = obj.finish;
	trans = obj.trans;
}

Trianglemesh::~Trianglemesh(){
  this->faces.clear();
  this->vertices.clear();
}


//VERTEX::~VERTEX(){
//  this->face_indices.clear();
//}

//FACE::~FACE(){
//  * Do nothing */
//}


Intersect_status::~Intersect_status(){
  /* Do Nothing */
}

void Sphere::transform(vector<Transformation>& transformations) {
	for (unsigned int i = 0; i < trans.size(); i++) {
		if (transformations[trans[i]].type == SCALE) {
			radius *= transformations[trans[i]].description;
		} else if (transformations[trans[i]].type == TRANSLATE) {
			origin += transformations[trans[i]].description;
		}
	}
}

void Polyhedron::transform(vector<Transformation>& transformations) {
	for (unsigned int i = 0; i < trans.size(); i++) {
		for (unsigned int j = 0; j < planes.size(); j++) {
			if (transformations[trans[i]].type == TRANSLATE) {
				planes[j].w += glm::dot(transformations[trans[i]].description, 
					glm::normalize(glm::vec3(planes[j])));
			} else if (transformations[trans[i]].type == SCALE) {
				planes[j].x *= transformations[trans[i]].description.x;
				planes[j].y *= transformations[trans[i]].description.y;
				planes[j].z *= transformations[trans[i]].description.z;
			}
		}
	}
}

void Trianglemesh::transform(vector<Transformation>& transformations) {
	for (unsigned int i = 0; i < trans.size(); i++) {
		for (unsigned int j = 0; j < vertices.size(); j++) {
			if (transformations[trans[i]].type == TRANSLATE) {
				vertices[j].pos += transformations[trans[i]].description;
			} else if (transformations[trans[i]].type == SCALE) {
				vertices[j].pos -= center;
				vertices[j].pos *= transformations[trans[i]].description;
				vertices[j].pos += center;
			}
		}
	}
}

void Trianglemesh::compute_normal(){
  glm::vec3 face_normal;
  // Assuming all vertices are given in counter-clock order
  // Computing face normals
  for (int i = 0; i < num_f; i++){
    face_normal = glm::cross(vertices[faces[i].C].pos - vertices[faces[i].B].pos,
                             vertices[faces[i].A].pos - vertices[faces[i].B].pos);
    faces[i].normal = glm::normalize(face_normal);
  }
  // Computing vertex normals
  for (int i = 0; i< num_v; i++){
    vertices[i].normal = glm::vec3(0, 0, 0);
    for (unsigned int j = 0; j < vertices[i].face_indices.size(); j++){
      vertices[i].normal += faces[ vertices[i].face_indices[j] ].normal;
    }
    vertices[i].normal = glm::normalize(vertices[i].normal
      * (1.0f / static_cast<float>(vertices[i].face_indices.size())));
  }
}

void Trianglemesh::compute_center(){
  glm::vec3 result(0,0,0);
  for (int i=0; i<this->num_v; i++){
    result += this->vertices[i].pos;
  }
  result = result * (1.0f/static_cast<float>(this->num_v));
  this->center = result;
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
    float AP = glm::distance(tria->vertices[tria->faces[plane_id].A].pos, point);
    float BP = glm::distance(tria->vertices[tria->faces[plane_id].B].pos, point);
    float CP = glm::distance(tria->vertices[tria->faces[plane_id].C].pos, point);
    float AP_weight = (AP + BP + CP) - AP;
    float BP_weight = (AP + BP + CP) - BP;
    float CP_weight = (AP + BP + CP) - CP;
    float total_weight = AP_weight + BP_weight + CP_weight;
    glm::vec3 point_normal = glm::normalize(AP_weight / total_weight * tria->vertices[tria->faces[plane_id].A].normal
                                          + BP_weight / total_weight * tria->vertices[tria->faces[plane_id].B].normal
                                          + CP_weight / total_weight * tria->vertices[tria->faces[plane_id].C].normal);
 
    return point_normal;
  }
	return glm::vec3(0, 0, 0);
}

void Trianglemesh::generate_off(){
  string name = "test.off";
  fstream off;
  off.open(name);
  off << "OFF" << endl;
  off << num_v << " " << num_f << " " << 0 << endl;
  for (int i=0; i<num_v; i++){
    off << vertices[i].pos[0] << " " << vertices[i].pos[1] << " " << vertices[i].pos[2] << endl;
  }
  for (int i=0; i<num_f; i++){
    off << 3 << " " << faces[i].A << " " << faces[i].B << " " << faces[i].C << endl;
  }

  off << "Below are additional debug info" << endl;
  off << "vertex normals" << endl;
  for (int i=0; i<num_v; i++){
    off << vertices[i].normal[0] << " " << vertices[i].normal[1] << " " << vertices[i].normal[2] << endl;
  }

  off << "face nromals" << endl;
  for (int i=0; i<num_f; i++){
    off << faces[i].normal[0] << " " << faces[i].normal[1] << " " << faces[i].normal[2] << endl;
  }

  off << "faces shared by vertices" << endl;
  for (int i=0; i<num_v; i++){
    for (unsigned int j=0; j<vertices[i].face_indices.size(); j++){
      off << vertices[i].face_indices[j] << " ";
    }
    off << endl;
  }
  
  return;
}

bool read_triangle_mesh(Trianglemesh* new_obj){
  string off;
  ifstream my_fin;
  int holder;

  cin >> new_obj->off_file; // get filename
  my_fin.open(new_obj->off_file);
  if (!my_fin.is_open()){
    cerr << "TRIANGLEMESH: CAN NOT OPEN FILE: " << new_obj->off_file <<endl;
    return false;
  }

  getline(my_fin, off);
  if (off.compare("OFF") != 0){
    cerr << "TRIANGLEMESH: NOT AN OFF FILE: " << new_obj->off_file <<endl;
    return false;
  }
  /* reading attributs */
  my_fin >> new_obj->num_v;
  my_fin >> new_obj->num_f;
  my_fin >> holder; // new_e: obsolete info
  /* reading vertices */
  new_obj->vertices.resize(new_obj->num_v);
  for (int i = 0; i< new_obj->num_v; i++){
    for (int j = 0; j < 3; j++){
      my_fin >> new_obj->vertices[i].pos[j];
    }
  }
  /* reading faces */
  new_obj->faces.resize(new_obj->num_f);
  for (int i = 0; i< new_obj->num_f; i++){
    my_fin >> holder; // the number 3
    if (holder != 3){
      cerr << "TRIANGLEMESH: NON TRIANGULAR FACE DETECTED: " << holder << endl;
      return false;
    }
    my_fin >> holder;
    new_obj->vertices[holder].face_indices.push_back(i);
    new_obj->faces[i].A = holder; // record the index to vector<VERTEX>

    my_fin >> holder;
    new_obj->vertices[holder].face_indices.push_back(i);
    new_obj->faces[i].B = holder;

    my_fin >> holder;
    new_obj->vertices[holder].face_indices.push_back(i);
    new_obj->faces[i].C = holder;
  }

  my_fin.close();
  new_obj->compute_normal(); // compute face & vertex normals
  new_obj->compute_center();
  // new_obj->generate_off(); // test if read-in is correct
  return true;
}
