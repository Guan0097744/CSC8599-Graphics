#pragma once

#include "../Scene.h"

#include "../graphics/models/Sphere.hpp"
#include "../graphics/models/Lamp.hpp"

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	
	void Start();
	void Update();
	void Cleanup();

protected:
	void Setup();
	void SetCamera();
	void SetFonts();
	void SetShaders();
	void SetLightings();
	void SetModels();
	void AddModel(Model* m, glm::vec3 size = glm::vec3(1.0f), float mass = 1.0f, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f));
	void RenderScene(Shader& shader);

	void LaunchItem(float dt);
	void ProcessInput(double dt);

	float			dt = 0.0f;
	float			lastFrame = 0.0f;

	Scene			scene;
	Camera			cam;
	Box				box;

	TextRenderer*	font;

	Shader*			shader;
	Shader*			boxShader;
	Shader*			pbrShader;

	DirLight*		dirLight;
	PointLight*		pointLight;
	SpotLight*		spotLight;

	Sphere*			sphere;
	Lamp*			lamp;
	Model*			kirby;
};
