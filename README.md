#How to use:
  1) Create directory `bin/`
  2) Run `make` or `make release`
  3) Integrate `libsimple_2d.a` into your project (see `example/Makefile`)
  4) Use the API
  
  
  
#API:
  id          Window::open                      ()  
    -> Opens new window with uninitialised name and return its id  
    
  id          Window::open                      (const std::string& name)  
    -> Opens new window with given name and returns its id  
    
  void        Window::close                     (id win_id)  
    -> Closes specified window  
    
  bool        Window::got_closed                (id win_id)  
    -> Returns `true` if specified window got closed, `true` otherwise  
    
  std::size_t Window::count                     ()  
    -> Returns the amount of active windows  
    
  id          Window::add_gobject               (id win_id, std::shared_ptr< GShape > gobject)  
    -> Adds a graphics_object to the specified window  
    
  void        Window::remove_gobject            (id win_id, id gobj_id)  
    -> Removes specified graphics_object from specified window  
    
  void        Window::clear_gobjects            (id win_id)  
    -> Removes all graphics_objects from specified window  
    
  void        Window::set_gobj_position         (id win_id, id gobj_id, glm::vec3 pos)  
    -> Sets position of specified graphics_object inside specified window  
    
  void        Window::set_gobj_rotation         (id win_id, id gobj_id, float rot)  
    -> Sets rotation of specified graphics_object inside specified window  
    
  void        Window::set_camera_position       (id win_id, glm::vec3 pos)  
    -> Sets camera position inside specified window  
    
  void        Window::set_camera_zoom           (id win_id, float zoom)  
    -> Sets camera zoom inside specified window  
    
  void        Window::mod_camera_zoom           (id win_id, float zoom_diff)  
    -> Sets camera zoom relative to its current zoom level inside specified window  
    
  void        Window::set_allow_zoom            (id win_id, bool b)  
    -> Toggles whether camera zoom is possible or not  
    
  void        Window::set_allow_camera_movement (id win_id, bool b)  
    -> Toggles whether camera movement is possible or not  
    
  void        Window::set_background_colour     (id win_id, glm::vec3 colour)  
    -> Sets background colour of specified window  
    
  void        Window::set_window_name           (id win_id, const std::string& name)  
    -> Sets name of specified winow  
      
  <obj>       GTriangle/GRect/GCircle           (glm::vec3 position, float size, glm::vec3 colour)  
  <obj>       GTriangle/GRect/GCircle           (glm::vec3 position, float rotation, float size, glm::vec3 colour)  
  <obj>       GTriangle/GRect/GCircle           (glm::vec3 position, const std::vector<Vertex>& vertices)  
  <obj>       GTriangle/GRect/GCircle           (glm::vec3 position, float rotation, const std::vector<Vertex>& vertices)  
    -> Constructs a graphics_object of type triangle/rectangle/circle with specified properties  
    -> If `vertices` are given and their count does not match the type, an exception will be thrown!  
    -> This object can then be added to a window  
    -> Adding it to multiple windows leads to undefined behaviour!  
      
  ##Do NOT use any other functions that might be visible from this library!
    
  
  
#Developed and tested for GNU/Linux (Ubuntu-based).
  
#Install to use (not necessary if build-dependencies are installed already):
  libglew2.1  
  libglfw3  
  
#Install to build:
  libglm-dev  
  libglew-dev  
  libglfw3-dev  
  
