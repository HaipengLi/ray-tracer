#include "global.h"
#include "object.hpp"

void destroy_scene();
void set_up_default_scene(int);
void set_up_user_scene();
bool intersect_scene(const Point &o, const Vector &u, Point &hit_point, Object *&hit_obj);
bool is_blocked(const Object *object, const Point o, const Point d);