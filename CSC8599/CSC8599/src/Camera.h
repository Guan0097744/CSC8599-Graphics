#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraDirection 
{
	NONE = 0,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera 
{
public:
	// position
	glm::vec3 cameraPos;

	// camera directional values
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::vec3 worldUp;

	// camera rotational values
	float yaw; // x-axis
	float pitch; // y-axis

	// camera movement values
	float speed;
	float sensitivity;
	float zoom;

	// default and initialize with position
	Camera(glm::vec3 position);

	// change camera direction (mouse movement)
	void UpdateCameraDirection(double dx, double dy);

	// change camera position in certain direction (keyboard)
	void UpdateCameraPos(CameraDirection direction, double dt);

	// change camera zoom (scroll wheel)
	void UpdateCameraZoom(double dy);

	// get view matrix for camera
	glm::mat4 GetViewMatrix();

	// get zoom value for camera
	float GetZoom();

private:
	// change camera directional vectors based on movement
	void UpdateCameraVectors();
};

#endif