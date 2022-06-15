#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include <stack>

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
#include "graphics/models/Gun.hpp"
#include "graphics/models/Sphere.hpp"

#include "physics/Environment.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(double dt);
void LaunchedItem(float dt);

//============================================================================================//
//Settings
//============================================================================================//

unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 1000;

Scene scene;

float mixVal = 0.5f;

glm::mat4 mouseTransform = glm::mat4(1.0f);
Camera Camera::defaultCamera(glm::vec3(-3.0f, 0.0f, -.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

SphereArray launchSpheres;

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


	//============================================================================================//
	//SHADERS
	//============================================================================================//

	Shader shader("assets/object_vs.glsl", "assets/object_fs.glsl");
	Shader lampShader("assets/object_vs.glsl", "assets/lamp_fs.glsl");

	//============================================================================================//
	//MODELS
	//============================================================================================//

	//Model m(glm::vec3(0.0f), glm::vec3(0.03f), true);
	//m.LoadModel("assets/models/gun/scene.gltf");
	//m.LoadModel("assets/models/kirby/scene.gltf");
	//m.LoadModel("assets/models/kirby2/Kirby.fbx");
	//m.LoadModel("assets/models/nanosuit/nanosuit.obj");
	//m.LoadModel("assets/models/pbr_helmet/scene.gltf");
	//m.LoadModel("assets/models/pbr_kirby/source/Robobo_Kirby.obj");
	//Gun g;
	//g.LoadModel("assets/models/gun/scene.gltf");

	launchSpheres.Init();

	//============================================================================================//
	//LIGHTS
	//============================================================================================//

	glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient	= glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse	= glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular	= glm::vec4(1.0f);
	float constant		= 1.0f;
	float linear		= 0.09f;
	float quadratic		= 0.03f;

	DirLight dirLight = { glm::vec3(-0.2f, -0.1f, -0.3f), 
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), 
		glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), 
		glm::vec4(0.75f, 0.75f, 0.75f, 1.0f) 
	};

	/*Lamp lamps[4];
	for (int i = 0; i < 4; i++) {
		lamps[i] = Lamp(glm::vec3(1.0f),
			ambient, diffuse, specular,
			constant, linear, quadratic,
			pointLightPositions[i], glm::vec3(0.25f));
		lamps[i].Init();
	}*/

	LampArray lamps;
	lamps.Init();
	for (int i = 0; i < 4; i++) 
	{
		lamps.instances.push_back({
			pointLightPositions[i],
			constant, linear, quadratic,
			ambient, diffuse, specular,
			});
	}

	SpotLight spotLight = { Camera::defaultCamera.cameraPos, Camera::defaultCamera.cameraFront,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),	// Cut off
		1.0f, 0.07f, 0.032f,	// Attenuation constants
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f) };


	//============================================================================================//
	//Render loop
	//============================================================================================//

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
		view = Camera::defaultCamera.GetViewMatrix();
		projection = glm::perspective(glm::radians(Camera::defaultCamera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.Activate();
		shader.Set3Float("viewPos", Camera::defaultCamera.cameraPos);

		/*dirLight.direction = glm::vec3(
			glm::rotate(model, glm::radians(0.5f), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::vec4(dirLight.direction, 1.0f));*/
		dirLight.Render(shader);

		for (int i = 0; i < 4; i++)
		{
			lamps.instances[i].Render(i, shader);
		}
		shader.SetInt("numPointLights", 4);

		/*spotLight.position = Camera::defaultCamera.cameraPos;
		spotLight.direction = Camera::defaultCamera.cameraFront;
		spotLight.Render(0, shader);
		shader.SetInt("numSpotLights", 1);*/

		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);

		//m.Render(shader);
		//g.Render(shader);

		std::stack<int> removeObjects;
		for (int i = 0; i < launchSpheres.instances.size(); i++)
		{
			if (glm::length(Camera::defaultCamera.cameraPos - launchSpheres.instances[i].pos) > 50.0f)
			{
				removeObjects.push(i);
				continue;
			}
		}
		for (int i = 0; i < removeObjects.size(); i++)
		{
			launchSpheres.instances.erase(launchSpheres.instances.begin() + removeObjects.top());
			removeObjects.pop();
		}

		if (launchSpheres.instances.size() > 0)
		{
			launchSpheres.Render(shader, deltaTime);
		}

		lampShader.Activate();
		lampShader.SetMat4("model", model);
		lampShader.SetMat4("view", view);
		lampShader.SetMat4("projection", projection);
		
		lamps.Render(lampShader, deltaTime);

		// GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		scene.NewFrame();
	}

	//m.Cleanup();
	//g.Cleanup();

	lamps.Cleanup();

	launchSpheres.Cleanup();

	glfwTerminate();

	return 0;
}

/**
 * @brief GLFW: whenever the window size changed (by OS or user resize) this callback function executes
 * @param window 
 * @param width 
 * @param height 
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

/**
 * @brief Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
 * @param dt 
*/
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

	//============================================================================================//
	//Move camera
	//============================================================================================//

	if (Keyboard::Key(GLFW_KEY_W))
	{
		Camera::defaultCamera.UpdateCameraPos(CameraDirection::FORWARD, dt);
	}
	if (Keyboard::Key(GLFW_KEY_S))
	{
		Camera::defaultCamera.UpdateCameraPos(CameraDirection::BACKWARD, dt);
	}
	if (Keyboard::Key(GLFW_KEY_A))
	{
		Camera::defaultCamera.UpdateCameraPos(CameraDirection::LEFT, dt);
	}
	if (Keyboard::Key(GLFW_KEY_D))
	{
		Camera::defaultCamera.UpdateCameraPos(CameraDirection::RIGHT, dt);
	}
	if (Keyboard::Key(GLFW_KEY_SPACE))
	{
		Camera::defaultCamera.UpdateCameraPos(CameraDirection::UP, dt);
	}
	if (Keyboard::Key(GLFW_KEY_LEFT_SHIFT))
	{
		Camera::defaultCamera.UpdateCameraPos(CameraDirection::DOWN, dt);
	}

	double dx = Mouse::GetDX();
	double dy = Mouse::GetDY();
	if (dx != 0 || dy != 0)
	{
		Camera::defaultCamera.UpdateCameraDirection(dx, dy);
	}
	double scrollDy = Mouse::GetScrollDY();
	if (scrollDy != 0)
	{
		Camera::defaultCamera.UpdateCameraZoom(scrollDy);
	}


	//============================================================================================//
	//Apply force
	//============================================================================================//

	if (Keyboard::KeyWentDown(GLFW_KEY_L))
	{
		LaunchedItem(dt);
	}
}

void LaunchedItem(float dt)
{
	RigidBody rb(1.0f, Camera::defaultCamera.cameraPos);
	rb.TransferEnergy(100.0f, Camera::defaultCamera.cameraFront);
	rb.ApplyImpulse(Camera::defaultCamera.cameraFront, 10000.0f, dt);
	rb.ApplyAcceleration(Environment::gravity);
	launchSpheres.instances.push_back(rb);
}
