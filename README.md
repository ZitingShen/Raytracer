# Raytracer

Ziting Shen & Qin Yang

All basic and intermediate features implemented:

0. anti-aliasing enabled by default
1. default trace depth = 6

Extra Credit:
0. Extra primitives:
  Ellipsoid, it shares the same primitive class with Sphere
  Implementation of Cylinder and Cone is incomplete
  Example: test5_ellipse.ppm
  usage:
  ellipsoid x y z a b c # an ellipsoid defined as xˆ2/aˆ2 + yˆ2/bˆ2 + zˆ2/cˆ2 = 1
1. Translate and Scale:
  Works on plane, sphere, triangle mesh, and, ellipsoid
2. Triangle Mesh:
  Works Properly:
  Normal is manually interpolated using weighed vertex normal depending on the distance to each vertex
3. Image textured objects:
  Rectangular textures could be mapped onto sphere (haven't tested on other), but as unlike skybox there are places of incongruous conjucture
  Example: test1.ppm


Efficiency - Most complex scene rendered:


Debug Mode:
