#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Keyboard {
public:
	// Key state changed
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// Get key state
	static bool Key(int key);

	// Get if key recently changed
	static bool KeyChanged(int key);

	// Get if key recently changed and is up
	static bool KeyWentUp(int key);

	// Get if key recently changed and is down
	static bool KeyWentDown(int key);

private:
	// Key state array (true for down, false for up)
	static bool Keys[];

	// Key changed array (true if changed)
	static bool KeysChanged[];
};

#endif