#ifndef LAMP_HPP
#define LAMP_HPP

#include <glm/glm.hpp>

#include "Cube.hpp"
#include "../Material.h"

class Lamp :public Cube
{
public:
    glm::vec3 lightColor;

    Lamp(unsigned int maxNumInstances = 1, glm::vec3 lightColor = glm::vec3(1.0f))
        : Cube(maxNumInstances, Material::white_rubber) 
    {
        id = "lamp";
        this->lightColor = lightColor;
    }
};

#endif // !LAMP_HPP
