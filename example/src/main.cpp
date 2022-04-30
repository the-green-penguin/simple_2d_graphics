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
#include <vector>

#include "../../src/window.h"

using namespace std::chrono_literals;



void test();
void create_gobjects();
void loop();

std::vector<id> windows;
std::vector<id> objects;



int main(int argc, char* argv[]){
	
	test();
	
	// keep running until windows are closed
	while(Window::count() != 0)
		std::this_thread::sleep_for(10ms);
	
	return 0;
}



//------------------------------------------------------------------------------
void test(){
	windows.push_back( Window::open("Test") );
	windows.push_back( Window::open() );
	
	create_gobjects();
	loop();
}



//------------------------------------------------------------------------------
void create_gobjects(){
	id obj;
	
	obj = Window::add_gobject(
		windows[0],
		t_triangle,
		{200.0f, 200.0f, 0.0f},
		50.0f,
		{0.5f, 0.5f, 0.0f}
	);
	objects.push_back(obj);
	
	obj = Window::add_gobject(
		windows[0],
		t_triangle,
		{200.0f, 200.0f, 0.0f},
		90.0f,
		30.0f,
		{0.25f, 0.25f, 0.7f}
	);
	objects.push_back(obj);
	
	obj = Window::add_gobject(
		windows[0],
		t_rectangle,
		{200.0f, 200.0f, 0.0f},
		0.0f,
		30.0f,
		{1.0f, 0.75f, 0.25f}
	);
	objects.push_back(obj);
	
	obj = Window::add_gobject(
		windows[0],
		t_circle,
		{100.0f, 100.0f, 0.0f},
		0.0f,
		100.0f,
		{1.0f, 0.75f, 0.25f}
	);
	objects.push_back(obj);
}



//------------------------------------------------------------------------------
void loop(){
	
	for(int i = 0; i < 100; i++){
		// move triangle
		Window::set_gobj_position(
			windows[0],
			objects[0],
			{200.0f + i, 200.0f + i, 0.0f}
		);
		
		// rotate circle
		Window::set_gobj_rotation(
			windows[0],
			objects[3],
			i * 10
		);
		
		// adjust camera
		Window::set_camera_position(
			windows[0],
			{0.0f + i/2, 0.0f + i, 0.0f}
		);
		Window::set_camera_zoom(
			windows[0],
			1.0f + i * 0.01f
		);
		
		// delay
		std::this_thread::sleep_for(10ms);
	}
}