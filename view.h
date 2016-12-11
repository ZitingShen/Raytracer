#ifndef VIEW_H
#define VIEW_H

#include <glm/glm.hpp>

class View {
  public:
    glm::vec3 camera, at, up;
    float fovy;
    float w, h;
    glm::vec3 camera_x, camera_y, camera_z;
    float pixel;
};
#endif
