#include "Camera.h"

/**
 * @brief Default and initialize with position
 * @param position 
*/
Camera::Camera(glm::vec3 position): 
	cameraPos(position),
	worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	yaw(0.0f),
	pitch(0.0f),
	speed(2.0f),
	sensitivity(0.5f),
	zoom(45.0f),
	cameraFront(glm::vec3(1.0f, 0.0f, 0.0f))
{
	UpdateCameraVectors();
}

/**
 * @brief Change camera direction (mouse movement)
 * @param dx 
 * @param dy 
*/
void Camera::UpdateCameraDirection(double dx, double dy) 
{
	dx *= sensitivity * 0.1f;
	dy *= sensitivity * 0.1f;

	yaw += dx;
	pitch += dy;

	if (pitch > 89.0f) 
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f) 
	{
		pitch = -89.0f;
	}

	UpdateCameraVectors();
}

/**
 * @brief Change camera position in certain direction (keyboard)
 * @param direction 
 * @param dt 
*/
void Camera::UpdateCameraPos(CameraDirection direction, double dt) 
{
	float velocity = (float)dt * speed;

	switch (direction) 
	{
	case CameraDirection::FORWARD:
		cameraPos += cameraFront * velocity;
		break;
	case CameraDirection::BACKWARD:
		cameraPos -= cameraFront * velocity;
		break;
	case CameraDirection::RIGHT:
		cameraPos += cameraRight * velocity;
		break;
	case CameraDirection::LEFT:
		cameraPos -= cameraRight * velocity;
		break;
	case CameraDirection::UP:
		cameraPos += cameraUp * velocity;
		break;
	case CameraDirection::DOWN:
		cameraPos -= cameraUp * velocity;
		break;
	}
}

/**
 * @brief Change camera zoom (scroll wheel)
 * @param dy 
*/
void Camera::UpdateCameraZoom(double dy) 
{
	if (zoom >= 1.0f && zoom <= 45.0f) 
	{
		zoom -= dy;
	}
	else if (zoom < 1.0f) 
	{
		zoom = 1.0f;
	}
	else // > 45.0f
	{ 
		zoom = 45.0f;
	}
}

/**
 * @brief Get view matrix for camera
 * @return 
*/
glm::mat4 Camera::GetViewMatrix() 
{
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

/**
 * @brief Get zoom value for camera
 * @return 
*/
float Camera::GetZoom() 
{
	return zoom;
}

/**
 * @brief Change camera directional vectors based on movement
*/
void Camera::UpdateCameraVectors() 
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	cameraFront = glm::normalize(direction);
	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp	= glm::normalize(glm::cross(cameraRight, cameraFront));
}