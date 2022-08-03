#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.h"

class Sphere :public Model
{
public:
	Sphere(unsigned int maxNumInstances = 1)
		: Model("sphere", maxNumInstances, NO_TEX | DYNAMIC) {}

	void Init() 
	{
		LoadModel("assets/models/sphere/scene.gltf");
	}

    /*void AddPBRTextures()
    {
        std::vector<Texture> textures =
        {
            Texture("assets/textures", "brickwall_diffuse.jpg", aiTextureType_DIFFUSE),
            Texture("assets/textures", "brickwall_normal.jpg", aiTextureType_NORMALS),
            Texture("assets/textures", "brickwall_specular.jpg", aiTextureType_SPECULAR)
        };

        for (Texture t : textures)
        {
            t.Load();
        }

        Mesh ret(textures);
        AddMesh(&ret);
    }*/
};

#endif // !SPHERE_HPP
