#include <iostream>

#include "managers/SceneManager.h"
#include "managers/PBRSceneManager.h"

std::string Shader::defaultDirectory = "assets/shaders";

int main() 
{
	std::cout << "PBR" << std::endl;

	/*SceneManager sm;
	sm.Start();
	sm.Update();
	sm.Cleanup();*/

	PBRSceneManager pbrScene;
	pbrScene.Start();
	pbrScene.Update();
	pbrScene.Cleanup();

	return 0;
}
