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
	//static Camera defaultCamera;

	//============================================================================================//
	//Camera variables
	//============================================================================================//

	// position
	glm::vec3 cameraPos;

	//============================================================================================//
	//Camera directional values
	//============================================================================================//

	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;
	//glm::vec3 worldUp;

	//============================================================================================//
	//Camera rotational values
	//============================================================================================//

	// y-axis
	float yaw;
	// x-axis
	float pitch;

	//============================================================================================//
	//Camera movement values
	//============================================================================================//

	float speed;
	float sensitivity;
	float zoom;

	//============================================================================================//
	//Utility
	//============================================================================================//

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 10.0f));
	void UpdateCameraDirection(double dx, double dy);
	void UpdateCameraPos(CameraDirection direction, double dt);
	void UpdateCameraZoom(double dy);
	glm::mat4 GetViewMatrix();
	float GetZoom();

private:
	void UpdateCameraVectors();
};

#endif