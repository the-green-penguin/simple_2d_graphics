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
  return open("");
}



//------------------------------------------------------------------------------
id Window::open(const std::string& name){
  id win_id = Manager::get_next_win_id();
  
  Thread_Message msg = { Thread_Message::open_win, std::make_tuple(win_id, name) };
  Manager::push_msg_from_API(msg);
  
  return win_id;
}



//------------------------------------------------------------------------------
void Window::close(id win_id){
  Thread_Message msg = { Thread_Message::close_win, win_id };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
bool Window::got_closed(id win_id){
  Manager::process_msgs_to_API();   // update Manager
  return Manager::get_instance().win_got_closed(win_id);
}



//------------------------------------------------------------------------------
std::size_t Window::count(){
  Manager::process_msgs_to_API();   // update Manager
  return Manager::get_instance().get_count();
}



//------------------------------------------------------------------------------
id Window::add_gobject(id win_id, gobj_type g_type, float size, glm::vec3 colour){
  std::shared_ptr< GShape > gobject = Manager::new_gobject(g_type, size, colour);
  id gobj_id = Manager::get_next_gobj_id();
  
  Thread_Message msg = { Thread_Message::add_gobject, std::make_tuple(win_id, gobj_id, gobject) };
  Manager::push_msg_from_API(msg);
  
  return gobj_id;
}



//------------------------------------------------------------------------------
id Window::add_gobject(id win_id, gobj_type g_type, glm::vec3 position, float size, glm::vec3 colour){
  std::shared_ptr< GShape > gobject = Manager::new_gobject(g_type, position, size, colour);
  id gobj_id = Manager::get_next_gobj_id();
  
  Thread_Message msg = { Thread_Message::add_gobject, std::make_tuple(win_id, gobj_id, gobject) };
  Manager::push_msg_from_API(msg);
  
  return gobj_id;
}



//------------------------------------------------------------------------------
id Window::add_gobject(id win_id, gobj_type g_type, glm::vec3 position, float rotation, float size, glm::vec3 colour){
  std::shared_ptr< GShape > gobject = Manager::new_gobject(g_type, position, rotation, size, colour);
  id gobj_id = Manager::get_next_gobj_id();
  
  Thread_Message msg = { Thread_Message::add_gobject, std::make_tuple(win_id, gobj_id, gobject) };
  Manager::push_msg_from_API(msg);
  
  return gobj_id;
}



//------------------------------------------------------------------------------
void Window::remove_gobject(id win_id, id gobj_id){
  Thread_Message msg = { Thread_Message::remove_gobject, std::make_tuple(win_id, gobj_id) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::clear_gobjects(id win_id){
  Thread_Message msg = { Thread_Message::clear_gobjects, win_id };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_gobj_position(id win_id, id gobj_id, glm::vec3 position){
  Thread_Message msg = { Thread_Message::set_gobj_position, std::make_tuple(win_id, gobj_id, position) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_gobj_rotation(id win_id, id gobj_id, float rotation){
  Thread_Message msg = { Thread_Message::set_gobj_rotation, std::make_tuple(win_id, gobj_id, rotation) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_camera_position(id win_id, glm::vec3 pos){
  Thread_Message msg = { Thread_Message::set_camera_position, std::make_tuple(win_id, pos) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_camera_zoom(id win_id, float zoom){
  Thread_Message msg = { Thread_Message::set_camera_zoom, std::make_tuple(win_id, zoom) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::mod_camera_zoom(id win_id, float zoom_diff){
  Thread_Message msg = { Thread_Message::mod_camera_zoom, std::make_tuple(win_id, zoom_diff) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_allow_zoom(id win_id, bool b){
  Thread_Message msg = { Thread_Message::set_allow_zoom, std::make_tuple(win_id, b) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_allow_camera_movement(id win_id, bool b){
  Thread_Message msg = { Thread_Message::set_allow_camera_movement, std::make_tuple(win_id, b) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_background_colour(id win_id, glm::vec3 colour){
  Thread_Message msg = { Thread_Message::set_background_colour, std::make_tuple(win_id, colour) };
  Manager::push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::set_window_name(id win_id, const std::string& name){
  Thread_Message msg = { Thread_Message::set_window_name, std::make_tuple(win_id, name) };
  Manager::push_msg_from_API(msg);
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
  if( glfwWindowShouldClose(window) ){
    Thread_Message msg = { Thread_Message::close_win, w_id };
    Manager::push_msg_from_API(msg);
  }
    
  else
    exe_update();
}



//------------------------------------------------------------------------------
void Window::Wrapper::update_name(const std::string& name){
  window_name = name;
  glfwSetWindowTitle(window, name.c_str());
}



////////////////////////////////////////////////////////////////////////////////
// Wrapper private
////////////////////////////////////////////////////////////////////////////////

void Window::Wrapper::create_glfw_window(){
  window = glfwCreateWindow(640, 480, window_name.c_str(), NULL, NULL);
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
  ///std::vector<std::string> shader_sources = {"src/shaders/simple_2d.frag", "src/shaders/simple_2d.vert"};
  this->shader_program = std::make_shared<Shader_Program>();
}



//------------------------------------------------------------------------------
void Window::Wrapper::exe_update(){
  glfwMakeContextCurrent(window);
  render();
}



//------------------------------------------------------------------------------
void Window::Wrapper::render(){
  // adjust window size
  glfwGetFramebufferSize(window, &width, &height);
  glViewport(0, 0, width, height);
  
  // clear screen
  set_background();
  
  // render content
  shader_program->use();
  camera.update(shader_program, (float)width, (float)height);
  render_gobjects();
  
  // show content
  glfwSwapBuffers(window);
}



//------------------------------------------------------------------------------
void Window::Wrapper::set_background(){
  glClearColor(
    background_colour.x,
    background_colour.y,
    background_colour.z,
    1.0f
  );
  glClear(GL_COLOR_BUFFER_BIT);
}



//------------------------------------------------------------------------------
void Window::Wrapper::render_gobjects(){
  for(auto &obj : graphics_objects)
    obj.second->render(shader_program);
}



////////////////////////////////////////////////////////////////////////////////
// Manager public
////////////////////////////////////////////////////////////////////////////////

Window::Manager& Window::Manager::get_instance(){
  static Manager instance;   // the single instance
  return instance;
}



//------------------------------------------------------------------------------
bool Window::Manager::win_got_closed(id id){
  return got_closed.at(id);
}



//------------------------------------------------------------------------------
std::size_t Window::Manager::get_count(){
  return window_count;
}



//------------------------------------------------------------------------------
void Window::Manager::push_msg_from_API(const Thread_Message& msg){
  std::lock_guard lock(get_instance().messages_from_API.mutex);
  get_instance().messages_from_API.data.push( msg );
}



//------------------------------------------------------------------------------
void Window::Manager::process_msgs_to_API(){
  while(true){
    Thread_Message msg;
    
    {
      std::lock_guard lock(get_instance().messages_to_API.mutex);
      if(get_instance().messages_to_API.data.empty())
        break;
      
      else{
        msg = get_instance().messages_to_API.data.front();
        get_instance().messages_to_API.data.pop();
      }
    }
    
    get_instance().process_msg(msg);
  }
}



//------------------------------------------------------------------------------
id Window::Manager::get_next_win_id(){
  get_instance().next_win_id++;
  get_instance().got_closed.insert( {get_instance().next_win_id, false} );
  
  return get_instance().next_win_id;
}



//------------------------------------------------------------------------------
id Window::Manager::get_next_gobj_id(){
  return get_instance().next_gobj_id++;
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
  try{  thread_loop();  }
  
  catch(std::exception& e){
    std::string message = "Window::Manager: ";
    message += e.what();
    throw std::runtime_error( message );
  }
}



//------------------------------------------------------------------------------
void Window::Manager::thread_loop(){
  while( ! stop_thread.load() ){
    glfwPollEvents();
    process_msgs_from_API();
    update_windows();
    wait_until_next_frame();
  }
}



//------------------------------------------------------------------------------
void Window::Manager::update_windows(){
  for(auto &w : windows)
    w.second->update();
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



//------------------------------------------------------------------------------
void Window::Manager::push_msg_to_API(const Thread_Message& msg){
  std::lock_guard lock(messages_to_API.mutex);
  messages_to_API.data.push( msg );
}



//------------------------------------------------------------------------------
void Window::Manager::process_msgs_from_API(){
  while(true){
    Thread_Message msg;
    
    {
      std::lock_guard lock(messages_from_API.mutex);
      if(messages_from_API.data.empty())
        break;
      
      else{
        msg = messages_from_API.data.front();
        messages_from_API.data.pop();
      }
    }
    
    process_msg(msg);
  }
}



//------------------------------------------------------------------------------
void Window::Manager::process_msg(Thread_Message& msg){
  switch(msg.type){
    case Thread_Message::open_win:{
      auto param = std::get< std::tuple< id, std::string > >(msg.parameters);
      add_win(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::close_win:{
      auto param = std::get< id >(msg.parameters);
      close_win(param);
      break;
    }
    case Thread_Message::got_closed:{
      auto param = std::get< id >(msg.parameters);
      got_closed.at(param) = true;
      break;
    }
    case Thread_Message::count_win:{
      auto param = std::get< std::size_t >(msg.parameters);
      window_count = param;
      break;
    }
    case Thread_Message::add_gobject:{
      auto param = std::get< std::tuple< id, id, std::shared_ptr< GShape > > >(msg.parameters);
      add_new_gobject(std::get<0>(param), std::get<1>(param), std::get<2>(param));
      break;
    }
    case Thread_Message::remove_gobject:{
      auto param = std::get< std::tuple< id, id > >(msg.parameters);
      remove_gobject(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::clear_gobjects:{
      auto param = std::get< id >(msg.parameters);
      clear_gobjects(param);
      break;
    }
    case Thread_Message::set_gobj_position:{
      auto param = std::get< std::tuple<id, id, glm::vec3 > >(msg.parameters);
      set_gobj_position(std::get<0>(param), std::get<1>(param), std::get<2>(param));
      break;
    }
    case Thread_Message::set_gobj_rotation:{
      auto param = std::get< std::tuple<id, id, float > >(msg.parameters);
      set_gobj_rotation(std::get<0>(param), std::get<1>(param), std::get<2>(param));
      break;
    }
    case Thread_Message::set_camera_position:{
      auto param = std::get< std::tuple<id, glm::vec3 > >(msg.parameters);
      set_camera_position(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::set_camera_zoom:{
      auto param = std::get< std::tuple<id, float > >(msg.parameters);
      set_camera_zoom(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::mod_camera_zoom:{
      auto param = std::get< std::tuple<id, float > >(msg.parameters);
      mod_camera_zoom(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::set_allow_zoom:{
      auto param = std::get< std::tuple<id, bool > >(msg.parameters);
      set_allow_zoom(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::set_allow_camera_movement:{
      auto param = std::get< std::tuple<id, bool > >(msg.parameters);
      set_allow_camera_movement(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::set_background_colour:{
      auto param = std::get< std::tuple<id, glm::vec3 > >(msg.parameters);
      set_background_colour(std::get<0>(param), std::get<1>(param));
      break;
    }
    case Thread_Message::set_window_name:{
      auto param = std::get< std::tuple<id, std::string > >(msg.parameters);
      set_window_name(std::get<0>(param), std::get<1>(param));
      break;
    }
  }
}



//------------------------------------------------------------------------------
void Window::Manager::add_win(id win_id, const std::string& name){
  windows.insert({
    win_id,
    std::make_shared< Wrapper >(win_id)
  });
  
  Thread_Message msg = {Thread_Message::count_win, windows.size()};
  push_msg_to_API(msg);
  msg = {Thread_Message::set_window_name, std::make_tuple(win_id, name) };
  push_msg_from_API(msg);
}



//------------------------------------------------------------------------------
void Window::Manager::close_win(id id){
  windows.erase(id);
  
  Thread_Message msg = {Thread_Message::count_win, windows.size()};
  push_msg_to_API(msg);
  msg = {Thread_Message::got_closed, id};
  push_msg_to_API(msg);
}



//------------------------------------------------------------------------------
void Window::Manager::add_new_gobject(id win_id, id gobj_id, std::shared_ptr< GShape > obj){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->graphics_objects.insert( {gobj_id, obj} );
}



//------------------------------------------------------------------------------
void Window::Manager::remove_gobject(id win_id, id gobj_id){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->graphics_objects.erase(gobj_id);
}



//------------------------------------------------------------------------------
void Window::Manager::clear_gobjects(id win_id){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->graphics_objects.clear();
}



//------------------------------------------------------------------------------
void Window::Manager::set_gobj_position(id win_id, id gobj_id, glm::vec3 position){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->graphics_objects.at(gobj_id)->set_position(position);
}



//------------------------------------------------------------------------------
void Window::Manager::set_gobj_rotation(id win_id, id gobj_id, float rotation){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->graphics_objects.at(gobj_id)->set_rotation(rotation);
}



//------------------------------------------------------------------------------
void Window::Manager::set_camera_position(id win_id, glm::vec3 pos){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->camera.set_position(pos);
}



//------------------------------------------------------------------------------
void Window::Manager::set_camera_zoom(id win_id, float zoom){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->camera.set_zoom(zoom);
}



//------------------------------------------------------------------------------
void Window::Manager::mod_camera_zoom(id win_id, float zoom_diff){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->camera.mod_zoom(zoom_diff);
}



//------------------------------------------------------------------------------
void Window::Manager::set_allow_zoom(id win_id, bool b){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->allow_zoom = b;
}



//------------------------------------------------------------------------------
void Window::Manager::set_allow_camera_movement(id win_id, bool b){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->allow_camera_movement = b;
}



//------------------------------------------------------------------------------
void Window::Manager::set_background_colour(id win_id, glm::vec3 colour){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->background_colour = colour;
}



//------------------------------------------------------------------------------
void Window::Manager::set_window_name(id win_id, const std::string& name){
  auto win = safe_get_window(win_id);
  if( win.has_value() )
    win.value()->update_name(name);
}



//------------------------------------------------------------------------------
std::optional< std::shared_ptr< Window::Wrapper > > Window::Manager::safe_get_window(id win_id){
  try{  return windows.at(win_id);  }
  catch(std::out_of_range& e){  return {};  }   // window not found -> return empty
}



//------------------------------------------------------------------------------
std::shared_ptr< GShape > Window::Manager::new_gobject(gobj_type g_type, float size, glm::vec3 colour){
  std::shared_ptr< GShape > obj;
  
  switch(g_type){
  case t_triangle:  obj = std::make_shared< GTriangle >(size, colour);   break;
  case t_rectangle: obj = std::make_shared< GRect >(size, colour);   break;
  case t_circle:    obj = std::make_shared< GCircle >(size, colour);   break;
  default:          throw std::runtime_error("new_gobject(): Invalid GObject type!");
  }
  
  return obj;
}



//------------------------------------------------------------------------------
std::shared_ptr< GShape > Window::Manager::new_gobject(gobj_type g_type, glm::vec3 position, float size, glm::vec3 colour){
  std::shared_ptr< GShape > obj;
  
  switch(g_type){
  case t_triangle:  obj = std::make_shared< GTriangle >(position, size, colour);   break;
  case t_rectangle: obj = std::make_shared< GRect >(position, size, colour);   break;
  case t_circle:    obj = std::make_shared< GCircle >(position, size, colour);   break;
  default:          throw std::runtime_error("new_gobject(): Invalid GObject type!");
  }
  
  return obj;
}



//------------------------------------------------------------------------------
std::shared_ptr< GShape > Window::Manager::new_gobject(gobj_type g_type, glm::vec3 position, float rotation, float size, glm::vec3 colour){
  std::shared_ptr< GShape > obj;
  
  switch(g_type){
  case t_triangle:  obj = std::make_shared< GTriangle >(position, rotation, size, colour);   break;
  case t_rectangle: obj = std::make_shared< GRect >(position, rotation, size, colour);   break;
  case t_circle:    obj = std::make_shared< GCircle >(position, rotation, size, colour);   break;
  default:          throw std::runtime_error("new_gobject(): Invalid GObject type!");
  }
  
  return obj;
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