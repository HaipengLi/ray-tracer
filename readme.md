# Ray Tracer
Implements a ray tracer to render a scene with highly specular reflections

# requirement
c++ 11
opengl 3.2
freeglut
glew

# build
Use make command to compile. make clean & make clean_object to clean.


# run

```
$ ./raycast [scene_option] [max_step] [additional option]
```
  * scene_option(-d):
    * -d : default scene with rendering sphere
  * max_step(a number):
    * use a number to define the maximum recursive depth of ray tracing program
  * additional option:
    * +s : add shadow effect
    * +l : add reflection effect
    * +r : add refraction effect
    * +c : add chessboard object to the scene
    * +f : add diffuse reflection effect
    * +p : add anti-aliasing effect