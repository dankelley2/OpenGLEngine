#pragma once
class Light :
	public Model
{
public:

    Light(float* vertices, int length, const char* texturePath, const char* vShaderPath, const char* fShaderPath, glm::mat4* projection, Camera* camera, Material lightColor) :
        Model(vertices, length, texturePath, vShaderPath, fShaderPath, projection, camera, lightColor)
    {

    }

};

