#include <iostream>

#include "managers/SceneManager.h"

std::string Shader::defaultDirectory = "assets/shaders";

int main() 
{
	std::cout << "I hate OpenGL!" << std::endl;

	/*
	scene = Scene(3, 3, "OpenGL Tutorial", 1280, 720);

	// GLFW window creation
	if (!scene.Init())
	{
		std::cout << "Failed to create window" << std::endl;
		scene.Cleanup();
		return -1;
	}

	// Set camera
	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;

	//============================================================================================//
	//SHADERS
	//============================================================================================//

	Shader::LoadIntoDefault("defaultHead.glsl");

	Shader shader(true, "instanced/instanced_vs.glsl", "object_fs.glsl");
	Shader boxShader(false, "instanced/box_vs.glsl", "instanced/box_fs.glsl");

	Shader dirShadowShader(false, "shadows/dirSpotShadow.vs", "shadows/dirShadow.fs");
	Shader spotShadowShader(false, "shadows/dirSpotShadow.vs", "shadows/pointSpotShadow.fs");
	Shader pointShadowShader(false, "shadows/pointShadow.vs", "shadows/pointSpotShadow.fs", "shadows/pointShadow.gs");

	Shader::ClearDefault();

	//============================================================================================//
	//FONTS
	//============================================================================================//

	TextRenderer font(32);
	if (!scene.RegisterFont(&font, "comic", "assets/fonts/comic.ttf")) 
	{
		std::cout << "Could not load font" << std::endl;
	}

	//============================================================================================//
	//MODELS
	//============================================================================================//

	scene.RegisterModel(&lamp);

	scene.RegisterModel(&wall);

	scene.RegisterModel(&sphere);

	//scene.RegisterModel(&cube);

	Box box;
	box.Init();

	// load all model data
	scene.LoadModels();

	//============================================================================================//
	//LIGHTS
	//============================================================================================//

	// directional light
	DirLight dirLight(glm::vec3(-0.2f, -0.9f, -0.2f),
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
		glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
		BoundingRegion(glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 50.0f)));
	scene.dirLight = &dirLight;
	States::Activate(&scene.dirLightActive, true);

	// point lights
	glm::vec3 pointLightPositions[] = 
	{
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f,  15.0f,  0.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.0014f;
	float k2 = 0.000007f;

	PointLight pointLights[4];

	for (unsigned int i = 0; i < 1; i++) 
	{
		pointLights[i] = PointLight(
			pointLightPositions[i],
			k0, k1, k2,
			ambient, diffuse, specular,
			0.5f, 50.0f
		);
		// create physical model for each lamp
		//scene.GenerateInstance(lamp.id, glm::vec3(10.0f, 0.25f, 10.0f), 0.25f, pointLightPositions[i]);
		// add lamp to scene's light source
		scene.pointLights.push_back(&pointLights[i]);
		// activate lamp in scene
		States::ActivateIndex(&scene.activePointLights, i);
	}

	// spot light
	SpotLight spotLight(
		//glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		cam.cameraPos, cam.cameraFront, cam.cameraUp,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.0014f, 0.000007f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f),
		0.1f, 100.0f
	);
	scene.spotLights.push_back(&spotLight);
	//scene.activeSpotLights = 1; // 0b00000001

	//scene.GenerateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -3.0f, 0.0f));

	glm::vec3 cubePositions[] = 
	{
		{ 1.0f, 3.0f, -5.0f },
		{ -7.25f, 2.1f, 1.5f },
		{ -15.0f, 2.55f, 9.0f },
		{ 4.0f, -3.5f, 5.0f },
		{ 2.8f, 1.9f, -6.2f },
		{ 3.5f, 6.3f, -1.0f },
		{ -3.4f, 10.9f, -5.5f },
		{ 0.0f, 11.0f, 0.0f },
		{ 0.0f, 5.0f, 0.0f }
	};
	for (unsigned int i = 0; i < 9; i++) 
	{
		//scene.GenerateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
	}

	// instantiate the brickwall plane
	scene.GenerateInstance(wall.id, glm::vec3(1.0f), 1.0f,
		{ 0.0f, 0.0f, 2.0f }, { -1.0f, glm::pi<float>(), 0.0f });

	scene.GenerateInstance(sphere.id, glm::vec3(1.0f), 1.0f, { 0.0f, 0.0f, -2.0f });

	// instantiate instances
	scene.InitInstances();

	// finish preparations (octree, etc)
	scene.Prepare(box, { shader });

	scene.variableLog["time"] = (double)0.0;

	scene.defaultFBO.Bind(); // bind default framebuffer

	//============================================================================================//
	//Render loop
	//============================================================================================//

	while (!scene.ShouldClose())
	{
		double currentTime	= glfwGetTime();
		dt					= currentTime - lastFrame;
		lastFrame			= currentTime;

		scene.variableLog["time"] += dt;
		scene.variableLog["fps"] = 1 / dt;

		ProcessInput(dt);

		// Render
		scene.Update();

		// activate the directional light's FBO

		// remove launch objects if too far
		for (int i = 0; i < sphere.currentNumInstances; i++) 
		{
			if (glm::length(cam.cameraPos - sphere.instances[i]->pos) > 250.0f) 
			{
				scene.MarkForDeletion(sphere.instances[i]->instanceId);
			}
		}

		// render scene to dirlight FBO
		dirLight.shadowFBO.Activate();
		scene.RenderDirLightShader(dirShadowShader);
		RenderScene(dirShadowShader);

		// render scene to point light FBOs
		for (unsigned int i = 0, len = scene.pointLights.size(); i < len; i++) {
		    if (States::IsIndexActive(&scene.activePointLights, i)) {
		        scene.pointLights[i]->shadowFBO.Activate();
		        scene.RenderPointLightShader(pointShadowShader, i);
		        RenderScene(pointShadowShader);
		    }
		}

		//// render scene to spot light FBOs
		//for (unsigned int i = 0, len = scene.spotLights.size(); i < len; i++) {
		//    if (States::isIndexActive(&scene.activeSpotLights, i)) {
		//        scene.spotLights[i]->shadowFBO.activate();
		//        scene.renderSpotLightShader(spotShadowShader, i);
		//        renderScene(spotShadowShader);
		//    }
		//}

		// render scene normally
		scene.defaultFBO.Activate();
		scene.RenderShader(shader);
		RenderScene(shader);

		// render boxes
		scene.RenderShader(boxShader, false);
		box.render(boxShader);

		// send new frame to window
		scene.NewFrame(box);

		// clear instances that have been marked for deletion
		scene.ClearDeadInstances();
	}
	
	
	scene.Cleanup();
	*/

	SceneManager sm;

	sm.Start();
	sm.Update();
	sm.Cleanup();

	return 0;
}
