#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Keyboard {
public:
	/*
		static callback
	*/

	// key state changed
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	/*
		static accessors
	*/

	// get key state
	static bool Key(int key);

	// get if key recently changed
	static bool KeyChanged(int key);

	// get if key recently changed and is up
	static bool KeyWentUp(int key);

	// get if key recently changed and is down
	static bool KeyWentDown(int key);

private:
	/*
		static keyboard values
	*/

	// key state array (true for down, false for up)
	static bool Keys[];
	// key changed array (true if changed)
	static bool KeysChanged[];
};

#endif