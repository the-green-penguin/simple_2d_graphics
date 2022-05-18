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
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <queue>
#include <chrono>

///#define GLFW_INCLUDE_NONE     // not needed/working on Ubuntu, etc.
// #include <glad/gl.h>     // not needed/working on Ubuntu, etc.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "graphics_object.h"
#include "camera.h"
#include "utils.h"



// .cpp contains non-member function!



enum gobj_type{
  t_triangle,
  t_rectangle,
  t_circle
};



class Window{   // outer Window class exposes only the API
public:
  Window() = delete;   // Window itself is not to be used explicitely
  ~Window() = delete;   // Window itself is not to be used explicitely
  
  static id open();
  static id open(const std::string& name);
  static void close(id win_id);
  static bool got_closed(id win_id);
  static std::size_t count();
  static id add_gobject(id win_id, gobj_type g_type, float size, glm::vec3 colour);
  static id add_gobject(id win_id, gobj_type g_type, glm::vec3 position, float size, glm::vec3 colour);
  static id add_gobject(id win_id, gobj_type g_type, glm::vec3 position, float rotation, float size, glm::vec3 colour);
  static void remove_gobject(id win_id, id gobj_id);
  static void clear_gobjects(id win_id);
  static void set_gobj_position(id win_id, id gobj_id, glm::vec3 position);
  static void set_gobj_rotation(id win_id, id gobj_id, float rotation);
  static void set_camera_position(id win_id, glm::vec3 pos);
  static void set_camera_zoom(id win_id, float zoom);
  static void mod_camera_zoom(id win_id, float zoom_diff);
  static void set_allow_zoom(id win_id, bool b);
  static void set_allow_camera_movement(id win_id, bool b);
  static void set_background_colour(id win_id, glm::vec3 colour);
  static void set_window_name(id win_id, const std::string& name);
  
  
  
private:
  typedef struct{
    std::queue< Thread_Message > data;
    std::mutex mutex;
  }thread_msg_queue;
  
//------------------------------------------------------------------------------
  // wrapper class (actual internal data)
  class Wrapper{
  public:
    Wrapper(id w_id);
    ~Wrapper();
    void update();
    void update_name(const std::string& name);
    
    std::unordered_map< id, std::shared_ptr< GShape > > graphics_objects;
    Camera camera;
    bool allow_zoom = false;
    bool allow_camera_movement = false;
    glm::vec3 background_colour = {0.0f, 0.0f, 0.0f};
    std::string window_name = "";
    
  private:
    id w_id;
    GLFWwindow* window;
    int width, height;
    std::shared_ptr< Shader_Program > shader_program;
    
    void create_glfw_window();
    void load_gl_functions();
    void enable_gl_debugging();
    void setup_shader_program();
    
    void exe_update();   // graphics thread
    void render();   // graphics thread
    void set_background();   // graphics thread
    void render_gobjects();   // graphics thread
  };
  
  
  
//------------------------------------------------------------------------------
  // window manager (Meyer's singleton), handles separate thread
  class Manager{    
  public:
    static Manager& get_instance();
    bool win_got_closed(id id);
    std::size_t get_count();
    static void push_msg_from_API(const Thread_Message& msg);
    static void process_msgs_to_API();
    static id get_next_win_id();
    static id get_next_gobj_id();
    
    thread_msg_queue messages_from_API;
    thread_msg_queue messages_to_API;
    
  private:
    // Meyer's singleton
    Manager();
    ~Manager();
    Manager(const Manager&) = delete;   // prevents creation of copies
    Manager& operator=(const Manager&) = delete;   // prevents creation of copies
    
    // "actual" private members
    void init_glfw();
    void thread_func();   // graphics thread
    void update_windows();   // graphics thread
    void wait_until_next_frame();   // graphics thread
    void push_msg_to_API(const Thread_Message& msg);   // graphics thread
    void process_msgs_from_API();   // graphics thread
    void process_msg(Thread_Message& msg);   // both threads
    void add_win(id win_id, const std::string& name);   // graphics thread
    void close_win(id id);   // graphics thread
    void add_new_gobject(id win_id, id gobj_id, std::shared_ptr< GShape > obj);   // graphics thread
    void remove_gobject(id win_id, id gobj_id);   // graphics thread
    void clear_gobjects(id win_id);   // graphics thread
    void set_gobj_position(id win_id, id gobj_id, glm::vec3 position);   // graphics thread
    void set_gobj_rotation(id win_id, id gobj_id, float rotation);   // graphics thread
    void set_camera_position(id win_id, glm::vec3 pos);   // graphics thread
    void set_camera_zoom(id win_id, float zoom);   // graphics thread
    void mod_camera_zoom(id win_id, float zoom_diff);   // graphics thread
    void set_allow_zoom(id win_id, bool b);   // graphics thread
    void set_allow_camera_movement(id win_id, bool b);   // graphics thread
    void set_background_colour(id win_id, glm::vec3 colour);   // graphics thread
    void set_window_name(id win_id, const std::string& name);   // graphics thread

    id next_win_id = 0;
    id next_gobj_id = 0;
    std::thread graphics_thread;
    std::atomic< bool > stop_thread = false;   // both threads
    std::chrono::steady_clock::time_point prev_time;   // graphics thread
    const uint fps = 60;   // graphics thread
    std::size_t window_count = 0;
    std::unordered_map< id, bool > got_closed;
    std::unordered_map< id, std::shared_ptr< Wrapper > > windows;   // graphics thread
  };



//------------------------------------------------------------------------------
  // actual private functions of Window
  static std::shared_ptr< GShape > new_gobject(gobj_type g_type, float size, glm::vec3 colour);
  static std::shared_ptr< GShape > new_gobject(gobj_type g_type, glm::vec3 position, float size, glm::vec3 colour);
  static std::shared_ptr< GShape > new_gobject(gobj_type g_type, glm::vec3 position, float rotation, float size, glm::vec3 colour);
};