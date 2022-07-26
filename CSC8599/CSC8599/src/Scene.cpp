#include "Scene.h"

#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 2

unsigned int Scene::SCR_WIDTH = 1000;
unsigned int Scene::SCR_HEIGHT = 1000;

/**
 * @brief GLFW: whenever the window size changed (by OS or user resize) this callback function executes
 * @param window 
 * @param scrWidth 
 * @param scrHeight 
*/
void Scene::framebuffer_size_callback(GLFWwindow* window, int scrWidth, int scrHeight)
{
	// Make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, scrWidth, scrHeight);
	SCR_WIDTH	= scrWidth;
	SCR_HEIGHT	= scrHeight;
}

Scene::Scene()
	: currentId("aaaaaaaa"), lightUBO(0) {}

Scene::Scene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight)
	: glfwVersionMajor(glfwVersionMajor), glfwVersionMinor(glfwVersionMinor), // GLFW version
	title(title), // window title
	// default indices/vals
	activeCamera(-1),
	activePointLights(0), activeSpotLights(0),
	currentId("aaaaaaaa"), lightUBO(0) 
{
	SCR_WIDTH	= scrWidth;
	SCR_HEIGHT	= scrHeight;
	defaultFBO	= FramebufferObject(scrWidth, scrHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	SetWindowColor(0.1f, 0.15f, 0.15f, 1.0f);
}

bool Scene::Init()
{
	glfwInit();

	// Set OpenGL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glfwVersionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glfwVersionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

	// GLFW window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
	if (window == NULL)
	{
		return false;
	}
	glfwMakeContextCurrent(window);

	// Set GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return false;
	}

	SetParametres();

	//initialize freetype library
	if (FT_Init_FreeType(&ft)) 
	{
		std::cout << "Could not init FreeType library" << std::endl;
		return false;
	}
	fonts = avl_createEmptyRoot(strkeycmp);

	return true;
}

void Scene::SetParametres()
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);									// Setup screen

	//============================================================================================//
	//GLFW Callbacks
	//============================================================================================//

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);			// Frame size
	glfwSetKeyCallback(window, Keyboard::KeyCallback);							// Key pressed
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);				// Set cursor
	glfwSetCursorPosCallback(window, Mouse::CursorPosCallback);					// cursor moved
	glfwSetScrollCallback(window, Mouse::MouseWheelCallback);					// Mouse scroll
	glfwSetMouseButtonCallback(window, Mouse::MouseButtonCallback);				// Mouse btn pressed
	glfwSwapInterval(1);														// v-sync

	//============================================================================================//
	//Rendering Setup
	//============================================================================================//
	
	glEnable(GL_DEPTH_TEST);													// Depth testing: doesn't show vertices not visible to camera (back of object)

	glEnable(GL_BLEND);															// Blending for text textures
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);													// Stencil testing
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);									// Keep fragments if either stencil or depth fails, replace if both pass

	//============================================================================================//
	//Init model/instance trees
	//============================================================================================//

	models		= avl_createEmptyRoot(strkeycmp);
	instances	= trie::Trie<RigidBody*>(trie::ascii_lowercase);
	octree		= new Octree::node(BoundingRegion(glm::vec3(-16.0f), glm::vec3(16.0f)));

}

bool Scene::RegisterFont(TextRenderer* tr, std::string name, std::string path)
{
	if (tr->LoadFont(ft, path)) 
	{
		fonts = avl_insert(fonts, (void*)name.c_str(), tr);
		return true;
	}
	else 
	{
		return false;
	}
}

/**
 * @brief To be called after instances have been generated/registered
 * @param box 
 * @param shaders 
*/
void Scene::OctreePrepare(Box& box, std::vector<Shader> shaders)
{
	// close FT library
	FT_Done_FreeType(ft);
	// process current instances
	octree->Update(box);
}

void Scene::SetLightsUBO(std::vector<Shader> shaders)
{
	// setup lighting UBO
	lightUBO = UBO::UBO(0, {
		UBO::NewStruct({ // dir light
			UBO::Type::VEC3,

			UBO::Type::VEC4,
			UBO::Type::VEC4,
			UBO::Type::VEC4,

			UBO::Type::SCALAR,

			UBO::NewColMat(4, 4)
		}),

		UBO::Type::SCALAR, // no point lights
		UBO::NewArray(MAX_POINT_LIGHTS, UBO::NewStruct({
			UBO::Type::VEC3,

			UBO::Type::VEC4,
			UBO::Type::VEC4,
			UBO::Type::VEC4,

			UBO::Type::SCALAR,
			UBO::Type::SCALAR,
			UBO::Type::SCALAR,

			UBO::Type::SCALAR
		})),

		UBO::Type::SCALAR, // no spot lights
		UBO::NewArray(MAX_SPOT_LIGHTS, UBO::NewStruct({
			UBO::Type::VEC3,
			UBO::Type::VEC3,

			UBO::Type::SCALAR,
			UBO::Type::SCALAR,

			UBO::Type::VEC4,
			UBO::Type::VEC4,
			UBO::Type::VEC4,

			UBO::Type::SCALAR,
			UBO::Type::SCALAR,
			UBO::Type::SCALAR,

			UBO::Type::SCALAR,
			UBO::Type::SCALAR,

			UBO::NewColMat(4, 4)
		}))
		});

	// attach the UBO to specified shaders
	for (Shader s : shaders)
	{
		lightUBO.AttachToShader(s, "Lights");
	}

	// setup memory
	lightUBO.Generate();
	lightUBO.Bind();
	lightUBO.InitNullData(GL_STATIC_DRAW);
	lightUBO.BindRange();

	// write initial values
	lightUBO.StartWrite();

	// directional light
	lightUBO.WriteElement<glm::vec3>(&dirLight->direction);
	lightUBO.WriteElement<glm::vec4>(&dirLight->ambient);
	lightUBO.WriteElement<glm::vec4>(&dirLight->diffuse);
	lightUBO.WriteElement<glm::vec4>(&dirLight->specular);
	lightUBO.WriteElement<float>(&dirLight->br.max.z);
	lightUBO.WriteArrayContainer<glm::mat4, glm::vec4>(&dirLight->lightSpaceMatrix, 4);

	// point lights
	numPointLights = std::min<unsigned int>(pointLights.size(), MAX_POINT_LIGHTS);
	lightUBO.WriteElement<unsigned int>(&numPointLights);
	unsigned int i = 0;
	for (; i < numPointLights; i++)
	{
		lightUBO.WriteElement<glm::vec3>(&pointLights[i]->position);
		lightUBO.WriteElement<glm::vec4>(&pointLights[i]->ambient);
		lightUBO.WriteElement<glm::vec4>(&pointLights[i]->diffuse);
		lightUBO.WriteElement<glm::vec4>(&pointLights[i]->specular);
		lightUBO.WriteElement<float>(&pointLights[i]->k0);
		lightUBO.WriteElement<float>(&pointLights[i]->k1);
		lightUBO.WriteElement<float>(&pointLights[i]->k2);
		lightUBO.WriteElement<float>(&pointLights[i]->farPlane);
	}
	lightUBO.AdvanceArray(MAX_POINT_LIGHTS - i); // advance to finish array

	// spot lights
	numSpotLights = std::min<unsigned int>(spotLights.size(), MAX_SPOT_LIGHTS);
	lightUBO.WriteElement<unsigned int>(&numSpotLights);
	for (i = 0; i < numSpotLights; i++) {
		lightUBO.WriteElement<glm::vec3>(&spotLights[i]->position);
		lightUBO.WriteElement<glm::vec3>(&spotLights[i]->direction);
		lightUBO.WriteElement<float>(&spotLights[i]->cutOff);
		lightUBO.WriteElement<float>(&spotLights[i]->outerCutOff);
		lightUBO.WriteElement<glm::vec4>(&spotLights[i]->ambient);
		lightUBO.WriteElement<glm::vec4>(&spotLights[i]->diffuse);
		lightUBO.WriteElement<glm::vec4>(&spotLights[i]->specular);
		lightUBO.WriteElement<float>(&spotLights[i]->k0);
		lightUBO.WriteElement<float>(&spotLights[i]->k1);
		lightUBO.WriteElement<float>(&spotLights[i]->k2);
		lightUBO.WriteElement<float>(&spotLights[i]->nearPlane);
		lightUBO.WriteElement<float>(&spotLights[i]->farPlane);
		lightUBO.WriteArrayContainer<glm::mat4, glm::vec4>(&spotLights[i]->lightSpaceMatrix, 4);
	}

	lightUBO.Clear();
}

void Scene::Update()
{
	/*glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

	// set background color
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[4]);
	// clear occupied bits
	defaultFBO.Clear();
}

void Scene::NewFrame(Box& box)
{
	box.positions.clear();
	box.sizes.clear();

	// process pending objects
	octree->ProcessPending();
	octree->Update(box);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Scene::ProcessInput(float dt)
{
	if (activeCamera != -1 && activeCamera < cameras.size()) 
	{
		// active camera exists

		// set camera direction
		double dx = Mouse::GetDX(), dy = Mouse::GetDY();
		if (dx != 0 || dy != 0) {
			cameras[activeCamera]->UpdateCameraDirection(dx, dy);
		}

		// set camera zoom
		double scrollDy = Mouse::GetScrollDY();
		if (scrollDy != 0) {
			cameras[activeCamera]->UpdateCameraZoom(scrollDy);
		}

		// set camera pos
		if (Keyboard::Key(GLFW_KEY_W)) 
		{
			cameras[activeCamera]->UpdateCameraPos(CameraDirection::FORWARD, dt);
		}
		if (Keyboard::Key(GLFW_KEY_S)) 
		{
			cameras[activeCamera]->UpdateCameraPos(CameraDirection::BACKWARD, dt);
		}
		if (Keyboard::Key(GLFW_KEY_D)) 
		{
			cameras[activeCamera]->UpdateCameraPos(CameraDirection::RIGHT, dt);
		}
		if (Keyboard::Key(GLFW_KEY_A)) 
		{
			cameras[activeCamera]->UpdateCameraPos(CameraDirection::LEFT, dt);
		}
		if (Keyboard::Key(GLFW_KEY_SPACE)) 
		{
			cameras[activeCamera]->UpdateCameraPos(CameraDirection::UP, dt);
		}
		if (Keyboard::Key(GLFW_KEY_LEFT_SHIFT)) 
		{
			cameras[activeCamera]->UpdateCameraPos(CameraDirection::DOWN, dt);
		}

		// set matrices
		view = cameras[activeCamera]->GetViewMatrix();
		projection = glm::perspective(
			glm::radians(cameras[activeCamera]->GetZoom()),	    // FOV
			(float)SCR_WIDTH / (float)SCR_HEIGHT,				// aspect ratio
			0.1f, 100.0f										// near and far bounds
		);
		textProjection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);

		// set pos
		cameraPos = cameras[activeCamera]->cameraPos;
	}
}

void Scene::RenderShader(Shader& shader, bool applyLighting)
{
	// activate shader
	shader.Activate();

	// set camera values
	shader.SetMat4("view", view);
	shader.SetMat4("projection", projection);
	shader.Set3Float("viewPos", cameraPos);

	// lighting
	if (applyLighting) 
	{
		unsigned int textureIdx = 31;

		// directional light
		dirLight->Render(shader, textureIdx--);

		// point lights
		unsigned int numLights = pointLights.size();
		unsigned int numActiveLights = 0;
		for (unsigned int i = 0; i < numLights; i++) 
		{
			if (States::IsIndexActive(&activePointLights, i)) 
			{
				// i'th light is active
				pointLights[i]->Render(shader, numActiveLights, textureIdx--);
				numActiveLights++;
			}
		}
		shader.SetInt("noPointLights", numActiveLights);

		// spot lights
		numLights = spotLights.size();
		numActiveLights = 0;
		for (unsigned int i = 0; i < numLights; i++) {
			if (States::IsIndexActive(&activeSpotLights, i)) {
				// i'th spot light active
				spotLights[i]->Render(shader, numActiveLights, textureIdx--);
				numActiveLights++;
			}
		}
		shader.SetInt("noSpotLights", numActiveLights);
	}
}

void Scene::RenderDirLightShader(Shader& shader)
{
	shader.Activate();
	shader.SetMat4("lightSpaceMatrix", dirLight->lightSpaceMatrix);
}

void Scene::RenderPointLightShader(Shader& shader, unsigned int idx)
{
	shader.Activate();

	// light space matrices
	for (unsigned int i = 0; i < 6; i++) 
	{
		shader.SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", pointLights[idx]->lightSpaceMatrices[i]);
	}

	shader.Set3Float("lightPos", pointLights[idx]->position);				// light position
	shader.SetFloat("farPlane", pointLights[idx]->farPlane);				// far plane
}

void Scene::RenderSpotLightShader(Shader& shader, unsigned int idx)
{
	shader.Activate();

	shader.SetMat4("lightSpaceMatrix", spotLights[idx]->lightSpaceMatrix);	// light space matrix
	shader.Set3Float("lightPos", spotLights[idx]->position);				// light position
	shader.SetFloat("farPlane", spotLights[idx]->farPlane);					// far plane
}

void Scene::RenderInstances(std::string modelId, Shader& shader, float dt)
{
	void* val = avl_get(models, (void*)modelId.c_str());
	if (val) 
	{
		// render each mesh in specified model
		shader.Activate();
		((Model*)val)->Render(shader, dt);
	}
}

void Scene::RenderText(std::string font, Shader& shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color)
{
	void* val = avl_get(fonts, (void*)font.c_str());
	if (val) 
	{
		shader.Activate();
		shader.SetMat4("projection", textProjection);

		((TextRenderer*)val)->Render(shader, text, x, y, scale, color);
	}
}

void Scene::SetShouldClose(bool shouldClose)
{
	glfwSetWindowShouldClose(window, shouldClose);
}

void Scene::SetWindowColor(float r, float g, float b, float a)
{
	// set rgba vals of the background color
	bgColor[0] = r;
	bgColor[1] = g;
	bgColor[2] = b;
	bgColor[3] = a;
}

bool Scene::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void Scene::Cleanup()
{
	// clean up instances
	instances.Cleanup();

	// clean all models
	avl_postorderTraverse(models, [](avl* node) -> void {
		((Model*)node->val)->Cleanup();
		});
	avl_free(models);

	// cleanup fonts
	avl_postorderTraverse(fonts, [](avl* node) -> void {
		((TextRenderer*)node->val)->Cleanup();
		});
	avl_free(fonts);

	// destroy octree
	octree->Destroy();

	// terminate glfw
	glfwTerminate();
}

std::string Scene::GenerateId()
{
	for (int i = currentId.length() - 1; i >= 0; i--) 
	{
		if ((int)currentId[i] != (int)'z') 
		{
			// increment then break
			currentId[i] = (char)(((int)currentId[i]) + 1);
			break;
		}
		else 
		{
			currentId[i] = 'a';
		}
	}
	return currentId;
}

void Scene::RegisterModel(Model* model)
{
	models = avl_insert(models, (void*)model->id.c_str(), model);
}

RigidBody* Scene::GenerateInstance(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
{
	// generate new rigid body
	void* val = avl_get(models, (void*)modelId.c_str());
	if (val) 
	{
		Model* model = (Model*)val;
		RigidBody* rb = model->GenerateInstance(size, mass, pos, rot);
		if (rb) 
		{
			// successfully generated, set new and unique id for instance
			std::string id = GenerateId();
			rb->instanceId = id;
			// insert into trie
			instances.Insert(rb->instanceId, rb);
			// insert into pending queue
			octree->AddToPending(rb, model);
			return rb;
		}
	}
	return nullptr;
}

/**
 * @brief initialize model instances
*/
void Scene::InitInstances()
{
	// initialize all instances for each model
	avl_inorderTraverse(models, [](avl* node) -> void {
		((Model*)node->val)->InitInstances();
		});
}

/**
 * @brief load model data
*/
void Scene::LoadModels()
{
	// initialize each model
	avl_inorderTraverse(models, [](avl* node) -> void {
		((Model*)node->val)->Init();
		});
}

void Scene::RemoveInstance(std::string instanceId)
{
	RigidBody* instance			= instances[instanceId];
	// get instance's model
	std::string targetModel		= instance->modelId;
	Model* model				= (Model*)avl_get(models, (void*)targetModel.c_str());

	// delete instance from model
	model->RemoveInstance(instanceId);

	// remove from tree
	instances[instanceId]		= NULL;
	instances.Erase(instanceId);
	free(instance);
}

/**
 * @brief mark instance for deletion
 * @param instanceId 
*/
void Scene::MarkForDeletion(std::string instanceId)
{
	RigidBody* instance			= instances[instanceId];

	// activate kill switch
	States::Activate(&instance->state, INSTANCE_DEAD);
	// push to list
	instancesToDelete.push_back(instance);
}

void Scene::ClearDeadInstances()
{
	for (RigidBody* rb : instancesToDelete) 
	{
		RemoveInstance(rb->instanceId);
	}
	instancesToDelete.clear();
}

Camera* Scene::GetActiveCamera()
{
	// if activate camera exists, return the one at that index
	return (activeCamera >= 0 && activeCamera < cameras.size()) ? cameras[activeCamera] : nullptr;
}

