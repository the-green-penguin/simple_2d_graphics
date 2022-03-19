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

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

#include "../window.h"

using namespace std::chrono_literals;



void test();
void create_gobjects(Window& window);
void loop(Window& window);

id obj_1, obj_2, obj_3;



int main(int argc, char* argv[]){
	
	test();
	
	return 0;
}



//------------------------------------------------------------------------------
void test(){
	Window window("Test");
	window.wait_for_setup();   // optional
	
	create_gobjects(window);
	loop(window);
	window.remove_gobject(obj_1);
}



//------------------------------------------------------------------------------
void create_gobjects(Window& window){
	obj_1 = window.add_gobject(
		std::make_shared<GTriangle>(
			glm::vec3(200.0f, 200.0f, 0.0f),
			50.0f,
			glm::vec3(0.5f, 0.5f, 0.0f)
		)
	);
	
	obj_2 = window.add_gobject(
		std::make_shared<GTriangle>(
			glm::vec3(200.0f, 200.0f, 0.0f),
			90.0f, 30.0f,
			glm::vec3(0.25f, 0.25f, 0.7f)
		)
	);
	
	obj_3 = window.add_gobject(
		std::make_shared<GRect>(
			glm::vec3(200.0f, 200.0f, 0.0f),
			0.0f, 30.0f,
			glm::vec3(1.0f, 0.75f, 0.25f)
		)
	);
}



//------------------------------------------------------------------------------
void loop(Window& window){
	
	for(int i = 0; i < 100; i++){
		window.set_gobj_position(
			obj_1, glm::vec3(200.0f + i, 200.0f + i, 0.0f)
		);
		window.set_gobj_rotation(obj_1, i);
		
		window.set_camera_position(
			glm::vec3(0.0f + i/2, 0.0f + i, 0.0f)
		);
		
		std::this_thread::sleep_for(10ms);
	}
}