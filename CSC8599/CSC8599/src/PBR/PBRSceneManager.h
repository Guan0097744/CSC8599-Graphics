#pragma once

#include "PBRScene.h"
#include "PBRCubemap.h"

#include "../graphics/models/Sphere.hpp"
#include "../graphics/models/PBRModel.hpp"

class PBRSceneManager
{
public:
	PBRSceneManager();
	~PBRSceneManager();

	void Start();
	void Update();
	void Cleanup();

protected:
	void Setup();
	void SetCamera();
	void SetFonts();
	void SetShaders();
	void SetLightings();
	void SetMap();
	void SetModels();
	void AddModel(Model* m, glm::vec3 size = glm::vec3(1.0f), float mass = 1.0f, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f));
	void RenderScene(Shader& shader);

	void ProcessInput(double dt);

	float			dt			= 0.0f;
	float			lastFrame	= 0.0f;

	PBRScene		scene;
	Camera			cam;
	Box				box;

	TextRenderer*	font;

	//============================================================================================//
	//Shaders
	//============================================================================================//

	//Shader*			boxShader;
	Shader*			pbrShader;
	Shader*			equirectangularShader;
	Shader*			prefilterShader;
	Shader*			irradianceShader;
	Shader*			brdfShader;
	Shader*			backgroundShader;

	//============================================================================================//
	//Cubemap
	//============================================================================================//

	//unsigned int	hdrTexture;
	//PBRCubemap		pbrMap;

	//============================================================================================//
	//Models
	//============================================================================================//
	
	PBRModel*		pbrModel;
	Sphere*			sphere;

};