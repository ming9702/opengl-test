#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    aiString path;
};

struct Mesh {
  public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
         std::vector<Texture> textures);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    void Draw(/*QOpenGLShaderProgram& shader*/);

    unsigned int VAO, VBO, EBO;
};

class Model {
  public:
    Model(const std::string &path);
    ~Model();

    void Draw(/*QOpenGLShaderProgram& shader*/);

  private:
    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                              std::string typeName);

  private:
    std::vector<Mesh> meshes;
    std::string directory;
};

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma);
