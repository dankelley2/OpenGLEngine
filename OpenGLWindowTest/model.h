#pragma once
class Model
{

private:

    enum TYPE
    {
        MODEL_STATIC,
        MODEL_LIGHT
    };
    unsigned int loadTexture(char const* path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }


public:
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    float rotationRads = 0.0f;

    Material material;
    TYPE modelType;
    unsigned int VBO, VAO, diffuseMap, triangles;

    Shader shader;

    glm::mat4* projectionMatrix;
    Camera* camera;
    Model* lightModel;

    Model(float* vertices, int length, const char* texturePath, const char* vShaderPath, const char* fShaderPath, glm::mat4* projection, Camera* camera, Model* lightModel, Material material)
    {
        this->material = material;
        modelType = MODEL_STATIC;
        this->camera = camera;
        this->lightModel = lightModel;
        this->projectionMatrix = projection;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, length, vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        triangles = length / 5 / sizeof(float);

        createTextureShader(texturePath, vShaderPath, fShaderPath);

    }
    
    Model(float* vertices, int length, const char* texturePath, const char* vShaderPath, const char* fShaderPath, glm::mat4* projection, Camera* camera, Material lightMaterial)
    {
        material = lightMaterial;
        modelType = MODEL_LIGHT;
        this->camera = camera;
        this->projectionMatrix = projection;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, length, vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texture coord attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // texture coord attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        triangles = length / 5 / sizeof(float);

        createTextureShader(texturePath, vShaderPath, fShaderPath);

    }

    void createTextureShader(const char* texturePath, const char* vShaderPath, const char* fShaderPath)
    {
        // texture 1
        // ---------
        diffuseMap = loadTexture(texturePath);
        shader = Shader(vShaderPath, fShaderPath);
    }

    void draw()
    {
        shader.use();

        //get model space position
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, translation);
        modelMatrix = glm::rotate(modelMatrix, rotationRads, rotationAxis);
        modelMatrix = glm::scale(modelMatrix, scale);

        //send object information to shaders
        shader.setMat4("projection", *projectionMatrix);
        shader.setMat4("view", camera->GetViewMatrix());
        shader.setMat4("model", modelMatrix);

        if (modelType != MODEL_LIGHT) {

            shader.setInt("material.diffuse", 0);
            shader.setVec3("material.ambient", material.ambient);
            shader.setVec3("material.specular", material.specular);
            shader.setFloat("material.shininess", 64.0f);


            shader.setVec3("light.ambient", lightModel->material.ambient);
            shader.setVec3("light.diffuse", lightModel->material.diffuse); 
            shader.setVec3("light.specular", lightModel->material.specular);
            shader.setVec3("light.position", lightModel->translation);
            shader.setVec3("viewPos", camera->Position);

        }

        //Activate textures, bind, and draw
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, triangles);
    }

    void setTranslation(glm::vec3 position)
    {
        translation = position;
    }
    void setTranslation(float posX, float posY, float posZ)
    {
        translation = glm::vec3(posX, posY, posZ);
    }

    void setRotation(float rads, glm::vec3 axis)
    {
        rotationRads = rads;
        rotationAxis = axis;
    }
    void setRotation(float rads, float axisX, float axisY, float axisZ)
    {
        rotationRads = rads;
        rotationAxis = glm::vec3(axisX, axisY, axisZ);
    }

    void setScale(glm::vec3 amount)
    {
        scale = amount;
    }
    void setScale(float amtX, float amtY, float amtZ)
    {
        scale = glm::vec3(amtX, amtY, amtZ);
    }

    ~Model()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};