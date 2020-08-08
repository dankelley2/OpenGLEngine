#pragma once
class Material
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float opacity;
	float refracti;

	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, float opacity = 1, float refracti = 1)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
		this->opacity = opacity;
		this->refracti = refracti;
	}

	Material() {

	}
};

