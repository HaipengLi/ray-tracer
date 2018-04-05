#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include "object.hpp"
class Chessboard: public Object {
    float width;
    // suppose the y of chessboard is always the same
    Point middle_point;
    int grid_num;

    RGB_float ambient;
    RGB_float specular;
    float shineness;

    // this number [0,1] determines how much 
    // refracted light contributes to the color
    // of a pixel
    float reflectance;       
    float transparency;
    float refractive_index;

public:
    Chessboard(Point middle_point, float width, int grid_num,
        RGB_float diffuse, RGB_float specular, float shineness, 
        float reflectance, float transparency, float refractive_index):
        middle_point(middle_point), width(width), grid_num(grid_num),
        ambient(ambient), specular(specular), shineness(shineness),
        reflectance(reflectance), transparency(transparency), refractive_index(refractive_index) {}

    ~Chessboard() {}

    bool has_point(const Point &p) const {
        float x_min = this->middle_point[0] - this->width / 2;
        float x_max = this->middle_point[0] + this->width / 2;
        float z_min = this->middle_point[2] - this->width / 2;
        float z_max = this->middle_point[2] + this->width / 2;
        return (p[0] <= x_max && p[0] >= x_min &&
            p[2] <= z_max && p[2] >= z_min);
    }

    RGB_float get_ambient(const Point &p) const { return ambient; }
    RGB_float get_diffuse(const Point &p) const {
        float grid_size = width / grid_num;
        int x_no = p[0] - this->middle_point[0] / grid_size;
        int z_no = p[2] - this->middle_point[2] / grid_size;
        RGB_float color = RGB_float(0), white = RGB_float(1);
        if (x_no % 2 == 0) {
            color = white;
        }
        if (z_no % 2 == 0) {
            color = white - color;
        }
        return color;
    }
    RGB_float get_specular(const Point &p) const { return specular; }
    
    // this number [0,1] determines how much 
    // reflected light contributes to the color
    // of a pixel
    float get_reflectance(const Point &p) const { return reflectance; }
    float get_shineness(const Point &p) const { return shineness; }

    float get_transparency(const Point &p) const { return transparency; }
    float get_refractive_index(const Point &p) const { return refractive_index; }

    bool intersect(const Point &p, const Vector &v, float &t) const {
        float y = this->middle_point[1];
        t = (y - p[1]) / v[1];
        if (t > 0 && this->has_point(p + t * v)) {
            return true;
        }
        return false;
    }


    bool is_visible_from(const Point &p, const Point &view_point) const {
        // p is on the object
        // if p is visible from view_point?
        // (is p shadowed by the object itself?)
        return true;
    }

    Vector get_normal(const Point &p) const { return normalize(Vector(0, 1, 0)); }
};

#endif