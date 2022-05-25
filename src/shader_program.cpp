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

#include "shader_program.h"

#include <iostream>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>



////////////////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////////////////

Shader_Program::Shader_Program(){
  shader_program = glCreateProgram();
  
  compile_shader(GL_VERTEX_SHADER, default_vert_shader);
  compile_shader(GL_FRAGMENT_SHADER, default_frag_shader);
    
  compile_shader_program();
}



//------------------------------------------------------------------------------
Shader_Program::Shader_Program(const std::vector< std::string >& file_names){
  shader_program = glCreateProgram();
  for(const auto &fn : file_names)
    load_shader(fn);
    
  compile_shader_program();
}



//------------------------------------------------------------------------------
Shader_Program::~Shader_Program(){}



//------------------------------------------------------------------------------
void Shader_Program::use(){
  glUseProgram(shader_program);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,float x){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform1f(uniform_location, x);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,double x){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform1d(uniform_location, x);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,int x){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform1i(uniform_location, x);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, uint x){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform1ui(uniform_location, x);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,float x, float y){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform2f(uniform_location, x, y);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,double x, double y){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform2d(uniform_location, x, y);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,int x, int y){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform2i(uniform_location, x, y);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, uint x, uint y){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform2ui(uniform_location, x, y);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,float x, float y, float z){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform3f(uniform_location, x, y, z);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,double x, double y, double z){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform3d(uniform_location, x, y, z);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,int x, int y, int z){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform3i(uniform_location, x, y, z);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, uint x, uint y, uint z){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform3ui(uniform_location, x, y, z);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,float x, float y, float z, float w){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform4f(uniform_location, x, y, z, w);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,double x, double y, double z, double w){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform4d(uniform_location, x, y, z, w);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name ,int x, int y, int z, int w){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform4i(uniform_location, x, y, z, w);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, uint x, uint y, uint z, uint w){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniform4ui(uniform_location, x, y, z, w);
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat2& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix2fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat3& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat4& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat2x3& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix2x3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat3x2& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix3x2fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat2x4& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix2x4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat4x2& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix4x2fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat3x4& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix3x4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



//------------------------------------------------------------------------------
void Shader_Program::set_uni(const std::string& name, const glm::mat4x3& matrix){
  GLint uniform_location = glGetUniformLocation(shader_program, name.c_str());
  glUniformMatrix4x3fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}



////////////////////////////////////////////////////////////////////////////////
// private
////////////////////////////////////////////////////////////////////////////////



void Shader_Program::load_shader(const std::string& file_name){
  // open file
  std::ifstream file;
  file.exceptions(std::ios::failbit | std::ios::badbit);   // without this, no exception will be thrown when ".open()" fails
  try{  file.open(file_name);  }
  catch(std::exception& f){
    throw std::runtime_error("Unable to open file '" + file_name + "'!");
  }
  
  // get file content
  std::string file_content(
    (std::istreambuf_iterator<char>(file)),
    (std::istreambuf_iterator<char>())
  );
  
  // compile
  compile_shader(
    get_shader_type(file_name),
    file_content
  );
    
  // cleanup
  file.close();
}



//------------------------------------------------------------------------------
void Shader_Program::compile_shader_program(){
  // attach & link shaders  
  for(const auto &s : shaders)
    glAttachShader(shader_program, s);
    
  glLinkProgram(shader_program);
  
  // check for errors
  glGetProgramiv(shader_program, GL_LINK_STATUS, &gl_success);
  if( ! gl_success) {
    glGetProgramInfoLog(shader_program, 512, NULL, info_log);
    std::cerr << "OpenGL returned:\n" << info_log << "\n";
    throw std::runtime_error("OpenGL: Failed to create shader program!");
  }
  
  // clean up memory
  for(const auto &s : shaders){
    glDetachShader(shader_program, s);
    glDeleteShader(s);
  }
}



//------------------------------------------------------------------------------
GLenum Shader_Program::get_shader_type(const std::string& file_name){
  if(file_name.ends_with(".vert"))
    return GL_VERTEX_SHADER;
  if(file_name.ends_with(".frag"))
    return GL_FRAGMENT_SHADER;
    
  throw std::runtime_error("Unable to detect shader type of '" + file_name + "'!");
}



//------------------------------------------------------------------------------
void Shader_Program::compile_shader(GLenum shader_type, const std::string& shader_source){
  const char* source = shader_source.c_str();
  
  // compile shader
  GLuint new_shader = glCreateShader(shader_type);
  glShaderSource(new_shader, 1, &source, NULL);
  glCompileShader(new_shader);
  
  // check for errors
  glGetShaderiv(new_shader, GL_COMPILE_STATUS, &gl_success);
  if( ! gl_success){
    glGetShaderInfoLog(new_shader, 512, NULL, info_log);
    std::cerr << "OpenGL returned:\n" << info_log << "\n";
    throw std::runtime_error("OpenGL: Failed to compile shader!");
  }
  
  // store shader id
  shaders.push_back(new_shader);
}