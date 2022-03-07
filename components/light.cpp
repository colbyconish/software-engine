#include "pch.h"
#include <SWE/Components/light.h>

namespace swe
{
	Light::Light(lightType lt, glm::vec3 c)
		: type(lt)
	{
		color = new glm::vec3(c);
		ambient = new glm::vec3(0.1f);
		diffuse = new glm::vec3(0.5f);
		specular = new glm::vec3(0.5f);
	}

	Light::~Light() 
	{
		delete color;
		delete ambient;
		delete diffuse;
		delete specular;
	}

	DirectionalLight::DirectionalLight(glm::vec3 c, glm::vec3 d)
		:Light(lightType::directional, c)
	{
		direction = new glm::vec3(d);
	}

	d_light DirectionalLight::getStruct()
	{
		return {
		*direction,
		*ambient,
		*diffuse,
		*specular };
	}

	DirectionalLight::~DirectionalLight()
	{
		delete direction;
	}

	light_ptr Light::createDirectional(glm::vec3 color, glm::vec3 direction)
	{
		return light_ptr(new DirectionalLight(color, direction));
	}

	compType Light::getType() const
	{
		return compTypeFromTemplate<Light>{}.type;
	}
}