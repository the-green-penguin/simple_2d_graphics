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
#include <vector>

///#define GLFW_INCLUDE_NONE     // not needed/working on Ubuntu, etc.
// #include <glad/gl.h>     // not needed/working on Ubuntu, etc.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>   // sudo apt install libglm-dev



class Shader_Program{
public:
  Shader_Program(const std::vector< std::string >& file_names);
  ~Shader_Program();
  void use();
  void set_uni(const std::string& name ,float x);
  void set_uni(const std::string& name ,double x);
  void set_uni(const std::string& name ,int x);
  void set_uni(const std::string& name ,unsigned int x);
  void set_uni(const std::string& name ,float x, float y);
  void set_uni(const std::string& name ,double x, double y);
  void set_uni(const std::string& name ,int x, int y);
  void set_uni(const std::string& name ,unsigned int x, unsigned int y);
  void set_uni(const std::string& name ,float x, float y, float z);
  void set_uni(const std::string& name ,double x, double y, double z);
  void set_uni(const std::string& name ,int x, int y, int z);
  void set_uni(const std::string& name ,unsigned int x, unsigned int y, unsigned int z);
  void set_uni(const std::string& name ,float x, float y, float z, float w);
  void set_uni(const std::string& name ,double x, double y, double z, double w);
  void set_uni(const std::string& name ,int x, int y, int z, int w);
  void set_uni(const std::string& name ,unsigned int x, unsigned int y, unsigned int z, unsigned int w);
  void set_uni(const std::string& name, const glm::mat2& matrix);
  void set_uni(const std::string& name, const glm::mat3& matrix);
  void set_uni(const std::string& name, const glm::mat4& matrix);
  void set_uni(const std::string& name, const glm::mat2x3& matrix);
  void set_uni(const std::string& name, const glm::mat3x2& matrix);
  void set_uni(const std::string& name, const glm::mat2x4& matrix);
  void set_uni(const std::string& name, const glm::mat4x2& matrix);
  void set_uni(const std::string& name, const glm::mat3x4& matrix);
  void set_uni(const std::string& name, const glm::mat4x3& matrix);
  
private:
  int gl_success;
  char info_log[512];
  std::vector< GLuint > shaders;
  GLuint shader_program;

  void load_shader(const std::string& file_name);   // file name extension -> shader type
  void compile_shader_program();
  GLenum get_shader_type(const std::string& file_name);
  void compile_shader(GLenum shader_type, const std::string& shader_source);
};
