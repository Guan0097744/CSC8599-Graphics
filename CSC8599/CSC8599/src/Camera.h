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
	glm::vec3 worldUp;

	//============================================================================================//
	//Camera rotational values
	//============================================================================================//

	// x-axis
	float yaw;
	// y-axis
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

	Camera(glm::vec3 position);
	void UpdateCameraDirection(double dx, double dy);
	void UpdateCameraPos(CameraDirection direction, double dt);
	void UpdateCameraZoom(double dy);
	glm::mat4 GetViewMatrix();
	float GetZoom();

private:
	void UpdateCameraVectors();
};

#endif