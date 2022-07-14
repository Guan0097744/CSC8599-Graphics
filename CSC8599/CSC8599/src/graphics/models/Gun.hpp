#include "../Model.h"
#include "../../io/Camera.h"
#include "../../io/Keyboard.h"

#include "../../Scene.h"
#include "../../physics/Environment.h"

class Gun :public Model
{
public:
	Gun(unsigned int maxNoInstances)
		: Model("m4a1", maxNoInstances, CONST_INSTANCES | NO_TEX) {}

	void Init() {
		LoadModel("assets/models/m4a1/scene.gltf");
	}
};