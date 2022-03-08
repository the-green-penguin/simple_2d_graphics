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

#include "../window.h"



void graphics();



int main(int argc, char* argv[]){
	
	graphics();
	///std::thread test(graphics);
	///test.join();
	
	return 0;
}



//------------------------------------------------------------------------------
void graphics(){
	Window window("Test");
	///window.run();
	
	///window.add_gobject(
	///	std::make_shared<GTriangle>(
	///		glm::vec3(200.0f, 200.0f, 0.0f),
	///		50.0f,
	///		glm::vec3(0.5f, 0.5f, 0.0f)
	///	)
	///);
	
	
	///float time = glfwGetTime();
	///auto gobj = window.graphics_objects;
  ///gobj.front()->set_rotation(time * 10);
  ///gobj.front()->set_position(
  ///  glm::vec3(200.0f + time * 10, 200.0f + time * 10, 0.0f)
  ///);
	
	
}
