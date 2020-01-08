#ifndef MODEL_H
#define MODEL_H

#include "./mesh.h"
#include "./shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model {
  public:
  Model(const char* path) { loadModel(path); }
  void Draw(Shader shader)
  {
    for (unsigned int i = 0; i < meshes.size(); ++i) {
      meshes[i].Draw(shader);
    }
  }

  private:
  // meshes retrived from all nodes
  vector<Mesh> meshes;
  string directory;
  vector<Texture> textures_loaded;

  void loadModel(string path)
  {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
      cout << "Failed to load model: " << import.GetErrorString() << endl;
      return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
  }

  void processNode(aiNode* node, const aiScene* scene)
  {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
      processNode(node->mChildren[i], scene);
    }
  }

  Mesh processMesh(aiMesh* mesh, const aiScene* scene)
  {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex vertex;
      glm::vec3 vector;

      // position
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.Position = vector;

      // normal
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.Normal = vector;

      // texture coords
      if (mesh->mTextureCoords[0]) {
        glm::vec2 vec;
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
      } else {
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      }
      vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      aiFace face = mesh->mFaces[i];
      for (unsigned int j = 0; j < face.mNumIndices; ++j)
        indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

      vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

      vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
  }

  vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
  {
    vector<Texture> textures;

    // this model i'm using for demo has no texture specified in .obj so
    // hard code the texture file for testing
    //Texture texture;
    //texture.id = TextureFromFile("Alena_Shek_Albedo.tga.png", directory);
    //texture.type = typeName;
    //texture.path = "123";
    //textures.push_back(texture);
    //textures_loaded.push_back(texture);

    // uncomment to use parsed texture files
    // unsigned int textureCount = mat->GetTextureCount(type);
    // for (unsigned int i = 0; i < textureCount; i++) {
    // aiString str;
    // mat->GetTexture(type, i, &str);
    // bool skip = false;
    // for (unsigned int j = 0; j < textures_loaded.size(); j++) {
    // if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0) {
    // textures.push_back(textures_loaded[j]);
    // skip = true;
    // break;
    //}
    //}
    // if (!skip) {
    // Texture texture;
    // texture.id = TextureFromFile(str.C_Str(), directory);
    // texture.type = typeName;
    // texture.path = str.C_Str();
    // textures.push_back(texture);
    // textures_loaded.push_back(texture);
    //}
    //}
    return textures;
  }
};

GLuint TextureFromFile(const char* path, const string& directory, bool gamma)
{
  string filename = path;
  filename = directory + "/" + filename;
  printf("%s\n", filename.c_str());
  GLuint textureID;
  glGenTextures(1, &textureID);
  int width, height, nrComponents;
  unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;
    else
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;
}
#endif
