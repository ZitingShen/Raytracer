#include "transformation.h"
#include <glm/gtx/transform.hpp>

Transformation::Transformation(Transformation_type type, float x, float y, 
	float z, int id) {
	this->type = type;
	this->id = id;
	if (type == SCALE) {
		description = glm::scale(glm::vec3(x, y, z));
	} else if (type == TRANSLATE) {
		description = glm::translate(glm::vec3(x, y, z));
	}
}