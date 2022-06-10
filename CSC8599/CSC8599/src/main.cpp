#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "graphics/Light.h"
#include "graphics/Model.h"

#include "io/Mouse.h"
#include "io/Keyboard.h"

#include "Camera.h"
#include "Scene.h"

#include "graphics/models/Cube.hpp"
#include "graphics/models/Lamp.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(double dt);

// settings
unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 1000;

Scene scene;

float mixVal = 0.5f;

glm::mat4 mouseTransform = glm::mat4(1.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() 
{
	glfwInit();

	// OpenGL Ver3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

	// GLFW window creation
	if (!scene.Init())
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	scene.SetParametres();

	glEnable(GL_DEPTH_TEST);

	Shader shader("assets/object_vs.glsl", "assets/object_fs.glsl");
	Shader lampShader("assets/object_vs.glsl", "assets/lamp_fs.glsl");

	Model m(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(0.05f));
	//m.LoadModel("assets/models/nanosuit/nanosuit.obj");
	//m.LoadModel("assets/models/kirby/scene.gltf");
	m.LoadModel("assets/models/kirby2/Kirby.fbx");
	//m.LoadModel("assets/models/gun/scene.gltf");
	//m.LoadModel("assets/models/shiba/scene.gltf");

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	DirLight dirLight = { glm::vec3(-0.2f, -0.1f, -0.3f), 
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 
		glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), 
		glm::vec4(0.75f, 0.75f, 0.75f, 1.0f) 
	};

	Lamp lamps[4];
	for (int i = 0; i < 4; i++) {
		lamps[i] = Lamp(glm::vec3(1.0f),
			glm::vec4(0.05f, 0.05f, 0.05f, 1.0f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), glm::vec4(1.0f),
			1.0f, 0.07f, 0.032f,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].Init();
	}

	SpotLight spotLight = { camera.cameraPos, camera.cameraFront, 
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),	// Cut off
		1.0f, 0.07f, 0.032f,	// Attenuation constants
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f) };

	/* 
		Render loop
	*/
	while (!scene.ShouldClose())
	{
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(deltaTime);

		// Render
		scene.Update();

		// Create transformation for screen
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.Activate();
		shader.Set3Float("viewPos", camera.cameraPos);

		/*dirLight.direction = glm::vec3(
			glm::rotate(model, glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::vec4(dirLight.direction, 1.0f));*/
		dirLight.Render(shader);

		for (int i = 0; i < 4; i++)
		{
			lamps[i].pointLight.Render(i, shader);
		}
		shader.SetInt("numPointLights", 4);

		spotLight.position = camera.cameraPos;
		spotLight.direction = camera.cameraFront;
		spotLight.Render(0, shader);
		shader.SetInt("numSpotLights", 1);

		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);

		m.Render(shader);

		lampShader.Activate();
		lampShader.SetMat4("model", model);
		lampShader.SetMat4("view", view);
		lampShader.SetMat4("projection", projection);
		for (int i = 0; i < 4; i++)
		{
			lamps[i].Render(lampShader);
		}

		// GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		scene.NewFrame();
	}

	m.Cleanup();

	for (int i = 0; i < 4; i++)
	{
		lamps[i].Cleanup();
	}

	glfwTerminate();

	return 0;
}

// GLFW: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(double dt)
{
	if (Keyboard::Key(GLFW_KEY_ESCAPE))
		scene.SetShouldClose(true);
		
	if (Keyboard::Key(GLFW_KEY_UP))
	{
		mixVal += 0.1f;
		if (mixVal > 1)
		{
			mixVal = 1.0f;
		}
	}
	if (Keyboard::Key(GLFW_KEY_DOWN))
	{
		mixVal -= 0.1f;
		if (mixVal < 0)
		{
			mixVal = 0.0f;
		}
	}

	// Move camera
	if (Keyboard::Key(GLFW_KEY_W))
	{
		camera.UpdateCameraPos(CameraDirection::FORWARD, dt);
	}
	if (Keyboard::Key(GLFW_KEY_S))
	{
		camera.UpdateCameraPos(CameraDirection::BACKWARD, dt);
	}
	if (Keyboard::Key(GLFW_KEY_A))
	{
		camera.UpdateCameraPos(CameraDirection::LEFT, dt);
	}
	if (Keyboard::Key(GLFW_KEY_D))
	{
		camera.UpdateCameraPos(CameraDirection::RIGHT, dt);
	}
	if (Keyboard::Key(GLFW_KEY_SPACE))
	{
		camera.UpdateCameraPos(CameraDirection::UP, dt);
	}
	if (Keyboard::Key(GLFW_KEY_LEFT_SHIFT))
	{
		camera.UpdateCameraPos(CameraDirection::DOWN, dt);
	}

	double dx = Mouse::GetDX();
	double dy = Mouse::GetDY();
	if (dx != 0 || dy != 0)
	{
		camera.UpdateCameraDirection(dx, dy);
	}
	double scrollDy = Mouse::GetScrollDY();
	if (scrollDy != 0)
	{
		camera.UpdateCameraZoom(scrollDy);
	}
}
