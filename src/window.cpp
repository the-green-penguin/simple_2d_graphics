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
#include <exception>



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
id Window::open(const std::string& name){
  id new_win_id = Manager::get_instance().add_win();
  set_window_name(new_win_id, name);
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



//------------------------------------------------------------------------------
id Window::add_gobject(id win_id, std::shared_ptr<GShape> gobject){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->graphics_objects.lock);
  win->graphics_objects.data.insert({
    win->next_gobj_id.load(),
    gobject
  });
  return win->next_gobj_id++;
}



//------------------------------------------------------------------------------
void Window::remove_gobject(id win_id, id id){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->ids_to_delete.lock);
  win->ids_to_delete.data.push(id);
}



//------------------------------------------------------------------------------
void Window::clear_gobjects(id win_id){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  win->clear_gobjects.store(true);
}



//------------------------------------------------------------------------------
void Window::set_camera_position(id win_id, glm::vec3 pos){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->camera.lock);
  win->camera.data.set_position(pos);
}



//------------------------------------------------------------------------------
void Window::set_camera_zoom(id win_id, float zoom){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->camera.lock);
  win->camera.data.set_zoom(zoom);
}



//------------------------------------------------------------------------------
void Window::mod_camera_zoom(id win_id, float zoom_diff){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->camera.lock);
  win->camera.data.mod_zoom(zoom_diff);
}



//------------------------------------------------------------------------------
void Window::set_gobj_position(id win_id, id gobj_id, glm::vec3 pos){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->graphics_objects.lock);
  win->graphics_objects.data.at(gobj_id)->set_position(pos);
}



//------------------------------------------------------------------------------
void Window::set_gobj_rotation(id win_id, id gobj_id, float rot){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->graphics_objects.lock);
  win->graphics_objects.data.at(gobj_id)->set_rotation(rot);
}



//------------------------------------------------------------------------------
void Window::set_allow_zoom(id win_id, bool b){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  win->allow_zoom.store(b);
}



//------------------------------------------------------------------------------
void Window::set_allow_camera_movement(id win_id, bool b){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  win->allow_camera_movement.store(b);
}



//------------------------------------------------------------------------------
void Window::set_background_colour(id win_id, glm::vec3 colour){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->background_colour.lock);
  win->background_colour.data = colour;
}



//------------------------------------------------------------------------------
void Window::set_window_name(id win_id, const std::string& name){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  auto win = Manager::get_instance().windows.data.at(win_id);
  
  std::lock_guard lg_1(win->window_name.lock);
  win->window_name.data = name;
  win->window_name.has_update = true;
}



////////////////////////////////////////////////////////////////////////////////
// Window private
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Wrapper public
////////////////////////////////////////////////////////////////////////////////

Window::Wrapper::Wrapper(id w_id){
  this->w_id = w_id;
  
  create_glfw_window();
  enable_gl_debugging();
  setup_shader_program();
}



//------------------------------------------------------------------------------
Window::Wrapper::~Wrapper(){
  glfwDestroyWindow(window);
}



//------------------------------------------------------------------------------
void Window::Wrapper::update(){
  if( glfwWindowShouldClose(window) )
    should_close.store(true);
    
  else
    exe_update();
}



////////////////////////////////////////////////////////////////////////////////
// Wrapper private
////////////////////////////////////////////////////////////////////////////////

void Window::Wrapper::create_glfw_window(){
  
  std::lock_guard lg(this->window_name.lock);   // lock string
  window = glfwCreateWindow(640, 480, window_name.data.c_str(), NULL, NULL);
  if( ! window)
    throw std::runtime_error("GLFW window creation failed!");
    
  glfwMakeContextCurrent(window);
  load_gl_functions();
  
  glfwSetWindowUserPointer(window, (void*)w_id);   // try storing window_id as "pointer" (hacky!!!)
  glfwSetErrorCallback(glfw_error);
  glfwSetScrollCallback(window, scroll_callback);
}



//------------------------------------------------------------------------------
void Window::Wrapper::load_gl_functions(){
  GLenum err = glewInit();   // needs to be called after every context creation!
  if(err != GLEW_OK){
    std::string message = "glewInit() returned: '";
    message += reinterpret_cast<const char*>( glewGetErrorString(err) );   // fix incompatible "string" types
    message += "'\n";
    throw std::runtime_error("GLEW initialization failed! " + message);
  }
}



//------------------------------------------------------------------------------
void Window::Wrapper::enable_gl_debugging(){
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
void Window::Wrapper::setup_shader_program(){
  std::vector<std::string> shader_sources = {"src/shaders/simple_2d.frag", "src/shaders/simple_2d.vert"};
  this->shader_program = std::make_shared<Shader_Program>(shader_sources);
}



//------------------------------------------------------------------------------
void Window::Wrapper::exe_update(){
  glfwMakeContextCurrent(window);
  update_name();
  render();
}



//------------------------------------------------------------------------------
void Window::Wrapper::update_name(){
  std::lock_guard lg(window_name.lock);
  if(window_name.has_update){
    glfwSetWindowTitle(window, window_name.data.c_str());
    window_name.has_update = false;
  }
}



//------------------------------------------------------------------------------
void Window::Wrapper::render(){
  delete_old_gobjects();
  
  // adjust window size
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  // clear screen
  set_background();
  
  // render content
  shader_program->use();
  {
    std::lock_guard lg(camera.lock);
    camera.data.update(shader_program, (float)width, (float)height);
  }
  render_gobjects();
  
  // show content
  glfwSwapBuffers(window);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_background(){
  std::lock_guard lg(background_colour.lock);
  glClearColor(
    background_colour.data.x,
    background_colour.data.y,
    background_colour.data.z,
    1.0f
  );
  glClear(GL_COLOR_BUFFER_BIT);
}



//------------------------------------------------------------------------------
void Window::Wrapper::delete_old_gobjects(){
  std::lock_guard lg_0(graphics_objects.lock);
  std::lock_guard lg_1(ids_to_delete.lock);
  
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
void Window::Wrapper::render_gobjects(){
  std::lock_guard lg(graphics_objects.lock);
    
  for(auto &obj : graphics_objects.data){
    // create vertex buffer if needed
    if(obj.second->buffers_ready == false)
      obj.second->setup_buffers();
      
    // actual rendering
    obj.second->render(shader_program);
  }
}



////////////////////////////////////////////////////////////////////////////////
// Manager public
////////////////////////////////////////////////////////////////////////////////

Window::Manager& Window::Manager::get_instance(){
  static Manager instance;   // the single instance
  return instance;
}



//------------------------------------------------------------------------------
id Window::Manager::add_win(){
  std::lock_guard lg(windows.lock);
  windows.data.insert({
    next_win_id,
    std::make_shared< Wrapper >(next_win_id)
  });
  
  return next_win_id++;
}



//------------------------------------------------------------------------------
void Window::Manager::close_win(id id){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  windows.data.at(id)->should_close.store(true);
}



//------------------------------------------------------------------------------
bool Window::Manager::win_got_closed(id id){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  return ( ! windows.data.contains(id));
}



//------------------------------------------------------------------------------
std::size_t Window::Manager::get_count(){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  return windows.data.size();
}



////////////////////////////////////////////////////////////////////////////////
// Manager private
////////////////////////////////////////////////////////////////////////////////

Window::Manager::Manager(){
  init_glfw();
  graphics_thread = std::thread(&Window::Manager::thread_func, this);
}



//------------------------------------------------------------------------------
Window::Manager::~Manager(){
  
  stop_thread.store(true);
  graphics_thread.join();
  
  {
    std::lock_guard lg(windows.lock);
    windows.data.clear();
  }
  
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
  
  while( ! stop_thread.load() ){
    glfwPollEvents();
    update_windows();
    remove_closed_windows();
    wait_until_next_frame();
  }
}



//------------------------------------------------------------------------------
void Window::Manager::update_windows(){
  std::shared_lock sl(Manager::get_instance().windows.lock, std::defer_lock);
  for(auto &w : windows.data)
    w.second->update();
}



//------------------------------------------------------------------------------
void Window::Manager::remove_closed_windows(){
  std::lock_guard lg(windows.lock);
  std::erase_if(
    windows.data,   // container
    [](const auto& item){
      auto const& [key, value] = item;
      return value->should_close.load();   // condition for removal
    }
  );
}



//------------------------------------------------------------------------------
void Window::Manager::wait_until_next_frame(){
  using namespace std::chrono;
  using namespace std::chrono_literals;
  
  steady_clock::time_point now = steady_clock::now();
  microsecs time_elapsed = duration_cast<microseconds>(now - prev_time).count();
  prev_time = now;
  
  microsecs time_per_frame = 1000000 / fps;
  microsecs wait_time = 0;
  if(time_per_frame > time_elapsed)
    wait_time = time_per_frame - time_elapsed;
  
  std::this_thread::sleep_for( microseconds(wait_time) );
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
  auto win_id = (id) glfwGetWindowUserPointer(window);
  Window::mod_camera_zoom(win_id, - y_offset * 0.1f);
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