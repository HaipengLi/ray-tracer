#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "global.h"

// abstract class Object
class Object{
public:

    Object(){}

    virtual ~Object() {}

    virtual bool has_point(const Point &p) const { return false; }
    virtual RGB_float get_ambient(const Point &p) const = 0;
    virtual RGB_float get_diffuse(const Point &p) const { return vec3(); };
    virtual RGB_float get_specular(const Point &p) const { return vec3(); };
    
    // this number [0,1] determines how much 
    // reflected light contributes to the color
    // of a pixel
    virtual float get_reflectance(const Point &p) const { return 0; };
    virtual float get_shineness(const Point &p) const { return 0; };

    virtual float get_transparency(const Point &p) const { return 0; };
    virtual float get_refractive_index(const Point &p) const { return 0; };

    virtual bool intersect(const Point &p, const Vector &v, float &t) const { return false; };

    virtual bool is_visible_from(const Point &p, const Point &view_point) const { return false; };

    virtual Vector get_normal(const Point &p) const { return Vector(); };

};

#endif