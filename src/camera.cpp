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

#include "camera.h"

#include <exception>

#include <glm/glm.hpp>   // sudo apt install libglm-dev
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



////////////////////////////////////////////////////////////////////////////////
// public
////////////////////////////////////////////////////////////////////////////////

Camera::Camera(){}



//------------------------------------------------------------------------------
Camera::Camera(glm::vec3 position){
  this->position = position;
}



//------------------------------------------------------------------------------
Camera::~Camera(){}



//------------------------------------------------------------------------------
void Camera::set_position(glm::vec3 pos){
  this->position = pos;
}



//------------------------------------------------------------------------------
void Camera::set_zoom(float zoom){
  this->zoom = zoom;
}



//------------------------------------------------------------------------------
void Camera::update(std::shared_ptr< Shader_Program > shader_program, float screen_width, float screen_height){
  glm::mat4 camera = glm::mat4(1.0f);
  
  // screen center
  glm::vec3 offset(screen_width / 2, screen_height / 2, 0.0f);
  camera = glm::translate(camera, offset);
  
  // camera position
  camera = glm::translate(camera, position);
  shader_program->set_uni("view", camera);
  
  // camera mode
  if( ! is_ortho)
    throw std::runtime_error("Window->Camera: Perspective projection not supported!");
    
  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::ortho(
    0.0f,
    screen_width * zoom,
    screen_height * zoom,
    0.0f,
    -1.0f, 1.0f
  );
  shader_program->set_uni("projection", projection);
}



////////////////////////////////////////////////////////////////////////////////
// private
////////////////////////////////////////////////////////////////////////////////
