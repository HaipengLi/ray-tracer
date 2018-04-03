#include <iostream>
#include <stdio.h>
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
extern int reflection_on;
extern int refraction_on;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector surf_norm, Spheres *sph) {

  float d = length(q - light1);  // distance between light and object
  vec3 norm = {surf_norm.x, surf_norm.y, surf_norm.z};
  vec3 l =  normalize(light1 - q);  // p to light source
  vec3 r = normalize(2 * dot(norm, l) * norm - l);  // reflect of l

  // global ambient
  vec3 ga_rgb = global_ambient * sph->mat_ambient;
  // ambient
  vec3 a_rgb = light1_ambient * sph->mat_ambient;
  // diffuse
  vec3 d_rgb = light1_diffuse * sph->mat_diffuse * (dot(norm, l));
  if (dot(norm, l) < 0) {
    d_rgb = 0;
  }
  // specular
  vec3 s_rgb = light1_specular * sph->mat_specular * pow(dot(r, v), sph->mat_shineness);
  if (dot(r, v) < 0) {
    s_rgb = 0;
  }

	RGB_float color;
  // add all terms
  color = ga_rgb + a_rgb + 1.0 / (decay_a + decay_b * d + decay_c * d * d) * (d_rgb + s_rgb);

	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point o, Vector u, int depth) {
  // exit of recursion
  if (depth == 0) return 0;

  // find the intersecion
  Point intersection_point;
  Spheres *intersection_sphere = intersect_scene(o, u, scene, &intersection_point);

  // no intersection, return background color
  if (intersection_sphere == NULL) {
    return background_clr;
  }

  Vector intersection_norm = sphere_normal(intersection_point, intersection_sphere);


  // compute the color of intersection by THREE parts
  // 1. shadow ray: phong local illumination?
  RGB_float shadow_ray_rgb;
  if (shadow_on && isBlocked(intersection_sphere, intersection_point, light1, scene)) {
    // if blocked by other sphere, only show global ambient
    shadow_ray_rgb = global_ambient * intersection_sphere->mat_ambient;
  } else {
    // else use phong
    shadow_ray_rgb = phong(intersection_point, normalize(-u), sphere_normal(intersection_point, intersection_sphere), intersection_sphere);
  }

  // 2. reflected ray
  RGB_float reflected_ray_rgb = 0;
  if (reflection_on) {
    Vector r = normalize(2 * dot(-u, intersection_norm) * intersection_norm + u);
    reflected_ray_rgb = recursive_ray_trace(intersection_point, r, depth - 1);
    if (reflected_ray_rgb.x < 0 || reflected_ray_rgb.y < 0 || reflected_ray_rgb.z < 0) {
      std::cout << "Warning: Negative reflected color\n";
    }
  }

  // 3. refracted ray
  RGB_float refracted_ray_rgb = 0;

  // sum ray
	RGB_float color = shadow_ray_rgb + 
    reflected_ray_rgb * intersection_sphere->reflectance + 
    refracted_ray_rgb;
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
      ray = cur_pixel_pos - eye_pos;

      //
      // You need to change this!!!
      //
      // ret_color = recursive_ray_trace();
      ret_color = background_clr; // just background for now

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;
      ret_color = recursive_ray_trace(eye_pos, ray, step_max + 1);

      // Checkboard for testing
      // RGB_float clr = {float(i/32), 0, float(j/32)};
      // ret_color = clr;


      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
