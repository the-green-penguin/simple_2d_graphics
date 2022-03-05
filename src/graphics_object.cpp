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



////////////////////////////////////////////////////////////////////////////////
// GObject public
////////////////////////////////////////////////////////////////////////////////

GObject::GObject(glm::vec3 position){
  this->position = position;
}



//------------------------------------------------------------------------------
GObject::~GObject(){}



////////////////////////////////////////////////////////////////////////////////
// GObject private
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// GShape public
////////////////////////////////////////////////////////////////////////////////

GShape::GShape(glm::vec3 position, const std::vector<Vertex>& vertices)
  : GObject(position){   // pass argument to base constructor
    
    index_count = vertices.size();
    setup_vertex_buffer(vertices);
}



//------------------------------------------------------------------------------
GShape::~GShape(){}



////////////////////////////////////////////////////////////////////////////////
// GShape private
////////////////////////////////////////////////////////////////////////////////

void GShape::setup_vertex_buffer(const std::vector<Vertex>& vertices){
  // create vertex buffer & array object
  glGenBuffers(1, &vertex_buffer);   // '1' = number of buffers to be created
  glGenVertexArrays(1, &vertex_array_object);
  
  glBindVertexArray(vertex_array_object);   // following modifications of the buffer are 'recorded' by the array object to be repeated in render loop (?)
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);   // modfying 'GL_ARRAY_BUFFER' now affects 'vertex_buffer' until new/no buffer object is bound
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);   // transfer data to GPU & specify how often data will change / will be used
  
  // specify how data is arranged in buffer
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);   // '0' = index of vertex attribute; '3' = components per vertex attribute; ... ; '6 * sizeof()' = size of array element; '(void*)0' = initial offset (none)
  glEnableVertexAttribArray(0);   // enables vertex attribte number '0'
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));   // '1' = index of vertex attribute; '3' = components per vertex attribute; ... ; '6 * sizeof()' = size of array element; '(void*)(3* sizeof(float)' = initial offset (skip x,y,z)
  glEnableVertexAttribArray(1);
}



//------------------------------------------------------------------------------
void GShape::render(){
  glBindVertexArray(vertex_array_object);
  glDrawArrays(GL_TRIANGLES, 0, index_count);
}



////////////////////////////////////////////////////////////////////////////////
// GTriangle public
////////////////////////////////////////////////////////////////////////////////

GTriangle::GTriangle(glm::vec3 position, const std::vector<Vertex>& vertices)
  : GShape(position, vertices){   // pass argument to base constructor
    
}



//------------------------------------------------------------------------------
GTriangle::~GTriangle(){}



////////////////////////////////////////////////////////////////////////////////
// GTriangle private
////////////////////////////////////////////////////////////////////////////////

