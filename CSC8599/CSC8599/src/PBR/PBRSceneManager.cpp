#include "PBRSceneManager.h"
#include "../physics/Environment.h"

PBRSceneManager::PBRSceneManager()
{
	
}

PBRSceneManager::~PBRSceneManager()
{
	delete font;

	//delete boxShader;
	delete pbrShader;
	delete equirectangularShader;
	delete prefilterShader;
	delete irradianceShader;
	delete brdfShader;
	delete backgroundShader;

	delete pbrModel;
}

void PBRSceneManager::Start()
{
	scene = PBRScene(4, 6, "OpenGL Tutorial", 1280, 720);

	// GLFW window creation
	if (!scene.Init())
	{
		std::cout << "Failed to create window" << std::endl;
		scene.Cleanup();
		return;
	}

	Setup();
}

void PBRSceneManager::Update()
{
	while (!scene.ShouldClose())
	{
		double currentTime	= glfwGetTime();
		dt					= currentTime - lastFrame;
		lastFrame			= currentTime;

		scene.variableLog["time"] += dt;
		scene.variableLog["fps"] = 1 / dt;

		//============================================================================================//
		//Input
		//============================================================================================//

		ProcessInput(dt);

		//============================================================================================//
		//Render
		//============================================================================================//
		
		scene.Update();

		// Render PBR scene
		scene.defaultFBO.Activate();
		scene.RenderShader(*pbrShader);
		RenderScene(*pbrShader);
		
		/*// render boxes
		scene.RenderShader(*boxShader, true);
		box.Render(*boxShader);*/

		// send new frame to window
		scene.NewFrame(box);

		// clear instances that have been marked for deletion
		scene.ClearDeadInstances();
	}
}

void PBRSceneManager::Cleanup()
{
	scene.Cleanup();
}

void PBRSceneManager::Setup()
{
	SetCamera();
	SetFonts();

	SetShaders();
	SetLightings();
	SetModels();

	scene.InitInstances();

	box.Init();
	scene.OctreePrepare(box);
	scene.variableLog["time"] = (double)0.0;

	// Bind default framebuffer
	scene.defaultFBO.Bind();

	scene.SetIBLFrameBuffer();
	//SetMap();
}

void PBRSceneManager::SetCamera()
{
	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;
}

void PBRSceneManager::SetFonts()
{
	font = new TextRenderer(32);
	if (!scene.RegisterFont(font, "comic", "assets/fonts/comic.ttf"))
	{
		std::cout << "Could not load font" << std::endl;
	}
}

void PBRSceneManager::SetShaders()
{
	//boxShader				= new Shader(false, "instanced/box_vs.glsl", "instanced/box_fs.glsl");

	pbrShader				= new Shader(false, "pbr/pbr_vs.glsl", "pbr/pbr_fs.glsl");
	equirectangularShader	= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/equirectangular_fs.glsl");
	prefilterShader			= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/prefilter_fs.glsl");
	irradianceShader		= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/irradiance_fs.glsl");
	brdfShader				= new Shader(false, "pbr/brdf_vs.glsl", "pbr/brdf_fs.glsl");
	backgroundShader		= new Shader(false, "pbr/background_vs.glsl", "pbr/background_fs.glsl");

	Shader::ClearDefault();
}

void PBRSceneManager::SetLightings()
{
	glm::vec3 lightsPos[4] = {
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(10.0f, 10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f)
	};
	glm::vec3 lightColor[4] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	PBRLight lights[4];
	for (int i = 0; i < 4; i++)
	{
		lights[i].position = lightsPos[i];
		lights[i].color = lightColor[i];
		scene.lights.push_back(&lights[i]);
	}

	scene.SetPBRLight(*pbrShader);
}

void PBRSceneManager::SetMap()
{
	// environment map
	PBRCubemap hdrMap;
	hdrMap.Generate();
	hdrMap.LoadMap("assets/skybox/pink_sunrise_2k.hdr");

	PBRCubemap envMap;
	envMap.Generate();
	envMap.Bind();
	envMap.Allocate(GL_RGB16F, GL_RGB, 512, 512, GL_FLOAT);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	equirectangularShader->Use();
	equirectangularShader->SetInt("equirectangularMap", 0);
	equirectangularShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);

	hdrMap.Bind();

	glViewport(0, 0, 512, 512);
	scene.captureFBO.Bind();

	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularShader->SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMap.id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, envMap.id);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void PBRSceneManager::SetModels()
{
	pbrModel = new PBRModel("PBRModel");
	//pbrModel->Init("assets/models/pbr_kirby/scene.gltf");
	pbrModel->Init("assets/models/pbr_kirby2/Robobo_Kirby.obj");
	AddModel(pbrModel, glm::vec3(0.1f), 1.0f, glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	scene.LoadModels();
}

void PBRSceneManager::AddModel(Model* m, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
{
	scene.RegisterModel(m);
	scene.GenerateInstance(m->id, size, mass, pos, rot);
}

void PBRSceneManager::RenderScene(Shader& shader)
{
	scene.RenderInstances(pbrModel->id, shader, dt);
}

void PBRSceneManager::ProcessInput(double dt)
{
	// process input with cameras
	scene.ProcessInput(dt);

	// close window
	if (Keyboard::Key(GLFW_KEY_ESCAPE))
	{
		scene.SetShouldClose(true);
	}
}
