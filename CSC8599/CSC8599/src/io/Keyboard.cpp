#include "Keyboard.h"

// key state array (true for down, false for up)
bool Keyboard::Keys[GLFW_KEY_LAST] = { 0 };
// key changed array (true if changed)
bool Keyboard::KeysChanged[GLFW_KEY_LAST] = { 0 };

// key state changed
void Keyboard::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action != GLFW_RELEASE) 
	{
		if (!Keys[key]) 
		{
			Keys[key] = true;
		}
	}
	else 
	{
		Keys[key] = false;
	}
	KeysChanged[key] = action != GLFW_REPEAT;
}

// get key state
bool Keyboard::Key(int key) 
{
	return Keys[key];
}

// get if key recently changed
bool Keyboard::KeyChanged(int key) 
{
	bool ret = KeysChanged[key];
	// set to false because change no longer new
	KeysChanged[key] = false;
	return ret;
}

// get if key recently changed and is up
bool Keyboard::KeyWentDown(int key) 
{
	return Keys[key] && KeyChanged(key);
}

// get if key recently changed and is down
bool Keyboard::KeyWentUp(int key) 
{
	return !Keys[key] && KeyChanged(key);
}