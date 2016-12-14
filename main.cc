#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "light.h"
#include "pigment.h"
#include "finish.h"
#include "transformation.h"
#include "object.h"
#include "view.h"
#include "output.h"
#include "ray.h"

using namespace std;

void read_in(Output& output, View& view, vector<Light>& lights,
	vector<Pigment*>& pigments, vector<Finish>& finishes,
	vector<Transformation>& transformations, vector<Object*>& objects);


int main(){
	Output output;
	ofstream output_file;
	View view;
	vector<Light> lights;
	vector<Pigment*> pigments;
	vector<Finish> finishes;
	vector<Transformation> transformations;
	vector<Object*> objects;

	read_in(output, view, lights, pigments, finishes, transformations, objects);

	output.format = P6;
	output_file.open(output.file_name);
	if (output.format == P6) output_file << "P6" << endl;
	else if (output.format == P3) output_file << "P3" << endl;
	output_file << output.width << " " << output.height << endl;
	output_file << 255 << endl;

	set_up_camera_frame(view, output);
	for (int i = 0; i < output.height; i++) {
		for (int j = 0; j < output.width; j++) {
			glm::vec3 color(0, 0, 0);
			//for (float anti_alias_x = 0; anti_alias_x <= 1; anti_alias_x+=0.5f) {
			//	for (float anti_alias_y = 0; anti_alias_y <= 1; anti_alias_y+=0.5f) {
					Ray ray;
			//		compute_ray(view, i+anti_alias_x, j+anti_alias_y, ray);
					compute_ray(view, i, j, ray);
					color += trace(ray, 0, objects, lights, finishes, pigments);
			//	}
			//}
			//color = color*(1/9.0f);
			write_pixel(output_file, color, output.format);
		}
	}
	output_file.close();
}

void read_in(Output& output, View& view, vector<Light>& lights,
	vector<Pigment*>& pigments, vector<Finish>& finishes,
	vector<Transformation>& transformations, vector<Object*>& objects) {
	float x, y, z, w;
	int num_lights, num_pigments, num_finishes;
	int num_transformations, num_objects;
	int num_trans, next_trans;
	int num_planes;
	string type;

	cin >> output.file_name
			>> output.width 
			>> output.height;
	cin >> x >> y >> z;
	view.camera = glm::vec3(x, y, z);
	cin >> x >> y >> z;
	view.at = glm::vec3(x, y, z);
	cin >> x >> y >> z;
	view.up = glm::vec3(x, y, z);
	cin >> view.fovy;

	cin >> num_lights;
	for (int i = 0; i < num_lights; i++) {
		Light new_light;
		cin >> x >> y >> z;
		new_light.pos = glm::vec3(x, y, z);
		cin >> x >> y >> z;
		new_light.color = glm::vec3(x, y, z);
		cin >> new_light.a >> new_light.b >> new_light.c;
		new_light.id = i;
		lights.push_back(new_light);
	}

	cin >> num_pigments;
	for (int i = 0; i < num_pigments; i++) {
		cin >> type;
		if (type == "solid") {
			Solid_pigment* new_pigment = new Solid_pigment();
			cin >> x >> y >> z;
			new_pigment->color = glm::vec3(x, y, z);
			new_pigment->type = SOLID;
			new_pigment->id = i;
			pigments.push_back(new_pigment);
		} else if (type == "checker") {
			Checker_pigment* new_pigment = new Checker_pigment();
			cin >> x >> y >> z;
			new_pigment->color1 = glm::vec3(x, y, z);
			cin >> x >> y >> z;
			new_pigment->color2 = glm::vec3(x, y, z);
			cin >> new_pigment->size;
			new_pigment->type = CHECKER;
			new_pigment->id = i;
			pigments.push_back(new_pigment);
		}
	}

	cin >> num_finishes;
	for (int i = 0 ; i < num_finishes; i++) {
		Finish new_finish;
		cin >> new_finish.ambient
		    >> new_finish.diffuse 
		    >> new_finish.specular
		    >> new_finish.shininess
				>> new_finish.reflectivity 
				>> new_finish.transmission 
				>> new_finish.refraction;
		new_finish.id = i;
		finishes.push_back(new_finish);
	}

	cin >> num_transformations;
	for (int i = 0; i < num_transformations; i++) {
		cin >> type;
		if (type == "scale") {
			cin >> x >> y >> z;
			Transformation new_transformation(SCALE, x, y, z, i);
			transformations.push_back(new_transformation);
		} else if (type == "translate") {
			cin >> x >> y >> z;
			Transformation new_transformation(TRANSLATE, x, y, z, i);
			transformations.push_back(new_transformation);
		}
	}

	cin >> num_objects;
	for (int i = 0; i < num_objects; i++) {
		Object new_object;
		new_object.id = i;
		cin >> new_object.pigment
			  >> new_object.finish;
		cin >> num_trans;
		for (int j = 0; j < num_trans; j++) {
			cin >> next_trans;
			new_object.trans.push_back(next_trans);
		}
		cin >> type;
		if (type == "sphere") {
			Sphere* new_obj = new Sphere(new_object);
			new_obj->type = SPHERE;
			cin >> x >> y >> z;
			new_obj->origin = glm::vec3(x, y, z);
			cin >> x;
			new_obj->radius = glm::vec3(x, x, x);
			new_obj->transform(transformations);
			cout << glm::to_string(new_obj->origin) << endl;
			cout << glm::to_string(new_obj->radius) << endl;
			objects.push_back(new_obj);
		} else if (type == "plane"){
			Polyhedron* new_obj = new Polyhedron(new_object);
			new_obj->type = POLYHEDRON;
			cin >> x >> y >> z >> w;
			new_obj->planes.push_back(glm::vec4(x, y, z, w));
			new_obj->transform(transformations);
			objects.push_back(new_obj);
		} else if (type == "polyhedron") {
			Polyhedron* new_obj = new Polyhedron(new_object);
			new_obj->type = POLYHEDRON;
			cin >> num_planes;
			for (int j = 0; j < num_planes; j++) {
				cin >> x >> y >> z >> w;
				new_obj->planes.push_back(glm::vec4(x, y, z, w));
			}
			new_obj->transform(transformations);
			objects.push_back(new_obj);
		} else if (type == "trianglemesh") {
			Trianglemesh* new_obj = new Trianglemesh(new_object);
			new_obj->type = TRIANGLEMESH;
		  if(!read_triangle_mesh(new_obj)){ // function in object.h
		  	cerr << "Error when reading trianglemesh" << endl; 
        exit(1);
		  }
		  new_obj->transform(transformations);
      objects.push_back(new_obj);
    }
	}
}

