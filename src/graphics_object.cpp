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

GObject::GObject(glm::vec3 position){
  this->position = position;
}



//------------------------------------------------------------------------------
GObject::~GObject(){}



//------------------------------------------------------------------------------
void GObject::set_position(glm::vec3 pos){  this-> position = pos;  }



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
  : GObject(position){
    
    this->rotation = rotation;
    this->vertices = vertices;
    this->indices = indices;
    this->index_count = indices.size();
    std::cout << indices.size() << " - " << this->indices.size() << "\n";
}



//------------------------------------------------------------------------------
GShape::~GShape(){
  glDeleteVertexArrays(1, &vertex_array_object);
  glDeleteBuffers(1, &vertex_buffer);
  glDeleteBuffers(1, &element_buffer);
}



//------------------------------------------------------------------------------
void GShape::setup_buffers(){
  // create vertex buffer & array object
  glGenBuffers(1, &vertex_buffer);   // '1' = number of buffers to be created
  glGenBuffers(1, &element_buffer);   // '1' = number of buffers to be created
  glGenVertexArrays(1, &vertex_array_object);
  
  glBindVertexArray(vertex_array_object);   // following modifications of the buffer are 'recorded' by the array object to be repeated in render loop (?)
  
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);   // modfying 'GL_ARRAY_BUFFER' now affects 'vertex_buffer' until new/no buffer object is bound
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);   // transfer data to GPU & specify how often data will change / will be used
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Index3), vertices.data(), GL_STATIC_DRAW);
  std::cout << indices.size() * sizeof(Index3) << "\n";
  // specify how data is arranged in buffer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);   // '0' = index of vertex attribute; '3' = components per vertex attribute; ... ; 'sizeof(Vertex)' = size of array element; '(void*)0' = initial offset (none)
  glEnableVertexAttribArray(0);   // enables vertex attribte number '0'
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));   // '1' = index of vertex attribute; '3' = components per vertex attribute; ... ; 'sizeof(Vertex)' = size of array element; '(void*)sizeof(glm::vec3)' = initial offset (skip x,y,z)
  glEnableVertexAttribArray(1);
  
  // cleanup
  vertices.clear();
  buffers_ready = true;
}



//------------------------------------------------------------------------------
void GShape::render(std::shared_ptr<Shader_Program> shader_program){
  model_transformation(shader_program);
  
  glBindVertexArray(vertex_array_object);
  ///glDrawArrays(GL_TRIANGLES, 0, index_count);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);   // you don't have to pass 'indices' as it's part of VAO
}



//------------------------------------------------------------------------------
void GShape::set_rotation(float rot){  this-> rotation = rot;  }



////////////////////////////////////////////////////////////////////////////////
// GShape private
////////////////////////////////////////////////////////////////////////////////

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
  : GShape(position, rotation, vertices, {{0, 1, 2}}){
    
    if(vertices.size() != 3)
      throw std::runtime_error("Could not create GTriangle! (Invalid vertex count)");
}



//------------------------------------------------------------------------------
GTriangle::GTriangle(glm::vec3 position, const std::vector<Vertex>& vertices)
  : GTriangle(position, 0.0f, vertices){}   // set rotation to default



//------------------------------------------------------------------------------
GTriangle::GTriangle(glm::vec3 position, float rotation, float size, glm::vec3 colour)
  : GShape(position, rotation, {}, {{0, 1, 2}}){
    
    // create equilateral triangle
    float height = size * sqrt(3) / 2;
    float third = 1.0f / 3.0f;
    
    vertices = {
      {{ size / 2   , - third * height   , 0.0f}, colour},
      {{ - size / 2 , - third * height   , 0.0f}, colour},
      {{ 0.0f       , 2 * third * height , 0.0f}, colour},
    };
    
    index_count = 3;
}



//------------------------------------------------------------------------------
GTriangle::GTriangle(glm::vec3 position, float size, glm::vec3 colour)
  : GTriangle(position, 0.0f, size, colour){}   // set rotation to default



//------------------------------------------------------------------------------
GTriangle::~GTriangle(){}



////////////////////////////////////////////////////////////////////////////////
// GTriangle private
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
// GRect public
////////////////////////////////////////////////////////////////////////////////

GRect::GRect(glm::vec3 position, float rotation, const std::vector<Vertex>& vertices)
  : GShape(position, rotation, vertices, {{0, 1, 3}, {1, 2, 3}}){
    
    if(vertices.size() != 4)
      throw std::runtime_error("Could not create GRect! (Invalid vertex count)");
}



//------------------------------------------------------------------------------
GRect::GRect(glm::vec3 position, const std::vector<Vertex>& vertices)
  : GRect(position, 0.0f, vertices){}   // set rotation to default



//------------------------------------------------------------------------------
GRect::GRect(glm::vec3 position, float rotation, float size, glm::vec3 colour)
  : GShape(position, rotation, {}, {{0, 1, 3}, {1, 2, 3}}){
    
    float half = size / 2;
    
    vertices = {
      {{ - half , - half, 0.0f}, colour},
      {{ - half , half  , 0.0f}, colour},
      {{ half   , - half, 0.0f}, colour},
      {{ half   , half  , 0.0f}, colour}
    };
    
    index_count = 6;
}



//------------------------------------------------------------------------------
GRect::GRect(glm::vec3 position, float size, glm::vec3 colour)
  : GRect(position, 0.0f, size, colour){}   // set rotation to default



//------------------------------------------------------------------------------
GRect::~GRect(){}



////////////////////////////////////////////////////////////////////////////////
// GRect private
////////////////////////////////////////////////////////////////////////////////

