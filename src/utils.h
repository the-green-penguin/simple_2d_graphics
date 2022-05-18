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
#include <variant>
#include <tuple>

#include <glm/glm.hpp>

#include "graphics_object.h"



typedef unsigned long long int id;
typedef uint microsecs;



struct Thread_Message{
  // type
  enum msg_type{
    open_win,
    close_win,
    got_closed,
    count_win,
    add_gobject,
    remove_gobject,
    clear_gobjects,
    set_gobj_position,
    set_gobj_rotation,
    set_camera_position,
    set_camera_zoom,
    mod_camera_zoom,
    set_allow_zoom,
    set_allow_camera_movement,
    set_background_colour,
    set_window_name
  } type;
  
  // parameters
  std::variant<
    id,
    std::size_t,
    std::string,
    std::tuple<id, bool>,
    std::tuple<id, float>,
    std::tuple<id, id>,
    std::tuple<id, glm::vec3>,
    std::tuple<id, std::string>,
    std::tuple<id, id, float>,
    std::tuple<id, id, glm::vec3>,
    std::tuple<id, id, std::shared_ptr< GShape > >
  > parameters;
};