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

#include <vector>

///#define GLFW_INCLUDE_NONE     // not needed/working on Ubuntu, etc.
// #include <glad/gl.h>     // not needed/working on Ubuntu, etc.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>   // sudo apt install libglm-dev



struct Vertex{
  glm::vec3 position;
  glm::vec3 colour;
};



//------------------------------------------------------------------------------
class GObject{
public:
  GObject(glm::vec3 position);
  ~GObject();
  virtual void render() = 0;
  
protected:
  glm::vec3 position;
};



//------------------------------------------------------------------------------
class GShape: public GObject{
public:
  GShape(
    glm::vec3 position,
    const std::vector<Vertex>& vertices
  );
  ~GShape();
  virtual void render();
  
protected:
  GLuint vertex_buffer, vertex_array_object;
  std::size_t index_count;
  
  void setup_vertex_buffer(const std::vector<Vertex>& vertices);
};



//------------------------------------------------------------------------------
class GTriangle: public GShape{
public:
  GTriangle(
    glm::vec3 position,
    const std::vector<Vertex>& vertices
  );
  ~GTriangle();
  
protected:
};