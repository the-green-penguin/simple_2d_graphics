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
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void APIENTRY glDebugOutput(GLenum source, GLenum type, uint id, GLenum severity, GLsizei length, const char *message, const void *userParam);



////////////////////////////////////////////////////////////////////////////////
// Window public
////////////////////////////////////////////////////////////////////////////////

id Window::open(){
  id new_win_id = Manager::get_instance().add_win();
  return new_win_id;
}



//------------------------------------------------------------------------------
void Window::close(id win_id){
  Manager::get_instance().close_win(win_id);
}



//------------------------------------------------------------------------------
bool Window::got_closed(id win_id){
  return Manager::get_instance().win_got_closed(win_id);
}



//------------------------------------------------------------------------------
std::size_t Window::count(){
  return Manager::get_instance().get_count();
}



////////////////////////////////////////////////////////////////////////////////
// Window private
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Helper public
////////////////////////////////////////////////////////////////////////////////

Window::Helper::Helper(Window::Wrapper* parent){
  std::cout << "Init Helper\n";
  this->parent = parent;
  
  create_glfw_window();
  enable_gl_debugging();
  setup_shader_program();
}



//------------------------------------------------------------------------------
Window::Helper::~Helper(){
  std::cout << "End Helper\n";
  glfwDestroyWindow(window);
}



//------------------------------------------------------------------------------
void Window::Helper::update(){
  if( glfwWindowShouldClose(window) )
    should_close.store(true);
    
  else
    exe_update();
}



////////////////////////////////////////////////////////////////////////////////
// Helper private
////////////////////////////////////////////////////////////////////////////////

void Window::Helper::create_glfw_window(){
  // create window
  std::lock_guard<std::mutex> lg(this->window_name.lock);   // lock string
  window = glfwCreateWindow(640, 480, window_name.data.c_str(), NULL, NULL);
  if( ! window)
    throw std::runtime_error("GLFW window creation failed!");
    
  // set callbacks / user_pointers
  change_context(window);
  
  glfwSetWindowUserPointer(window, parent);
  glfwSetErrorCallback(glfw_error);
  glfwSetScrollCallback(window, scroll_callback);
}



//------------------------------------------------------------------------------
void Window::Helper::change_context(GLFWwindow* window){
  glfwMakeContextCurrent(window);
  load_gl_functions();
}



//------------------------------------------------------------------------------
void Window::Helper::load_gl_functions(){
  GLenum err = glewInit();   // needs to be called after every context change!
  if(err != GLEW_OK){
    std::string message = "glewInit() returned: '";
    message += reinterpret_cast<const char*>( glewGetErrorString(err) );   // fix incompatible "string" types
    message += "'\n";
    throw std::runtime_error("GLEW initialization failed! " + message);
  }
}



//------------------------------------------------------------------------------
void Window::Helper::enable_gl_debugging(){
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if(flags & GL_CONTEXT_FLAG_DEBUG_BIT){   // check if debug flag was set -> configure debugging
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    ///glDebugMessageCallback(glDebugOutput, nullptr);   // function defined below
    ///glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
  }
}



//------------------------------------------------------------------------------
void Window::Helper::setup_shader_program(){
  std::vector<std::string> shader_sources = {"src/shaders/simple_2d.frag", "src/shaders/simple_2d.vert"};
  this->shader_program = std::make_shared<Shader_Program>(shader_sources);
}



//------------------------------------------------------------------------------
void Window::Helper::exe_update(){
  change_context(window);
  update_name();
  render();
}



//------------------------------------------------------------------------------
void Window::Helper::update_name(){
  std::lock_guard<std::mutex> lg(window_name.lock);
  glfwSetWindowTitle(window, window_name.data.c_str());
}



//------------------------------------------------------------------------------
void Window::Helper::render(){
  delete_old_gobjects();
  
  // adjust window size
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  // clear screen
  set_background();
  
  // render content
  shader_program->use();
  {
    std::lock_guard<std::mutex> lg(camera.lock);
    camera.data.update(shader_program, (float)width, (float)height);
  }
  render_gobjects();
  
  // show content
  glfwSwapBuffers(window);
}



//------------------------------------------------------------------------------
void Window::Helper::set_background(){
  std::lock_guard<std::mutex> lg(background_colour.lock);
  glClearColor(
    background_colour.data.x,
    background_colour.data.y,
    background_colour.data.z,
    1.0f
  );
  glClear(GL_COLOR_BUFFER_BIT);
}



//------------------------------------------------------------------------------
void Window::Helper::delete_old_gobjects(){
  std::lock_guard<std::mutex> lg_0(graphics_objects.lock);
  std::lock_guard<std::mutex> lg_1(ids_to_delete.lock);
  
  // delete all
  if( clear_gobjects.load() )
    graphics_objects.data.clear();
  
  // delete by id
  while( ! ids_to_delete.data.empty()){
    graphics_objects.data.erase( ids_to_delete.data.front() );
    ids_to_delete.data.pop();
  }
}



//------------------------------------------------------------------------------
void Window::Helper::render_gobjects(){
  std::lock_guard<std::mutex> lg(graphics_objects.lock);
    
  for(auto &obj : graphics_objects.data){
    // create vertex buffer if needed
    if(obj.second->buffers_ready == false)
      obj.second->setup_buffers();
      
    // actual rendering
    obj.second->render(shader_program);
  }
}



////////////////////////////////////////////////////////////////////////////////
// Wrapper public
////////////////////////////////////////////////////////////////////////////////

Window::Wrapper::Wrapper(){
  std::cout << "Init Wrapper\n";
  helper = std::make_shared< Helper >(this);
}



//------------------------------------------------------------------------------
Window::Wrapper::~Wrapper(){
  std::cout << "End Wrapper\n";
}



//------------------------------------------------------------------------------
id Window::Wrapper::add_gobject(std::shared_ptr<GShape> gobject){
  std::lock_guard<std::mutex> lg(helper->graphics_objects.lock);   // lock map
  helper->graphics_objects.data.insert({next_gobj_id, gobject});
  return next_gobj_id++;
}



//------------------------------------------------------------------------------
void Window::Wrapper::remove_gobject(id id){
  std::lock_guard<std::mutex> lg(helper->ids_to_delete.lock);   // lock vector
  helper->ids_to_delete.data.push(id);
}



//------------------------------------------------------------------------------
void Window::Wrapper::clear_gobjects(){
  helper->clear_gobjects.store(true);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_camera_position(glm::vec3 pos){
  std::lock_guard<std::mutex> lg(helper->camera.lock);   // lock camera
  helper->camera.data.set_position(pos);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_camera_zoom(float zoom){
  std::lock_guard<std::mutex> lg(helper->camera.lock);   // lock camera
  helper->camera.data.set_zoom(zoom);
}



//------------------------------------------------------------------------------
void Window::Wrapper::mod_camera_zoom(float zoom_diff){
  std::lock_guard<std::mutex> lg(helper->camera.lock);   // lock camera
  helper->camera.data.mod_zoom(zoom_diff);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_gobj_position(id id, glm::vec3 pos){
  std::lock_guard<std::mutex> lg(helper->graphics_objects.lock);   // lock map
  helper->graphics_objects.data.at(id)->set_position(pos);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_gobj_rotation(id id, float rot){
  std::lock_guard<std::mutex> lg(helper->graphics_objects.lock);   // lock map
  helper->graphics_objects.data.at(id)->set_rotation(rot);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_allow_zoom(bool b){
  helper->allow_zoom.store(b);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_allow_camera_movement(bool b){
  helper->allow_camera_movement.store(b);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_background_colour(glm::vec3 colour){
  std::lock_guard<std::mutex> lg(helper->background_colour.lock);   // lock vec3
  helper->background_colour.data = colour;
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_window_name(const std::string& name){
  std::lock_guard<std::mutex> lg(helper->window_name.lock);   // lock string
  helper->window_name.data = name;
}



////////////////////////////////////////////////////////////////////////////////
// Wrapper private
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Manager public
////////////////////////////////////////////////////////////////////////////////

Window::Manager& Window::Manager::get_instance(){
  static Manager instance;   // the single instance
  return instance;
}



//------------------------------------------------------------------------------
id Window::Manager::add_win(){
  windows.insert({
    next_win_id,
    std::make_shared< Wrapper >()
  });
  
  return next_win_id++;
}



//------------------------------------------------------------------------------
void Window::Manager::close_win(id id){
  windows.at(id)->helper->should_close.store(true);
}



//------------------------------------------------------------------------------
bool Window::Manager::win_got_closed(id id){
  return ( ! windows.contains(id));
}



//------------------------------------------------------------------------------
std::size_t Window::Manager::get_count(){
  return windows.size();
}



////////////////////////////////////////////////////////////////////////////////
// Manager private
////////////////////////////////////////////////////////////////////////////////

Window::Manager::Manager(){
  std::cout << "Init Manager\n";
  init_glfw();
  graphics_thread = std::thread(&Window::Manager::thread_func, this);
}



//------------------------------------------------------------------------------
Window::Manager::~Manager(){
  std::cout << "End Manager\n";
  stop_thread.store(true);
  graphics_thread.join();
  windows.clear();
  glfwTerminate();
}



//------------------------------------------------------------------------------
void Window::Manager::init_glfw(){
  if( ! glfwInit())
    throw std::runtime_error("GLFW initialization failed!");
  
  // use OpenGL version 3.3 core  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // enable GLFW debugging
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);   // remove in "release"!
}



//------------------------------------------------------------------------------
void Window::Manager::thread_func(){
  std::cout << "start thread\n";
  while( ! stop_thread.load() ){
    
    glfwPollEvents();
    for(auto &w : windows)
      w.second->helper->update();
    
    remove_closed_windows();
    
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms);
  }
}



//------------------------------------------------------------------------------
void Window::Manager::remove_closed_windows(){
  std::erase_if(
    windows,   // container
    [](const auto& item){
      auto const& [key, value] = item;
      return value->helper->should_close.load();   // condition for removal
    }
  );
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
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
  ///auto wrapper = reinterpret_cast< Window::Wrapper* >( glfwGetWindowUserPointer(window) );
  ///wrapper->mod_camera_zoom( - y_offset * 0.1f);
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