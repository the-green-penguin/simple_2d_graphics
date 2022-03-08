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



void graphics();



int main(int argc, char* argv[]){
	
	graphics();
	
	return 0;
}



//------------------------------------------------------------------------------
void graphics(){
	Window window("Test");
	window.wait_for_setup();
	
	window.add_gobject(
		std::make_shared<GTriangle>(
			glm::vec3(200.0f, 200.0f, 0.0f),
			50.0f,
			glm::vec3(0.5f, 0.5f, 0.0f)
		)
	);
	
	window.add_gobject(
		std::make_shared<GTriangle>(
			glm::vec3(200.0f, 200.0f, 0.0f),
			90.0f, 10.0f,
			glm::vec3(0.25f, 0.25f, 0.7f)
		)
	);
	
	for(int i = 0; i < 100; i++){
		auto gobj = window.graphics_objects;
		{
			std::lock_guard<std::mutex> lg(gobj->second);   // lock vector
  		gobj->first[0]->set_rotation(i);
  		gobj->first[0]->set_position(
    		glm::vec3(200.0f + i, 200.0f + i, 0.0f)
  		);
		}
		
		std::cout << "test\n";
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10ms);
	}
}
