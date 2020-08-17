#include "Model.h"

#include <iostream>
#include <glad/glad.h>
#include <stdexcept>

Model::Model(const std::string& path) {
  if (!gladLoadGL()) throw new std::runtime_error("err");
  loadModel(path);
}

Model::~Model() {}

// void Model::Draw(QOpenGLShaderProgram& shader) {
//   for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);
// }

void Model::loadModel(const std::string& path) {
  Assimp::Importer import;
  const aiScene* scene =
      import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
  // 处理节点所有的网格（如果有的话）
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }
  // 接下来对它的子节点重复这一过程
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    // 处理顶点位置、法线和纹理坐标
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.Position = vector;

    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.Normal = vector;

    if (mesh->mTextureCoords[0])  // 网格是否有纹理坐标？
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.TexCoords = vec;
    } else
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);

    vertices.push_back(vertex);
  }
  // 处理索引
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }
  // 处理材质
//   if (mesh->mMaterialIndex >= 0) {
//     aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//     std::vector<Texture> diffuseMaps = loadMaterialTextures(
//         material, aiTextureType_DIFFUSE, "texture_diffuse");
//     textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//     std::vector<Texture> specularMaps = loadMaterialTextures(
//         material, aiTextureType_SPECULAR, "texture_specular");
//     textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//   }
  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
                                                 aiTextureType type,
                                                 std::string typeName) {
//   std::vector<Texture> textures;
//   for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
//     aiString str;
//     mat->GetTexture(type, i, &str);
//     Texture texture;
//     texture.id = TextureFromFile(str.C_Str(), directory);
//     texture.type = typeName;
//     texture.path = str;
//     textures.push_back(texture);
//   }
//   return textures;
  return {};
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture> textures)
    : vertices{vertices}, indices{indices}, textures{textures} {}

// void Mesh::Draw(QOpenGLShaderProgram& shader) {
  // 	unsigned int diffuseNr = 1;
  // 	unsigned int specularNr = 1;
  // 	for (unsigned int i = 0; i < textures.size(); i++)
  // 	{
  // 		glActiveTexture(GL_TEXTURE0 + i); //
  // 在绑定之前激活相应的纹理单元
  // 		// 获取纹理序号（diffuse_textureN 中的 N）
  // 		std::string number;
  // 		std::string name = textures[i].type;
  // 		if (name == "texture_diffuse")
  // 			number = std::to_string(diffuseNr++);
  // 		else if (name == "texture_specular")
  // 			number = std::to_string(specularNr++);
  //
  // 		shader.setFloat(("material." + name + number).c_str(), i);
  // 		glBindTexture(GL_TEXTURE_2D, textures[i].id);
  // 	}
  // 	glActiveTexture(GL_TEXTURE0);
  //
  // 	// 绘制网格
  // 	glBindVertexArray(VAO);
  // 	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  // 	glBindVertexArray(0);

/*}*/