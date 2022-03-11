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
		specular = new glm::vec3(0.9f);
	}

	Light::~Light() 
	{
		delete color;
		delete ambient;
		delete diffuse;
		delete specular;
	}

	compType Light::getType() const
	{
		return compTypeFromTemplate<Light>{}.type;
	}

	DirectionalLight::DirectionalLight(glm::vec3 color, glm::vec3 d)
		:Light(lightType::directional, color)
	{
		direction = new glm::vec3(d);
	}

	DirectionalLight::~DirectionalLight()
	{
		delete direction;
	}

	light_ptr Light::createDirectional(glm::vec3 color, glm::vec3 direction)
	{
		return light_ptr(new DirectionalLight(color, direction));
	}

	PointLight::PointLight(glm::vec3 color, float q, float l, float c)
		:Light(lightType::point, color), quadratic(q), linear(l), constant(c) {}

	PointLight::~PointLight() {}

	light_ptr Light::createPoint(glm::vec3 color, float quadratic, float linear, float constant)
	{
		return light_ptr(new PointLight(color, quadratic, linear, constant));
	}

	SpotLight::SpotLight(glm::vec3 c, glm::vec3 d, float i, float o)
		:Light(lightType::spot, c), innerCutOff(i), outerCutOff(o)
	{
		direction = new glm::vec3(d);
	}

	SpotLight::~SpotLight()
	{
		delete direction;
	}

	light_ptr Light::createSpot(glm::vec3 color, glm::vec3 direction, float inner, float outer)
	{
		return light_ptr(new SpotLight(color, direction, inner, outer));
	}
}