#include "../Model.h"
#include "../../Camera.h"

class Gun :public Model
{
public:
	Gun() :Model(glm::vec3(0.0f), glm::vec3(1 / 300.0f), true) {}

	void Render(Shader shader, bool setModel = false)
	{
		glm::mat4 model = glm::mat4(1.0f);
		
		//============================================================================================//
		//Set position
		//============================================================================================//

		rb.pos = Camera::defaultCamera.cameraPos + glm::vec3(Camera::defaultCamera.cameraFront * 0.5f) - glm::vec3(Camera::defaultCamera.cameraUp * 0.2f);
		model = glm::translate(model, rb.pos);

		//============================================================================================//
		//Rotate
		//============================================================================================//
		
		float theta;

		// Around cameraRight
		theta = acos(glm::dot(Camera::defaultCamera.worldUp, Camera::defaultCamera.cameraFront) /
			glm::length(Camera::defaultCamera.cameraUp / glm::length(Camera::defaultCamera.cameraFront)));
		model = glm::rotate(model, atanf(1) * 2 - theta, Camera::defaultCamera.cameraRight);

		// Around cameraUp
		glm::vec2 front2d = glm::vec2(Camera::defaultCamera.cameraFront.x, Camera::defaultCamera.cameraFront.z);
		theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), front2d) / glm::length(front2d));
		model = glm::rotate(model, Camera::defaultCamera.cameraFront.z < 0 ? theta : -theta, glm::vec3(0.0f, 1.0f, 0.0f));
		

		model = glm::scale(model, size);

		shader.SetMat4("model", model);
		Model::Render(shader, false);
	}
};