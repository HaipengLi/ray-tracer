#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "object.hpp"
#include <stdlib.h>
#include <assert.h>

class Sphere: public Object {
    Point center;
    float radius;
    float radius2;

    RGB_float ambient;    // material property used in Phong model
    RGB_float diffuse;
    RGB_float specular;
    float shineness;

    // this number [0,1] determines how much 
    // refracted light contributes to the color
    // of a pixel
    float reflectance;       
    float transparency;
    float refractive_index;

public:
    Sphere(Point center, float radius,
        RGB_float ambient, RGB_float diffuse, RGB_float specular, float shineness, 
        float reflectance, float transparency, float refractive_index):
        center(center), radius(radius), radius2(radius * radius),
        ambient(ambient), diffuse(diffuse), specular(specular), shineness(shineness),
        reflectance(reflectance), transparency(transparency), refractive_index(refractive_index) {}

    ~Sphere() {}

    bool has_point(const Point &p) const { return fabs(length(p - center) - radius) < 1e-4;  }

    RGB_float get_ambient(const Point &p) const { return ambient; }
    RGB_float get_diffuse(const Point &p) const { return diffuse; }
    RGB_float get_specular(const Point &p) const { return specular; }
    
    // this number [0,1] determines how much 
    // reflected light contributes to the color
    // of a pixel
    float get_reflectance(const Point &p) const { return reflectance; }
    float get_shineness(const Point &p) const { return shineness; }

    float get_transparency(const Point &p) const { return transparency; }
    float get_refractive_index(const Point &p) const { return refractive_index; }

    bool intersect(const Point &p, const Vector &v, float &t) const {
        // TODO: bug
        Vector l = center - p;
        float tca = dot(l, v);
        if (tca < 0) return false;
        float d2 = dot(l, l) - tca * tca;
        if (d2 > radius2) return false;
        float thc = sqrt(radius2 - d2);
        float t0 = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) {
            t0 = t1;
        }
        if (t0 < 0) {
            return false;
        } else {
            t = t0;
            return true;
        }
}


    bool is_visible_from(const Point &p, const Point &view_point) const {
        // p is on the object
        // if p is visible from view_point?
        // (is p shadowed by the object itself?)
        return (dot(this->center - view_point, this->center - p) >= 0);
    }

    Vector get_normal(const Point &p) const { return normalize(p - this->center); }
};

#endif