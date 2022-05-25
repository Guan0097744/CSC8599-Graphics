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

#include "io/Mouse.h"
#include "io/Keyboard.h"

#include "Camera.h"
#include "Scene.h"

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

	float vertices[] = {
		// Positions		// Texture Coordinates
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the VAO first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set attribute pointer
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Textures
	
	Texture texture1("assets/takagi.jpg", "texture1");
	texture1.Load();
	Texture texture2("assets/takagi2.jpg", "texture2");
	texture2.Load();

	shader.Activate();
	shader.SetInt("texture1", texture1.id);
	shader.SetInt("texture2", texture2.id);

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

		glActiveTexture(GL_TEXTURE0); // Activate the texture unit first before binding texture
		texture1.Bind();
		glActiveTexture(GL_TEXTURE1);
		texture2.Bind();

		glBindVertexArray(VAO);

		// Create transformation for screen
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		model = glm::rotate(model, /*(float)glfwGetTime() **/ glm::radians(-55.0f), glm::vec3(0.5f));
		//view = glm::translate(view, glm::vec3(-x, -y, -z));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.Activate();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// Wireframe Mode
		shader.SetFloat("mixVal", mixVal);
		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		// GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		scene.NewFrame();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader.id);

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
