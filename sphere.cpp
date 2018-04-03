#include "sphere.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>
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
  // assert the point o is not inside the sphere
  // assert(length(o - sph->center) >= sph->radius);
  if (fabs(length(o - sph->center) - sph->radius) < 1e-3) {
    // the point is on the sphere: two case
    if (dot(u, sph->center - o) > 0) {
      // 1. u is to the sphere, intersection: o 
      std::cout << "Warning: self reflection??\n";
      *hit = o;
      return 0;
    } else {
      // 2. u is off the sphere, intersection: NULL
      return -1;
    }
  }
  if (length(o - sph->center) < sph->radius) {
    // should not happen in this assignment
    std::cout << "Warning: view point inside the sphere; inside distance is " << 
      fabs(length(o - sph->center) - sph->radius) << "\n";
  }
  Vector center_to_o = o - sph->center;
  // construct quadratic equation a, b, c
  float a = dot(u, u), b = 2 * dot(u, center_to_o), c = dot(center_to_o, center_to_o) - sph->radius * sph->radius;
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
  // FIXME: bugs of hit
  if (t != -1) {
    *hit = u * t + o;  //get_point(o, vec_scale(u, t))
  }
  return t;
}

/*********************************************************************
 * This function returns a pointer to the sphere object that the
 * ray intersects first; NULL if no intersection. You should decide
 * which arguments to use for the function. For exmaple, note that you
 * should return the point of intersection to the calling function.
 **********************************************************************/
Spheres *intersect_scene(Point o, Vector u, Spheres *slist, Point* hit_min) {
  Spheres *p = slist;
  float t_min = std::numeric_limits<float>::max(), t_temp;
  Point hit;
  Spheres *sphere_min = NULL;
  while (p != NULL) {
      t_temp = intersect_sphere(o, u, p, &hit);
      if (t_temp != -1 && t_temp < t_min) {
        t_min = t_temp;
        // make sure the point is on the sphere
        assert(fabs(length(p->center - hit) - p->radius) < 1e-3);
        // make sure the point is visible
        assert(dot(normalize(u), sphere_normal(hit, p)) <= 1e-3);
        sphere_min = p;
        *hit_min = hit;
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

bool isBlocked(Spheres *sph, Point o, Point d, Spheres *slist) {
  // is there any other spheres on the way between o and d?
  Vector v = d - o;
  Point hit;
  float t;
  for (Spheres *p = slist; p != NULL; p = p->next) {
    // special case: sphere itself
    if (p == sph) {
      if (dot(sph->center - d, sph->center - o) < 0) {
        return true;
      } else {
        continue;
      }
    }
    t = intersect_sphere(o, v, p, &hit);
    if (t > 0 && t < 1) {
      // t is (0, 1), then the intersection is between o and d
      return true;
    }
  }
  return false;
}
/******************************************
 * computes a sphere normal - done for you
 ******************************************/
Vector sphere_normal(Point q, Spheres *sph) {
  Vector rc;

  rc = q - sph->center;
  rc = normalize(rc);
  return rc;
}
