#ifndef PBRSCENE_H
#define PBRSCENE_H

#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <jsoncpp/json.hpp>

#include "graphics/buffer/FrameBuffer.hpp"
#include "graphics/buffer/UniformBuffer.hpp"

#include "graphics/models/Box.hpp"

#include "graphics/Model.h"
#include "graphics/Light.h"
#include "graphics/Shader.h"
#include "graphics/Text.h"

#include "io/Camera.h"
#include "io/Keyboard.h"
#include "io/Mouse.h"

#include "algorithms/States.hpp"
#include "algorithms/tree/avl.h"
#include "algorithms/tree/Octree.h"
#include "algorithms/tree/Trie.hpp"

using namespace Octree;

// Forward declarations
class node;
class Model;

class PBRScene
{
public:
	avl*					models;					//Store models
	trie::Trie<RigidBody*>	instances;				//Store instances

	std::vector<RigidBody*> instancesToDelete;		// list of instances that should be deleted
	Octree::node*			octree;					// pointer to root node in octree

	jsoncpp::json			variableLog;			// map for logged variables

	FT_Library				ft;						//Freetype library
	avl* fonts;

	FramebufferObject		defaultFBO;
	UBO::UBO				lightUBO;

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	PBRScene();
	PBRScene(int glfwVersionMajor, int glfwVersionMinor, const char* title, unsigned int scrWidth, unsigned int scrHeight);

	bool Init();
	void SetParametres();
	bool RegisterFont(TextRenderer* tr, std::string name, std::string path);
	void OctreePrepare(Box& box);

	//============================================================================================//
	//PBR Setting
	//============================================================================================//

	void SetPBRLight(Shader& shader);
	void SetPBRCubemap();

	//============================================================================================//
	//Main Loop
	//============================================================================================//

	void Update();
	void NewFrame(Box& box);
	void ProcessInput(float dt);
	void RenderShader(Shader& shader, bool applyOctree = false);
	void RenderInstances(std::string modelId, Shader& shader, float dt);	// Render specified model's instances
	void RenderText(std::string font, Shader& shader, std::string text, float x, float y, glm::vec2 scale, glm::vec3 color);

	void SetShouldClose(bool shouldClose);
	void SetWindowColor(float r, float g, float b, float a);

	bool ShouldClose();
	void Cleanup();

	//============================================================================================//
	//Model/Instance
	//============================================================================================//

	std::string				currentId;				// current instance id
	std::string				GenerateId();			// generate next instance id

	void RegisterModel(Model* model);				// register model into model trie

	RigidBody* GenerateInstance(std::string modelId,
		glm::vec3 size = glm::vec3(1.0f),
		float mass = 1.0f,
		glm::vec3 pos = glm::vec3(0.0f),
		glm::vec3 rot = glm::vec3(0.0f));			// generate instance of specified model with physical parameters

	void InitInstances();							// initialize model instances
	void LoadModels();								// load model data
	void RemoveInstance(std::string instanceId);	// delete instance
	void ClearDeadInstances();						// clear all instances marked for deletion


	//============================================================================================//
	//Lights
	//============================================================================================//

	unsigned int			numPBRLights;
	std::vector<PBRLight*>	lights;

	//============================================================================================//
	//Camera Variables
	//============================================================================================//

	std::vector<Camera*>	cameras;				// list of cameras
	unsigned int			activeCamera;			// index of active camera
	glm::mat4				view;
	glm::mat4				projection;
	glm::mat4				textProjection;
	glm::vec3				cameraPos;

	// get current active camera in scene
	Camera* GetActiveCamera();

protected:

	//============================================================================================//
	//Window vals
	//============================================================================================//

	GLFWwindow* window;
	const char* title;
	static unsigned int		SCR_WIDTH;
	static unsigned int		SCR_HEIGHT;
	float					bgColor[4]; // background color

	int						glfwVersionMajor;
	int						glfwVersionMinor;
};

#endif
