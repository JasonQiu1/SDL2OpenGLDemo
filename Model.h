#pragma once
#include <string>
#include <vector>

#include <assimp/scene.h>

#include "Shader.h"
#include "Mesh.h"

class Model {
private:
	std::vector<Mesh> meshes;
	std::vector<Texture> texturesLoaded;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

public:
	Model(std::string path);

	void Draw(Shader& shader);
};