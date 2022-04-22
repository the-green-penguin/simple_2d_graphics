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

///#define GLFW_INCLUDE_NONE     // not needed/working on Ubuntu, etc.
// #include <glad/gl.h>     // not needed/working on Ubuntu, etc.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>   // sudo apt install libglm-dev
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_program.h"
#include "graphics_object.h"
#include "camera.h"



// .cpp contains non-member function!



typedef unsigned long long int id;   // if you call "add_gobject()" or "open()" more than 2^64 times, it's your problem!



class Window{   // outer Window class exposes only the API
public:
  Window() = delete;   // Window itself is not to be used explicitely
  ~Window() = delete;   // Window itself is not to be used explicitely
  
  static id open();
  static void close(id win_id);
  static bool got_closed(id win_id);
  static std::size_t count();
  static id add_gobject(id win_id, std::shared_ptr< GShape > gobject);
  static void remove_gobject(id win_id, id gobj_id);
  static void clear_gobjects(id win_id);
  static void set_gobj_position(id win_id, id gobj_id, glm::vec3 pos);
  static void set_gobj_rotation(id win_id, id gobj_id, float rot);
  static void set_camera_position(id win_id, glm::vec3 pos);
  static void set_camera_zoom(id win_id, float zoom);
  static void mod_camera_zoom(id win_id, float zoom_diff);
  static void set_allow_zoom(id win_id, bool b);
  static void set_allow_camera_movement(id win_id, bool b);
  static void set_background_colour(id win_id, glm::vec3 colour);
  static void set_window_name(id win_id, const std::string& name);
  
private:
  // predeclare private classes
  class Helper;
  class Wrapper;
  class Manager;
  
//------------------------------------------------------------------------------
  // structs for thread synchronisation
  typedef struct{
    std::unordered_map< id, std::shared_ptr< GShape > > data;
    std::mutex lock;
  } sync_gobjects;

  typedef struct{
    Camera data;
    std::mutex lock;
  } sync_camera;

  typedef struct{
    std::queue<id> data;
    std::mutex lock;
  } sync_ids_to_delete;

  typedef struct{
    glm::vec3 data;
    std::mutex lock;
  } sync_background_colour;

  typedef struct{
    std::string data;
    std::mutex lock;
  } sync_name;
  
//------------------------------------------------------------------------------
  // helper class (actual internal data)
  class Helper{
  public:
    Helper(Wrapper* parent);
    ~Helper();
    void update();
    
    // these are accessed by two threads -> lock needed
    sync_gobjects graphics_objects;
    sync_camera camera;
    sync_ids_to_delete ids_to_delete;
    std::atomic< bool > should_close = false;
    std::atomic< bool > closed = false;
    std::atomic< bool > allow_zoom = false;
    std::atomic< bool > allow_camera_movement = false;
    std::atomic< bool > clear_gobjects = false;
    sync_background_colour background_colour;
    sync_name window_name;
    
  private:
    Wrapper* parent;
    GLFWwindow* window;
    int width, height;
    std::shared_ptr< Shader_Program > shader_program;
    
    void create_glfw_window();
    void change_context(GLFWwindow* window);   // used by both threads
      void load_gl_functions();
    void enable_gl_debugging();
    void setup_shader_program();
    
    void exe_update();   // graphics thread
    void update_name();   // graphics thread
    void render();   // graphics thread
    void set_background();   // graphics thread
    void delete_old_gobjects();   // graphics thread
    void render_gobjects();   // graphics thread
  };
  
//------------------------------------------------------------------------------
  // wrapper handling API calls (API <-> Helper)
  class Wrapper{
  public:
    Wrapper();
    ~Wrapper();
    id add_gobject(std::shared_ptr< GShape > gobject);
    void remove_gobject(id id);
    void clear_gobjects();
    void set_gobj_position(id id, glm::vec3 pos);
    void set_gobj_rotation(id id, float rot);
    void set_camera_position(glm::vec3 pos);
    void set_camera_zoom(float zoom);
    void mod_camera_zoom(float zoom_diff);
    void set_allow_zoom(bool b);
    void set_allow_camera_movement(bool b);
    void set_background_colour(glm::vec3 colour);
    void set_window_name(const std::string& name);
    
    std::shared_ptr< Helper > helper;
    
  private:
    id next_gobj_id = 0;
  };
  
//------------------------------------------------------------------------------
  // window manager (Meyer's singleton), handles separate thread
  class Manager{    
  public:
    static Manager& get_instance();
    id add_win();
    void close_win(id id);
    bool win_got_closed(id id);
    std::size_t get_count();
    
  private:
    // Meyer's singleton
    Manager();
    ~Manager();
    Manager(const Manager&) = delete;   // prevents creation of copies
    Manager& operator=(const Manager&) = delete;   // prevents creation of copies
    
    // "actual" private members
    void init_glfw();
    void thread_func();   // graphics thread
    void remove_closed_windows();   // graphics thread

    id next_win_id = 0;
    std::thread graphics_thread;
    std::atomic< bool > stop_thread = false;
    std::unordered_map< id, std::shared_ptr< Wrapper > > windows;
  };
};