#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Scene 
{
public:
	static unsigned int SCR_WIDTH;
	static unsigned int SCR_HEIGHT;
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	
	Scene();
	bool Init();
	void SetParametres();

	void Update();
	void NewFrame();

	bool ShouldClose();
	void SetShouldClose(bool shouldClose);

private:
	GLFWwindow* window;
};

#endif

