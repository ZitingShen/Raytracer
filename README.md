# Raytracer

Ziting Shen & Qin Yang

usage:

./main < input.in

All basic and intermediate features implemented:

0. anti-aliasing enabled by default
1. default trace depth = 6

Extra Credit:
0. Extra primitives:
  Ellipsoid, it shares the same primitive class with Sphere
  Implementation of Cylinder and Cone is incomplete
  Example: test1.ppm, test5_ellipse.ppm
  usage:
  ellipsoid x y z a b c # an ellipsoid defined as xˆ2/aˆ2 + yˆ2/bˆ2 + zˆ2/cˆ2 = 1

1. Translate and Scale:
  Works on plane, sphere, triangle mesh, and, ellipsoid

2. Triangle Mesh:
  Works Properly
  Distortion at the boundary. Not sure why.
  Normal is manually interpolated using weighed vertex normal depending on the distance to each vertex

3. Image textured objects:
  Rectangular textures could be mapped onto sphere (haven't tested on other), but as unlike skybox there are places of incongruous conjucture
  Example: test1.ppm

Efficiency - Most complex scene rendered:

Without -O2 enabled:
0.Bunny.off with trace depth 3: took about 2 hours to render on my early 2015 rmbp '13

With -O2 enabled:
1. Kitten with trace depth of 6: took about 3 mins to render
2. Elephant

Debug Mode:

In main.cc

#define DEBUG_I                   -1
#define DEBUG_J                   -1

Determines which ray to look at (with anti-aliasing enabled, will be nine rays);
Debug mode is enabled when DEBUG_I and DEBUG_J are set to correct range;
Debug mode is disabled when DEBUG_I and DEBUG_J are out of range (e.g. when both are -1)
