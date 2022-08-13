#include <iostream>

#include "PBR/PBRSceneManager.h"
//#include "managers/SceneManager.h"

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
