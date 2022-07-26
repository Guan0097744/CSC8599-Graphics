#include <iostream>

#include "managers/SceneManager.h"

std::string Shader::defaultDirectory = "assets/shaders";

int main() 
{
	std::cout << "I hate OpenGL!" << std::endl;

	SceneManager sm;

	sm.Start();
	sm.Update();
	sm.Cleanup();

	return 0;
}
