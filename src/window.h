/*

MIT License

Copyright (c) 2022 the_green_penguin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


*/

#pragma once

#include <string>
#include <memory>
#include <vector>

///#define GLFW_INCLUDE_NONE     // not needed/working on Ubuntu, etc.
// #include <glad/gl.h>     // not needed/working on Ubuntu, etc.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader_program.h"
#include "graphics_object.h"



// .cpp contains non-member function!
class Window{
public:
  Window(const std::string& window_name);
  ~Window();
  void run();   // run as separate thread
  void add_gobject(const std::shared_ptr<GObject>& gobject);
  
private:
  GLFWwindow* window;
  std::string window_name;
  int width, height;
  std::shared_ptr<Shader_Program> shader_program;
  std::vector<
    std::shared_ptr<GObject>
  > graphics_objects;
  
  void test();
  void setup_glfw();
  void setup_glew();
  void setup_glfw_debugging();
  void setup_shader_program();
  void render();
};
