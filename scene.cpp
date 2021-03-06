//
// this provide functions to set up the scene
//
#include "scene.h"
#include "sphere.hpp"
#include "chessboard.hpp"
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <limits>
#include <math.h>
#include <vector>

using namespace std;

extern Point light1;
extern vec3 light1_ambient;
extern vec3 light1_diffuse;
extern vec3 light1_specular;

extern vec3 global_ambient;
extern vector<Object*> scene;

extern RGB_float background_clr;
extern float decay_a;
extern float decay_b;
extern float decay_c;


//////////////////////////////////////////////////////////////////////////

/*******************************************
 * set up the default scene - DO NOT CHANGE
 *******************************************/
void destroy_scene() {
  for (auto p: scene) {
    delete p;
  }
}
void set_up_default_scene(int chessboard_on) {
  // set background color
  background_clr.x = 0.5;
  background_clr.y = 0.05;
  background_clr.z = 0.8;

  // setup global ambient term
  global_ambient[0] = global_ambient[1] = global_ambient[2] = 0.2;

  // setup light 1
  light1.x = -2.0;
  light1.y = 5.0;
  light1.z = 1.0;
  light1_ambient[0] = light1_ambient[1] = light1_ambient[2] = 0.1;
  light1_diffuse[0] = light1_diffuse[1] = light1_diffuse[2] = 1.0;
  light1_specular[0] = light1_specular[1] = light1_specular[2] = 1.0;

  // set up decay parameters
  decay_a = 0.5;
  decay_b = 0.3;
  decay_c = 0.0;

  // sphere 1
  Point sphere1_ctr = Point(1.5, -0.2, -3.2);
  float sphere1_rad = 1.23;
  RGB_float sphere1_ambient = {0.7, 0.7, 0.7};
  RGB_float sphere1_diffuse = {0.1, 0.5, 0.8};
  RGB_float sphere1_specular = {1.0, 1.0, 1.0};
  float sphere1_shineness = 10;
  float sphere1_reflectance = 0.4;
  float sphere1_transparency = 0.4;
  float sphere1_refractive_index = 1.52;
  scene.push_back(new Sphere(sphere1_ctr, sphere1_rad, sphere1_ambient,
             sphere1_diffuse, sphere1_specular, sphere1_shineness,
		     sphere1_reflectance, sphere1_transparency, sphere1_refractive_index));

  // sphere 2
  Point sphere2_ctr = Point(-1.5, 0.0, -3.5);
  float sphere2_rad = 1.5;
  RGB_float sphere2_ambient = {0.6, 0.6, 0.6};
  RGB_float sphere2_diffuse = {1.0, 0.0, 0.25};
  RGB_float sphere2_specular = {1.0, 1.0, 1.0};
  float sphere2_shineness = 6;
  float sphere2_reflectance = 0.3;
  float sphere2_transparency = 0.5;
  float sphere2_refractive_index = 1.52;
  scene.push_back(new Sphere(sphere2_ctr, sphere2_rad, sphere2_ambient,
             sphere2_diffuse, sphere2_specular, sphere2_shineness,
		     sphere2_reflectance, sphere2_transparency, sphere2_refractive_index));

  // sphere 3
  Point sphere3_ctr = Point(-0.35, 1.75, -2.25);
  float sphere3_rad = 0.5;
  RGB_float sphere3_ambient = {0.2, 0.2, 0.2};
  RGB_float sphere3_diffuse = {0.0, 1.0, 0.25};
  RGB_float sphere3_specular = {0.0, 1.0, 0.0};
  float sphere3_shineness = 30;
  float sphere3_reflectance = 0.3;
  float sphere3_transparency = 0.3;
  float sphere3_refractive_index = 1.52;
  scene.push_back(new Sphere(sphere3_ctr, sphere3_rad, sphere3_ambient,
    sphere3_diffuse, sphere3_specular, sphere3_shineness,
		  sphere3_reflectance, sphere3_transparency, sphere3_refractive_index));

  // chessboard
  if (chessboard_on) {
    Point chessboard_middle_point = Point(0, -4, -6);
    float chessboard_width = 10;
    int chessboard_grid_num = 8;
    RGB_float chessboard_ambient = {0.2, 0.2, 0.2};
    RGB_float chessboard_specular = {1, 1, 1};
    float chessboard_shineness = 10;
    float chessboard_reflectance = 0.3;
    float chessboard_transparency = 0;
    float chessboard_refractive_index = 1;
    scene.push_back(new Chessboard(chessboard_middle_point, chessboard_width, chessboard_grid_num,
      chessboard_ambient, chessboard_specular, chessboard_shineness,
      chessboard_reflectance, chessboard_transparency, chessboard_refractive_index));
    }

}

/***************************************
 * You can create your own scene here
 ***************************************/
void set_up_user_scene() {
}

bool intersect_scene(const Point &o, const Vector &u, Point &hit_point, Object *&hit_obj) {
  bool success = false;
  float t_min = std::numeric_limits<float>::max(), t_temp;
  for (unsigned i = 0; i < scene.size(); ++i) {
    if (scene[i]->intersect(o, u, t_temp) && t_temp < t_min) {
      success = true;
      t_min = t_temp;      
      hit_obj = scene[i];
      hit_point = o + u * t_min;
    }
  }
  return success;
}

bool is_blocked(const Object *object, const Point o, const Point d) {
  // is there any other objects on the way between o and d? (o is on the object)
  Vector v = normalize(d - o);
  float t;
  bool intersected;
  for (unsigned i = 0; i < scene.size(); i++) {
    // jump itself
    if (object == scene[i]) {
      continue;
    }
    if (scene[i]->intersect(o, v, t)) {
      // if the intersection is between o and d
      if (length(d - o) > length(t * v)) {
        return true;
      }
    }
  }
  return false;
}