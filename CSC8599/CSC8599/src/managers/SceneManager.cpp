#include "SceneManager.h"
#include "../physics/Environment.h"

SceneManager::SceneManager()
{
	
}

SceneManager::~SceneManager()
{
	delete font;

	delete shader;
	delete boxShader;
	delete pbrShader;
	delete equirectangularShader;
	delete prefilterShader;
	delete irradianceShader;
	delete brdfShader;
	delete backgroundShader;

	delete dirLight;
	delete pointLight;
	delete spotLight;

	delete sphere;
	delete lamp;
	delete pbrModel;
}

void SceneManager::Start()
{
	scene = Scene(3, 3, "OpenGL Tutorial", 1280, 720);
	//scene = PBRScene(3, 3, "OpenGL Tutorial", 1280, 720);

	// GLFW window creation
	if (!scene.Init())
	{
		std::cout << "Failed to create window" << std::endl;
		scene.Cleanup();
		return;
	}

	Setup();
}

void SceneManager::Update()
{
	while (!scene.ShouldClose())
	{
		double currentTime = glfwGetTime();
		dt = currentTime - lastFrame;
		lastFrame = currentTime;

		scene.variableLog["time"] += dt;
		scene.variableLog["fps"] = 1 / dt;

		ProcessInput(dt);

		// Render
		scene.Update();

		//// render scene normally
		//scene.defaultFBO.Activate();
		//scene.RenderShader(*shader);
		//RenderScene(*shader);

		// Render PBR scene
		scene.defaultFBO.Activate();
		scene.RenderShader(*pbrShader);
		RenderScene(*pbrShader);

		// render boxes
		scene.RenderShader(*boxShader, false);
		box.Render(*boxShader);

		// send new frame to window
		scene.NewFrame(box);

		// clear instances that have been marked for deletion
		scene.ClearDeadInstances();
	}
}

void SceneManager::Cleanup()
{
	scene.Cleanup();
}

void SceneManager::Setup()
{
	SetCamera();
	SetFonts();

	SetShaders();
	SetLightings();
	SetModels();

	box.Init();

	scene.InitInstances();

	//// finish preparations (octree, etc)
	//scene.OctreePrepare(box, { *shader });
	//scene.OctreePrepare(box, { *pbrShader });


	scene.variableLog["time"] = (double)0.0;

	scene.defaultFBO.Bind(); // bind default framebuffer
}

void SceneManager::SetCamera()
{
	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;
}

void SceneManager::SetFonts()
{
	font = new TextRenderer(32);
	if (!scene.RegisterFont(font, "comic", "assets/fonts/comic.ttf"))
	{
		std::cout << "Could not load font" << std::endl;
	}
}

void SceneManager::SetShaders()
{
	//Shader::LoadIntoDefault("defaultHead.glsl");

	//shader				= new Shader(true, "instanced/instanced_vs.glsl", "object_fs.glsl");
	pbrShader			= new Shader(false, "pbr/pbr_vs.glsl", "pbr/pbr_fs.glsl");
	boxShader			= new Shader(false, "instanced/box_vs.glsl", "instanced/box_fs.glsl");
	equirectangularShader = new Shader(false, "pbr/cubemap_vs.glsl", "pbr/equirectangular_fs.glsl");
	prefilterShader		= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/prefilter_fs.glsl");
	irradianceShader	= new Shader(false, "pbr/cubemap_vs.glsl", "pbr/irradiance_fs.glsl");
	brdfShader			= new Shader(false, "pbr/brdf_vs.glsl", "pbr/brdf_fs.glsl");
	backgroundShader	= new Shader(false, "pbr/background_vs.glsl", "pbr/background_fs.glsl");

	Shader::ClearDefault();
}

void SceneManager::SetLightings()
{
	//============================================================================================//
	// Directional light
	//============================================================================================//
	
	dirLight = new DirLight(glm::vec3(-0.2f, -0.9f, -0.2f),
		glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
		glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
		glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
		BoundingRegion(glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 50.0f)));
	scene.dirLight = dirLight;
	
	//============================================================================================//
	// Point lights
	//============================================================================================//

	glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	glm::vec4 specular = glm::vec4(1.0f);
	float k0 = 1.0f;
	float k1 = 0.0014f;
	float k2 = 0.000007f;

	pointLight = new PointLight(
		glm::vec3(0.0f, 5.0f, 0.0f),
		k0, k1, k2,
		ambient, diffuse, specular,
		0.5f, 50.0f
	);

	// create physical model for each lamp
	//scene.GenerateInstance(lamp->id, glm::vec3(1.0f), 0.25f, glm::vec3(0.0f, 15.0f, 0.0f));
	// add lamp to scene's light source
	scene.pointLights.push_back(pointLight);
	// activate lamp in scene
	States::ActivateIndex(&scene.activePointLights, 0);
	
	/*
	//============================================================================================//
	// Spot light
	//============================================================================================//
	
	spotLight = new SpotLight(
		//glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
		cam.cameraPos, cam.cameraFront, cam.cameraUp,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
		1.0f, 0.0014f, 0.000007f,
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(1.0f), glm::vec4(1.0f),
		0.1f, 100.0f
	);
	scene.spotLights.push_back(spotLight);
	States::ActivateIndex(&scene.activeSpotLights, 0);
	//scene.activeSpotLights = 1; // 0b00000001
	*/
	
}

void SceneManager::SetModels()
{
	sphere		= new Sphere(10);
	scene.RegisterModel(sphere);
	//AddModel(sphere, glm::vec3(1.0f), 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	pbrModel	= new PBRModel("Kirby");
	pbrModel->Init("assets/models/pbr_kirby_2/scene.gltf");
	AddModel(pbrModel, glm::vec3(0.1f), 1.0f, glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	scene.LoadModels();
}

void SceneManager::AddModel(Model* m, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
{
	scene.RegisterModel(m);
	scene.GenerateInstance(m->id, size, mass, pos, rot);
}

void SceneManager::RenderScene(Shader& shader)
{
	//scene.RenderInstances(sphere->id, shader, dt);
	if (sphere->currentNumInstances > 0) 
	{
		scene.RenderInstances(sphere->id, shader, dt);
	}

	scene.RenderInstances(pbrModel->id, shader, dt);
	//scene.RenderInstances(lamp->id, shader, dt);
}

void SceneManager::ProcessInput(double dt)
{
	// process input with cameras
	scene.ProcessInput(dt);

	// close window
	if (Keyboard::Key(GLFW_KEY_ESCAPE))
	{
		scene.SetShouldClose(true);
	}

	// update flash light
	if (States::IsIndexActive(&scene.activeSpotLights, 0))
	{
		scene.spotLights[0]->position = scene.GetActiveCamera()->cameraPos;
		scene.spotLights[0]->direction = scene.GetActiveCamera()->cameraFront;
		scene.spotLights[0]->up = scene.GetActiveCamera()->cameraUp;
		scene.spotLights[0]->UpdateMatrices();
	}

	if (Keyboard::KeyWentDown(GLFW_KEY_L))
	{
		States::ToggleIndex(&scene.activeSpotLights, 0); // toggle spot light
	}
}
