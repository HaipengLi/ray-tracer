#include "include/Angel.h"
#include <iostream>
#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "global.h"
#include "sphere.h"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern Point eye_pos;
extern float image_plane;
extern RGB_float background_clr;
extern RGB_float null_clr;

extern Spheres *scene;

// light 1 position and color
extern Point light1;
extern vec3 light1_ambient;
extern vec3 light1_diffuse;
extern vec3 light1_specular;

// global ambient term
extern vec3 global_ambient;

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector surf_norm, Spheres *sph) {

  float d = length(q - light1);  // distance between light and object
  vec3 norm = {surf_norm.x, surf_norm.y, surf_norm.z};
  vec3 l =  light1 - q;  // p to light source
  vec3 r = 2 * dot(norm, l) * norm - l;  // reflect of l

  // global ambient
  vec3 ga_rgb = global_ambient * sph->mat_ambient;
  // ambient
  vec3 a_rgb = light1_ambient * sph->mat_ambient;
  // diffuse
  vec3 d_rgb = light1_diffuse * sph->mat_diffuse * (dot(norm, l));
  if (d_rgb < 0) {
    std::cout << "Warning: diffuse term is negative!\n";
  }
  // specular
  vec3 s_rgb = light1_specular * sph->mat_specular * pow(dot(r, v), sph->mat_shineness);

	RGB_float color;
  // add all terms
  color = ga_rgb + a_rgb + 1.0 / (decay_a + decay_b * d + decay_c * d * d) * (d_rgb + s_rgb);

	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace() {
//
// do your thing here
//
	RGB_float color;
	return color;
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  RGB_float ret_color;
  Point cur_pixel_pos;
  Vector ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = get_vec(eye_pos, cur_pixel_pos);

      //
      // You need to change this!!!
      //
      // ret_color = recursive_ray_trace();
      ret_color = background_clr; // just background for now

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      // ret_color = recursive_ray_trace(cur_pixel_pos, ray, 1);

// Checkboard for testing
RGB_float clr = {float(i/32), 0, float(j/32)};
ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.r);
      frame[i][j][1] = GLfloat(ret_color.g);
      frame[i][j][2] = GLfloat(ret_color.b);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
