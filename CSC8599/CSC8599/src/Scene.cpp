#include "Scene.h"

#include "io/Keyboard.h"
#include "io/Mouse.h"

unsigned int Scene::SCR_WIDTH = 1000;
unsigned int Scene::SCR_HEIGHT = 1000;

// GLFW: whenever the window size changed (by OS or user resize) this callback function executes
void Scene::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

Scene::Scene() :window(nullptr)
{

}

bool Scene::Init()
{
	// GLFW window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSC8599", NULL, NULL);
	if (window == NULL)
	{
		return false;
	}

	glfwMakeContextCurrent(window);
	return true;
}

void Scene::SetParametres()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, Scene::framebuffer_size_callback);
	
	glfwSetKeyCallback(window, Keyboard::KeyCallback);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, Mouse::CursorPosCallback);
	glfwSetScrollCallback(window, Mouse::MouseWheelCallback);
	glfwSetMouseButtonCallback(window, Mouse::MouseButtonCallback);
}

void Scene::Update()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::NewFrame()
{
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Scene::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Scene::SetShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

