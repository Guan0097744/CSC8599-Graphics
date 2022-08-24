#include "PBRSceneManager.h"
#include "../physics/Environment.h"

PBRSceneManager::PBRSceneManager()
{
	
}

PBRSceneManager::~PBRSceneManager()
{
	delete font;

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
	scene = PBRScene(4, 6, "PBR Scene", 1280, 720);

	// GLFW window creation
	if (!scene.Init())
	{
		std::cout << "Failed to create window" << std::endl;
		scene.Cleanup();
		return;
	}

	Setup();
}

void PBRSceneManager::Cleanup()
{
	scene.Cleanup();
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
		scene.RenderShader(pbrShader);

		// Bind pre-computed IBL data
		glActiveTexture(GL_TEXTURE6);
		irrMap.Bind();
		glActiveTexture(GL_TEXTURE7);
 		preMap.Bind();
		glActiveTexture(GL_TEXTURE8);
		brdfTexture->Bind();

		RenderInstances(*pbrShader);

		scene.RenderShader(backgroundShader);
		glActiveTexture(GL_TEXTURE0);
		envMap.Bind();
		irrMap.Bind();
		//preMap.Bind();
		envMap.RenderCube();

		// send new frame to window
		scene.NewFrame();

		// clear instances that have been marked for deletion
		scene.ClearDeadInstances();
	}
}

void PBRSceneManager::Setup()
{
	SetCamera();
	SetFonts();

	SetShaders();
	SetLightings();
	SetModels();

	scene.InitInstances();

	scene.variableLog["time"] = (double)0.0;

	scene.SetIBLFrameBuffer();
	SetMap();

	glViewport(0, 0, scene.SCR_WIDTH, scene.SCR_HEIGHT);
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
	pbrShader				= new Shader(false, "pbr/pbr_vs.glsl", "pbr/pbr_fs.glsl");
	equirectangularShader	= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/equirectangular_fs.glsl");
	irradianceShader		= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/irradiance_fs.glsl");
	prefilterShader			= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/prefilter_fs.glsl");
	brdfShader				= new Shader(false, "pbr/brdf_vs.glsl", "pbr/brdf_fs.glsl");
	backgroundShader		= new Shader(false, "pbr/background_vs.glsl", "pbr/background_fs.glsl");

	pbrShader->Use();
	pbrShader->SetInt("albedoMap", 0);
	pbrShader->SetInt("normalMap", 1);
	pbrShader->SetInt("metallicMap", 2);
	pbrShader->SetInt("roughnessMap", 3);
	pbrShader->SetInt("aoMap", 4);

	pbrShader->SetInt("irradianceMap", 6);
	pbrShader->SetInt("prefilterMap", 7);
	pbrShader->SetInt("brdfLUT", 8);

	backgroundShader->Use();
	backgroundShader->SetInt("environmentMap", 0);

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
	Texture hdrMap;
	hdrMap.LoadHDR("assets/skybox/kiara_1_dawn_4k.hdr");
	//hdrMap.LoadHDR("assets/skybox/newport_loft.hdr");

	//============================================================================================//
	//Set up projection and view matrices for capturing data onto the 6 cubemap face directions
	//============================================================================================//

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

	//============================================================================================//
	//Environment map
	//============================================================================================//

	envMap.Generate();
	envMap.Bind();
	envMap.Allocate(GL_RGB16F, GL_RGB, 512, 512, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);

	//Convert HDR equirectangular environment map to cubemap equivalent
	equirectangularShader->Use();
	equirectangularShader->SetInt("equirectangularMap", 0);
	equirectangularShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	hdrMap.Bind();

	//Attach to framebuffer
	glViewport(0, 0, 512, 512);	//Configure the viewport to the capture dimensions
	scene.captureFBO.Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularShader->SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envMap.id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		envMap.RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	//Then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	envMap.Bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	

	//============================================================================================//
	//Irradiance map
	//============================================================================================//

	irrMap.Generate();
	irrMap.Bind();
	irrMap.Allocate(GL_RGB16F, GL_RGB, 32, 32, GL_FLOAT);

	scene.captureFBO.Bind();
	scene.captureFBO.AttachRBO(GL_DEPTH_COMPONENT24, scene.captureFBO.rbos[0], 32, 32);

	//Solve diffuse integral by convolution to create an irradiance (cube)map.
	irradianceShader->Use();
	irradianceShader->SetInt("environmentMap", 0);
	irradianceShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	envMap.Bind();

	//Attach to framebuffer
	glViewport(0, 0, 32, 32);	//Configure the viewport to the capture dimensions
	scene.captureFBO.Bind();
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader->SetMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irrMap.id, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		irrMap.RenderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//============================================================================================//
	//Prefilter map
	//============================================================================================//

	preMap.Generate();
	preMap.Bind();
	preMap.Allocate(GL_RGB16F, GL_RGB, 128, 128, GL_FLOAT, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	prefilterShader->Use();
	prefilterShader->SetInt("environmentMap", 0);
	prefilterShader->SetMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	envMap.Bind();

	scene.captureFBO.Bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
		unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));

		scene.captureFBO.AttachRBO(GL_DEPTH_COMPONENT24, scene.captureFBO.rbos[0], mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader->SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader->SetMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, preMap.id, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			preMap.RenderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//============================================================================================//
	//BRDF
	//============================================================================================//

	brdfTexture = new Texture();
	//brdfTexture->Generate();
	brdfTexture->Bind();
	brdfTexture->Allocate(GL_RG16F, GL_RG, 512, 512, GL_FLOAT);
	brdfTexture->SetParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	scene.captureFBO.Bind();
	scene.captureFBO.AttachRBO(GL_DEPTH_COMPONENT24, scene.captureFBO.rbos[0], 512, 512);
	scene.captureFBO.AttachTexture(GL_COLOR_ATTACHMENT0, *brdfTexture);

	glViewport(0, 0, 512, 512);
	brdfShader->Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	plane.Render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void PBRSceneManager::RenderInstances(Shader& shader)
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
