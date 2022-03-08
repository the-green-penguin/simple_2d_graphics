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
#include <memory>

///#define GLFW_INCLUDE_NONE     // not needed/working on Ubuntu, etc.
// #include <glad/gl.h>     // not needed/working on Ubuntu, etc.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>   // sudo apt install libglm-dev
#include <glm/gtc/matrix_transform.hpp>

#include "shader_program.h"



struct Vertex{
  glm::vec3 position;
  glm::vec3 colour;
};



//------------------------------------------------------------------------------
class GObject{
public:
  GObject(glm::vec3 position);
  ~GObject();
  virtual void render(std::shared_ptr<Shader_Program> shader_program) = 0;
  void set_position(glm::vec3 pos);
  
protected:
  glm::vec3 position = {0.0f, 0.0f, 0.0f};
};



//------------------------------------------------------------------------------
class GShape: public GObject{
public:
  bool buffers_ready = false;
  
  GShape(
    glm::vec3 position,
    float rotation,
    const std::vector<Vertex>& vertices
  );
  ~GShape();
  virtual void setup_vertex_buffer();
  virtual void render(std::shared_ptr<Shader_Program> shader_program);
  void set_rotation(float rot);
  
protected:
  GLuint vertex_buffer, vertex_array_object;
  std::vector<Vertex> vertices;
  std::size_t index_count;
  float rotation = 0.0f;  // degrees
  
  virtual void model_transformation(std::shared_ptr<Shader_Program> shader_program);
};



//------------------------------------------------------------------------------
class GTriangle: public GShape{
public:
  GTriangle(
    glm::vec3 position,
    float rotation,
    const std::vector<Vertex>& vertices
  );
  GTriangle(
    glm::vec3 position,
    const std::vector<Vertex>& vertices
  );
  GTriangle(
    glm::vec3 position,
    float rotation,
    float size,
    glm::vec3 colour
  );
  GTriangle(
    glm::vec3 position,
    float size,
    glm::vec3 colour
  );
  ~GTriangle();
  
protected:
};