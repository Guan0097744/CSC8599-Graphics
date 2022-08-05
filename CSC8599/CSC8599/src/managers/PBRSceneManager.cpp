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
	backgroundShader		= new Shader(false, "pbr/background_vs.glsl", "pbr/background_fs.glsl");
	equirectangularShader	= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/equirectangular_fs.glsl");
	prefilterShader			= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/prefilter_fs.glsl");
	irradianceShader		= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/irradiance_fs.glsl");
	brdfShader				= new Shader(false, "pbr/brdf_vs.glsl", "pbr/brdf_fs.glsl");

	Shader::ClearDefault();
}

void PBRSceneManager::SetLightings()
{
	glm::vec3 lightsPos[2] = {
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(10.0f, 10.0f, 10.0f),
		//glm::vec3(-10.0f, -10.0f, 10.0f),
		//glm::vec3(10.0f, -10.0f, 10.0f)
	};
	glm::vec3 lightColor[2] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		//glm::vec3(300.0f, 300.0f, 300.0f),
		//glm::vec3(300.0f, 300.0f, 300.0f)
	};

	PBRLight lights[2];
	for (int i = 0; i < 2; i++)
	{
		lights[i].position = lightsPos[i];
		lights[i].color = lightColor[i];
		scene.lights.push_back(lights[i]);
	}

	//scene.SetPBRLight(*pbrShader);
}

void PBRSceneManager::SetModels()
{
	pbrModel = new PBRModel("PBRModel");
	//pbrModel->Init("assets/models/pbr_kirby/scene.gltf");
	pbrModel->Init("assets/models/pbr_kirby2/Robobo_Kirby.obj");
	//pbrModel->Init("assets/models/pbr_benz_300sl/scene.gltf");
	//pbrModel->Init("assets/models/pbr_phone/scene.gltf");

	//AddModel(pbrModel, glm::vec3(0.1f), 1.0f, glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
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
