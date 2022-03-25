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

#include "window.h"

#include <iostream>
#include <string>
#include <exception>
#include <chrono>



void glfw_error(int error, const char* description);
///void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void APIENTRY glDebugOutput(GLenum source, GLenum type, uint id, GLenum severity, GLsizei length, const char *message, const void *userParam);



////////////////////////////////////////////////////////////////////////////////
// Window public
////////////////////////////////////////////////////////////////////////////////

Window::Window(const std::string& window_name){
  helper = std::make_shared< Window_Helper >(window_name);
  graphics_objects = helper->graphics_objects;
  setup_ready = helper->setup_ready;
  
  helper_thread = std::thread(&Window_Helper::run, helper);
}



//------------------------------------------------------------------------------
Window::~Window(){
  helper_thread.join();
}



//------------------------------------------------------------------------------
void Window::wait_for_setup(){
  using namespace std::chrono_literals;
  while( ! setup_ready->load())
    std::this_thread::sleep_for(100ns);
}



//------------------------------------------------------------------------------
id Window::add_gobject(std::shared_ptr<GShape> gobject){
  std::lock_guard<std::mutex> lg(helper->graphics_objects->lock);   // lock map
  helper->graphics_objects->data.insert({next_id, gobject});
  return next_id++;
}



//------------------------------------------------------------------------------
void Window::remove_gobject(id id){
  std::lock_guard<std::mutex> lg(helper->graphics_objects->lock);   // lock map
  helper->graphics_objects->data.erase(id);
}



//------------------------------------------------------------------------------
void Window::clear_gobjects(){
  std::lock_guard<std::mutex> lg(helper->graphics_objects->lock);   // lock map
  helper->graphics_objects->data.clear();
}



//------------------------------------------------------------------------------
void Window::set_camera_position(glm::vec3 pos){
  std::lock_guard<std::mutex> lg(helper->camera.lock);   // lock camera
  helper->camera.data.set_position(pos);
}



//------------------------------------------------------------------------------
void Window::set_camera_zoom(float zoom){
  std::lock_guard<std::mutex> lg(helper->camera.lock);   // lock camera
  helper->camera.data.set_zoom(zoom);
}



//------------------------------------------------------------------------------
void Window::set_gobj_position(id id, glm::vec3 pos){
  std::lock_guard<std::mutex> lg(helper->graphics_objects->lock);   // lock map
  helper->graphics_objects->data.at(id)->set_position(pos);
}



//------------------------------------------------------------------------------
void Window::set_gobj_rotation(id id, float rot){
  std::lock_guard<std::mutex> lg(helper->graphics_objects->lock);   // lock map
  helper->graphics_objects->data.at(id)->set_rotation(rot);
}



////////////////////////////////////////////////////////////////////////////////
// Window private
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Helper public
////////////////////////////////////////////////////////////////////////////////

Window::Window_Helper::Window_Helper(const std::string& window_name){
  this->window_name = window_name;
  this->graphics_objects = std::make_shared< sync_gobjects >();
}



//------------------------------------------------------------------------------
Window::Window_Helper::~Window_Helper(){}



//------------------------------------------------------------------------------
void Window::Window_Helper::run(){   // run as separate thread
  init();
  
  while(!glfwWindowShouldClose(window)){  //"...ShouldClose()" requires "...PollEvents()"!
    render();   
    glfwPollEvents();
  }
  
  stop();
}



////////////////////////////////////////////////////////////////////////////////
// Helper private
////////////////////////////////////////////////////////////////////////////////

void Window::Window_Helper::init(){
  try{
    setup_glfw();
    setup_glew();
    setup_glfw_debugging();
    setup_shader_program();
  }
  catch(std::exception &e){
    std::string error_message = "Error: Unable to open new window!\n\nCaused by exception:\n";
    throw std::runtime_error(error_message + e.what());
  }
  
  setup_ready->store(true);
}



//------------------------------------------------------------------------------
void Window::Window_Helper::setup_glfw(){
  if(!glfwInit())
    throw std::runtime_error("GLFW initialization failed!");
  
  // use OpenGL version 3.3 core  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // enable GLFW debugging
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);   // remove in "release"!
  
  window = glfwCreateWindow(640, 480, window_name.c_str(), NULL, NULL);
  if(!window)
    throw std::runtime_error("GLFW window creation failed!");
    
  glfwMakeContextCurrent(window);
  glfwSetErrorCallback(glfw_error);
}



//------------------------------------------------------------------------------
void Window::Window_Helper::setup_glew(){
  GLenum err = glewInit();   // needs to be called after context is created!
  if (err != GLEW_OK){
    std::cerr << "glewInit() returned: '" << glewGetErrorString(err) << "'\n";
    throw std::runtime_error("GLEW initialization failed!");
  }
}



//------------------------------------------------------------------------------
void Window::Window_Helper::setup_glfw_debugging(){
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if(flags & GL_CONTEXT_FLAG_DEBUG_BIT){   // check if debug flag was set -> configure debugging
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);   // function defined below
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  }
}



//------------------------------------------------------------------------------
void Window::Window_Helper::setup_shader_program(){
  std::vector<std::string> shader_sources = {"src/shaders/simple_2d.frag", "src/shaders/simple_2d.vert"};
  this->shader_program = std::make_shared<Shader_Program>(shader_sources);
  shader_program->use();
}



//------------------------------------------------------------------------------
void Window::Window_Helper::stop(){
  glfwDestroyWindow(window);
  glfwTerminate();
}



//------------------------------------------------------------------------------
void Window::Window_Helper::render(){
  // window size
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  // clear screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  // rendering content
  {
    std::lock_guard<std::mutex> lg(camera.lock);   // lock camera
    camera.data.update(shader_program, (float)width, (float)height);
  }
  render_gobjects();
  
  // show content
  glfwSwapBuffers(window);
}



//------------------------------------------------------------------------------
void Window::Window_Helper::render_gobjects(){
  std::lock_guard<std::mutex> lg(graphics_objects->lock);   // lock map
  
  for(auto &obj : graphics_objects->data){
    // create vertex buffer if needed
    if(obj.second->buffers_ready == false)
      obj.second->setup_buffers();
      
    // actual rendering
    obj.second->render(shader_program);
  }
}



////////////////////////////////////////////////////////////////////////////////
// non-member functions
////////////////////////////////////////////////////////////////////////////////

void glfw_error(int error, const char* description){
  std::string message = "GLFW error: [";
  message += std::to_string(error) + "] ";
  message += description;
  throw std::runtime_error(message);
}



//------------------------------------------------------------------------------
// Function "glDebugOutput" taken from "https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/7.in_practice/1.debugging/debugging.cpp"   [slightly modified]   [CC BY-NC 4.0 license] 
void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam){

  if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; // ignore these non-significant error codes

  std::cout << "---------------" << "\n";
  std::cout << "Debug message (" << id << "): " <<  message << "\n";

  switch(source){
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
  } std::cout << "\n";
  
  switch(type){
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
  } std::cout << "\n";
      
  switch(severity){
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
  } std::cout << "\n";
  
  std::cout << "\n";
}