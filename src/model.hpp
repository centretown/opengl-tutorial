#pragma once

#include "mesh.hpp"
#include "shader.hpp"

#include <assimp/scene.h> // Output data structure

class Model {
private:
  vector<Texture> textures_loaded;
  vector<Mesh> meshes;
  string directory;
  bool gammaCorrection;

  void loadModel(string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       string typeName);

public:
  Model(string const &path, bool gamma = false) : gammaCorrection(gamma) {
    loadModel(path);
  }
  void Draw(Shader &shader);
};
