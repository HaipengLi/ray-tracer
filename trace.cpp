#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "global.h"
#include "scene.h"
#include "sphere.hpp"

using namespace std;
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

extern vector<Object*> scene;

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
extern int super_sampling_on;
extern int scochatic_on;
extern int chessboard_on;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
RGB_float phong(Point q, Vector v, Vector surf_norm, Object *obj) {
  // q: point on obj
  // v: vector from q to eye
  float d = length(q - light1);  // distance between light and object
  vec3 norm = {surf_norm.x, surf_norm.y, surf_norm.z};
  vec3 l =  normalize(light1 - q);  // p to light source
  vec3 r = normalize(2 * dot(norm, l) * norm - l);  // reflect of l

  // global ambient
  vec3 ga_rgb = global_ambient * obj->get_ambient(q);
  if (!obj->is_visible_from(q, light1)) {
    // if this point q not visible from light, only show ga
    return ga_rgb;
  }
  // ambient
  vec3 a_rgb = light1_ambient * obj->get_ambient(q);
  // diffuse
  vec3 d_rgb = light1_diffuse * obj->get_diffuse(q) * (dot(norm, l));
  if (dot(norm, l) < 0) {
    // cout << "Warning: Should not happen 1\n";
    d_rgb = 0;
  }
  // specular
  vec3 s_rgb = light1_specular * obj->get_specular(q) * pow(dot(r, v), obj->get_shineness(q));
  if (dot(r, v) < 0) {
    s_rgb = 0;
  }

	RGB_float color;
  // add all terms
  color = ga_rgb + a_rgb + 1.0 / (decay_a + decay_b * d + decay_c * d * d) * (d_rgb + s_rgb);

	return color;
}

float random_float(int lo, int hi) {
    return ((float)rand() / (float)RAND_MAX) * (hi - lo) + lo;
}

Vector random_vector(Vector norm) {
  while (true) {
    Vector v = vec3(random_float(-1, 1), random_float(-1, 1), random_float(-1, 1));
    if (dot(norm, v) > 0) {
      return normalize(v);
    }
  }
}



/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
RGB_float recursive_ray_trace(Point o, Vector i, int depth) {
  // exit of recursion
  if (depth == 0) return 0;
  i = normalize(i);

  // find the intersecion
  Point hit_point;
  Object* hit_obj;
  if (!intersect_scene(o, i, hit_point, hit_obj)) {
    // no intersection, return background color
    return background_clr;
  }

  Vector hit_norm = hit_obj->get_normal(hit_point);
  bool inside = (dot(i, hit_norm) > 0);
  if (inside) {
    // norm should be -norm inside the sphere
    hit_norm = -hit_norm;
  }


  // compute the color of intersection by THREE parts
  // 1. shadow ray: phong local illumination?
  RGB_float shadow_ray_rgb;
  if (shadow_on && is_blocked(hit_obj, hit_point + hit_norm * BIAS, light1)) {
    // if blocked by other sphere, only show global ambient
    shadow_ray_rgb = global_ambient * hit_obj->get_ambient(hit_point);
  } else {
    // else use phong
    shadow_ray_rgb = phong(hit_point, normalize(-i), hit_norm, hit_obj);
  }

  // 2. reflected ray
  RGB_float reflected_ray_rgb = 0;
  if (reflection_on) {
    Vector r = normalize(2 * dot(-i, hit_norm) * hit_norm + i);
    reflected_ray_rgb = recursive_ray_trace(hit_point + hit_norm * BIAS, r, depth - 1);
    if (reflected_ray_rgb.x < 0 || reflected_ray_rgb.y < 0 || reflected_ray_rgb.z < 0) {
      std::cout << "Warning: Negative reflected color\n";
    }
  }

  // 3. refracted ray
  RGB_float refracted_ray_rgb = 0;
  if (refraction_on && hit_obj->get_transparency(hit_point) > 0) {
    // https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
    // payattention to refractive index inside / outside the sphere
    float cos_theta_i = -dot(hit_norm, i);
    float refractive_index = inside ? hit_obj->get_refractive_index(hit_point) : 1.0 / hit_obj->get_refractive_index(hit_point);
    Vector t = refractive_index * i + 
      (refractive_index * cos_theta_i - sqrt(1 - pow(refractive_index, 2) * (1 - pow(cos_theta_i, 2)))) * hit_norm;
    refracted_ray_rgb = recursive_ray_trace(hit_point - hit_norm * BIAS, t, depth - 1);
  }

  // 4. scochatic rays
  RGB_float scochatic_ray_rgb = 0;
  if (scochatic_on) {
    for (int index = 0; index < STOCHASTIC_RAY_NUM; index++) {
      Vector s_ray = random_vector(hit_norm);
      scochatic_ray_rgb += recursive_ray_trace(hit_point + hit_norm * BIAS, s_ray, depth - 1); 
    }
  }

  // sum ray
	RGB_float color = shadow_ray_rgb + 
    reflected_ray_rgb * hit_obj->get_reflectance(hit_point) + 
    refracted_ray_rgb * hit_obj->get_transparency(hit_point) * (1 - hit_obj->get_reflectance(hit_point)) +
    scochatic_ray_rgb * 0.1 ;
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

      float x_quarter_grid = x_grid_size / 4;
      float y_quarter_grid = y_grid_size / 4;
      if (super_sampling_on) {
        // std::cout << ray<< "\n";
        // std::cout << ray + Vector(-x_quarter_grid, -y_quarter_grid, 0) << "\n";
        ret_color += recursive_ray_trace(eye_pos, ray + Vector(-x_quarter_grid, -y_quarter_grid, 0), step_max + 1);
        ret_color += recursive_ray_trace(eye_pos, ray + Vector(x_quarter_grid, -y_quarter_grid, 0), step_max + 1);
        ret_color += recursive_ray_trace(eye_pos, ray + Vector(-x_quarter_grid, y_quarter_grid, 0), step_max + 1);
        ret_color += recursive_ray_trace(eye_pos, ray + Vector(x_quarter_grid, y_quarter_grid, 0), step_max + 1);
        ret_color /= 5.0;
      }

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
