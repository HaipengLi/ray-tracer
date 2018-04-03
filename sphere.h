#ifndef _SPHERE_H_
#define _SPHERE_H_
/**********************************************************************
 * Some stuff to handle spheres
 **********************************************************************/
#include "global.h"
// #include "vector.h"

typedef struct sphere {
  int index;               // identifies a sphere; must be greater than 0

  Point center;            // Point: x, y, z
  float radius;

  vec3 mat_ambient;    // material property used in Phong model
  vec3 mat_diffuse;
  vec3 mat_specular;
  float mat_shineness;

  float reflectance;       // this number [0,1] determines how much 
                           // reflected light contributes to the color
                           // of a pixel
  struct sphere *next;
} Spheres;   // a list of spheres

// is
bool isBlocked(Spheres *sph, Point o, Point p, Spheres *sph_list);
// intersect ray with sphere
Spheres *intersect_scene(Point, Vector, Spheres *, Point *);
// return the unit normal at a point on sphere
Vector sphere_normal(Point, Spheres *);
// add a sphere to the sphere list
Spheres *add_sphere(Spheres *, Point, float, float [], float [], float [], float, float, int);

#endif
