#pragma once
class Material
{
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
	{
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}

	Material() {

	}
};

