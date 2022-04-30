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

#include "graphics_object.h"

#include <iostream>
#include <exception>
#include <math.h>



////////////////////////////////////////////////////////////////////////////////
// GObject public
////////////////////////////////////////////////////////////////////////////////

GObject::GObject(glm::vec3 position, float rotation){
  this->position = position;
  this->rotation = fmod(rotation, 360.0f);
}



//------------------------------------------------------------------------------
GObject::~GObject(){}



//------------------------------------------------------------------------------
void GObject::set_position(glm::vec3 pos){  this->position = pos;  }



//------------------------------------------------------------------------------
void GObject::set_rotation(float rot){  this->rotation = fmod(rot, 360.0f);  }



////////////////////////////////////////////////////////////////////////////////
// GObject private
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// GShape public
////////////////////////////////////////////////////////////////////////////////

GShape::GShape(
  glm::vec3 position,
  float rotation,
  const std::vector<Vertex>& vertices,
  const std::vector<Index3>& indices)
  : GObject(position, rotation){
    
    this->vertices = vertices;
    this->indices = indices;
}



//------------------------------------------------------------------------------
GShape::~GShape(){
  glDeleteVertexArrays(1, &vertex_array_object);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &element_buffer);
}



//------------------------------------------------------------------------------
void GShape::render(std::shared_ptr<Shader_Program> shader_program){
  if( ! buffers_ready)
    setup_buffers();
  
  model_transformation(shader_program);
  
  glBindVertexArray(vertex_array_object);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}



////////////////////////////////////////////////////////////////////////////////
// GShape private
////////////////////////////////////////////////////////////////////////////////

void GShape::setup_buffers(){
  int buffer_size;
  
  // create vertex/index buffer & array object
  glGenVertexArrays(1, &vertex_array_object);
  glBindVertexArray(vertex_array_object);   // following modifications of the buffer are 'recorded' by the array object to be repeated in render loop (?)
  
  // vertex buffer
  buffer_size = vertices.size() * sizeof(Vertex);
  glGenBuffers(1, &vertex_buffer);   // '1' = number of buffers to be created
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);   // modfying 'GL_ARRAY_BUFFER' now affects 'vertex_buffer' until new/no buffer object is bound
  glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices.data(), GL_STATIC_DRAW);   // transfer data to GPU & specify how often data will change / will be used
  
  // specify how data is arranged in vertex buffer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);   // '0' = index of vertex attribute; '3' = components per vertex attribute; ... ; 'sizeof(Vertex)' = size of array element; '(void*)0' = initial offset (none)
  glEnableVertexAttribArray(0);   // enables vertex attribte number '0'
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));   // '1' = index of vertex attribute; '3' = components per vertex attribute; ... ; 'sizeof(Vertex)' = size of array element; '(void*)sizeof(glm::vec3)' = initial offset (skip x,y,z)
  glEnableVertexAttribArray(1);
  
  // index buffer
  buffer_size = indices.size() * sizeof(Index3);
  glGenBuffers(1, &element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer_size, indices.data(), GL_STATIC_DRAW);
  
  index_count = indices.size() * 3;
  
  // cleanup
  vertices.clear();
  indices.clear();
  buffers_ready = true;
}



//------------------------------------------------------------------------------
void GShape::model_transformation(std::shared_ptr<Shader_Program> shader_program){
  glm::mat4 mtrans = glm::mat4(1.0f);
  
  mtrans = glm::translate(mtrans, position);
  mtrans = glm::rotate(mtrans, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));   // z-axis
  
  shader_program->set_uni("model", mtrans);
}



////////////////////////////////////////////////////////////////////////////////
// GTriangle public
////////////////////////////////////////////////////////////////////////////////

GTriangle::GTriangle(glm::vec3 position, float rotation, const std::vector<Vertex>& vertices)
  : GShape(position, rotation, vertices, {{}}){
    
    if(vertices.size() != 3)
      throw std::runtime_error("Could not create GTriangle! (Invalid vertex count)");

    indices = tri_index;
}



//------------------------------------------------------------------------------
GTriangle::GTriangle(glm::vec3 position, const std::vector<Vertex>& vertices)
  : GTriangle(position, 0.0f, vertices){}   // set rotation to default



//------------------------------------------------------------------------------
GTriangle::GTriangle(glm::vec3 position, float rotation, float size, glm::vec3 colour)
  : GShape(position, rotation, {}, {{}}){
    
    generate_triangle(size, colour);
}



//------------------------------------------------------------------------------
GTriangle::GTriangle(glm::vec3 position, float size, glm::vec3 colour)
  : GTriangle(position, 0.0f, size, colour){}   // set rotation to default



//------------------------------------------------------------------------------
GTriangle::GTriangle(float size, glm::vec3 colour)
  : GTriangle({0.0f, 0.0f, 0.0f}, 0.0f, size, colour){}   // set position & rotation to default



//------------------------------------------------------------------------------
GTriangle::~GTriangle(){}



////////////////////////////////////////////////////////////////////////////////
// GTriangle private
////////////////////////////////////////////////////////////////////////////////

void GTriangle::generate_triangle(float size, glm::vec3 colour){
  // create equilateral triangle
  float height = size * sqrt(3) / 2;
  float third = 1.0f / 3.0f;
  
  vertices = {
    {{ size / 2   , - third * height   , 0.0f}, colour},
    {{ - size / 2 , - third * height   , 0.0f}, colour},
    {{ 0.0f       , 2 * third * height , 0.0f}, colour},
  };
  
  indices = tri_index;
}



////////////////////////////////////////////////////////////////////////////////
// GRect public
////////////////////////////////////////////////////////////////////////////////

GRect::GRect(glm::vec3 position, float rotation, const std::vector<Vertex>& vertices)
  : GShape(position, rotation, vertices, {{}}){
    
    if(vertices.size() != 4)
      throw std::runtime_error("Could not create GRect! (Invalid vertex count)");

    indices = rect_index;
}



//------------------------------------------------------------------------------
GRect::GRect(glm::vec3 position, const std::vector<Vertex>& vertices)
  : GRect(position, 0.0f, vertices){}   // set rotation to default



//------------------------------------------------------------------------------
GRect::GRect(glm::vec3 position, float rotation, float size, glm::vec3 colour)
  : GShape(position, rotation, {}, {{}}){
    
    generate_rect(size, colour);
}



//------------------------------------------------------------------------------
GRect::GRect(glm::vec3 position, float size, glm::vec3 colour)
  : GRect(position, 0.0f, size, colour){}   // set rotation to default



//------------------------------------------------------------------------------
GRect::GRect(float size, glm::vec3 colour)
  : GRect({0.0f, 0.0f, 0.0f}, 0.0f, size, colour){}   // set position & rotation to default



//------------------------------------------------------------------------------
GRect::~GRect(){}



////////////////////////////////////////////////////////////////////////////////
// GRect private
////////////////////////////////////////////////////////////////////////////////

void GRect::generate_rect(float size, glm::vec3 colour){
  float half = size / 2;
  
  vertices = {
    {{ - half , - half, 0.0f}, colour},
    {{ - half , half  , 0.0f}, colour},
    {{ half   , - half, 0.0f}, colour},
    {{ half   , half  , 0.0f}, colour}
  };
  
  indices = rect_index;
}



////////////////////////////////////////////////////////////////////////////////
// GCircle public
////////////////////////////////////////////////////////////////////////////////

GCircle::GCircle(glm::vec3 position, float rotation, const std::vector<Vertex>& vertices, const std::vector<Index3>& indices)
  : GShape(position, rotation, vertices, indices){
    
    if(vertices.size() != 4)
      throw std::runtime_error("Could not create GCircle! (Invalid vertex count)");
}



//------------------------------------------------------------------------------
GCircle::GCircle(glm::vec3 position, const std::vector<Vertex>& vertices, const std::vector<Index3>& indices)
  : GCircle(position, 0.0f, vertices, indices){}   // set rotation to default



//------------------------------------------------------------------------------
GCircle::GCircle(glm::vec3 position, float rotation, float size, glm::vec3 colour)
  : GShape(position, rotation, {}, {{}}){
    
    generate_indices();
    generate_vertices(size, colour);
}



//------------------------------------------------------------------------------
GCircle::GCircle(glm::vec3 position, float size, glm::vec3 colour)
  : GCircle(position, 0.0f, size, colour){}   // set rotation to default



//------------------------------------------------------------------------------
GCircle::GCircle(float size, glm::vec3 colour)
  : GCircle({0.0f, 0.0f, 0.0f}, 0.0f, size, colour){}   // set position & rotation to default



//------------------------------------------------------------------------------
GCircle::~GCircle(){}



////////////////////////////////////////////////////////////////////////////////
// GCircle private
////////////////////////////////////////////////////////////////////////////////

void GCircle::generate_indices(){
  indices.clear();
    
  for(uint i = vertex_count; i > 0; i--){
    uint next = i % vertex_count + 1;
    indices.push_back( {0, i, next} );
  }
}



//------------------------------------------------------------------------------
void GCircle::generate_vertices(float size, glm::vec3 colour){
  float half = size / 2;
  vertices.push_back( {{ 0.0f, 0.0f, 0.0f}, colour} );   // center
  
  for(uint i = 0; i < vertex_count; i++){
    float segment = 360.0f * i / vertex_count;
    float y = half * sin(segment * M_PI / 180);
    float x = half * cos(segment * M_PI / 180);
    vertices.push_back( {{x, y, 0.0f}, colour} );
  }
}