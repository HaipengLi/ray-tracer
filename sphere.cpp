#include "sphere.h"
#include <stdlib.h>
#include <limits>
#include <math.h>
/**********************************************************************
 * This function intersects a ray with a given sphere 'sph'. You should
 * use the parametric representation of a line and do the intersection.
 * The function should return the parameter value for the intersection, 
 * which will be compared with others to determine which intersection
 * is closest. The value -1.0 is returned if there is no intersection
 *
 * If there is an intersection, the point of intersection should be
 * stored in the "hit" variable
 **********************************************************************/
float intersect_sphere(Point o, Vector u, Spheres *sph, Point *hit) {
  Vector center_to_o = get_vec(sph->center, o);
  // construct quadratic equation a, b, c
  float a = vec_dot(u, u), b = 2 * vec_dot(u, center_to_o), c = vec_dot(center_to_o, center_to_o) - sph->radius * sph->radius;
  float delta = b * b - 4 * a * c;
  if (delta < 0) {
    return -1;
  }
  float t = -1, t_1, t_2;
  // t_1 is smaller
  t_1 = (-b - sqrt(delta)) / (2 * a);
  if (t_1 >= 0) {
    t = t_1;
  } else {
    t_2 = (-b + sqrt(delta)) / (2 * a);
    if (t_2 >= 0) {
      t = t_2;
    }
  }
  if (t != -1) *hit = get_point(o, vec_scale(u, t));
  return t;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
// FIXME: int what_is_this
Spheres *intersect_scene(Point o, Vector u, Spheres *slist, Point* hit_min, int what_is_this) {
  hit_min = NULL;
  Spheres *p = slist;
  float t_min = std::numeric_limits<float>::max(), t_temp;
  Point* hit;
  Spheres *sphere_min = NULL;
  while (p != NULL) {
      t_temp = intersect_sphere(o, u, p, hit);
      if (t_temp < t_min) {
        t_min = t_temp;
        sphere_min = p;
        hit_min = hit;
      }
      p = p->next;
  }
	return sphere_min;
}

/*****************************************************
 * This function adds a sphere into the sphere list
 *
 * You need not change this.
 *****************************************************/
Spheres *add_sphere(Spheres *slist, Point ctr, float rad, float amb[],
		    float dif[], float spe[], float shine, 
		    float refl, int sindex) {
  Spheres *new_sphere;

  new_sphere = (Spheres *)malloc(sizeof(Spheres));
  new_sphere->index = sindex;
  new_sphere->center = ctr;
  new_sphere->radius = rad;
  (new_sphere->mat_ambient)[0] = amb[0];
  (new_sphere->mat_ambient)[1] = amb[1];
  (new_sphere->mat_ambient)[2] = amb[2];
  (new_sphere->mat_diffuse)[0] = dif[0];
  (new_sphere->mat_diffuse)[1] = dif[1];
  (new_sphere->mat_diffuse)[2] = dif[2];
  (new_sphere->mat_specular)[0] = spe[0];
  (new_sphere->mat_specular)[1] = spe[1];
  (new_sphere->mat_specular)[2] = spe[2];
  new_sphere->mat_shineness = shine;
  new_sphere->reflectance = refl;
  new_sphere->next = NULL;

  if (slist == NULL) { // first object
    slist = new_sphere;
  } else { // insert at the beginning
    new_sphere->next = slist;
    slist = new_sphere;
  }

  return slist;
}

/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = get_vec(sph->center, q);
  normalize(&rc);
  return rc;
}
