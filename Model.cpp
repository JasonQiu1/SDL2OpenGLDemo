#include <glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

#include <iostream>
#include <string>
#include <vector>

#include "Model.h"
#include "Shader.h"
#include "Mesh.h"

Model::Model(std::string path)
{
	loadModel(path);
}

void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene{importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs)};

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('\\')+1);

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};
		aiVector3D pos{ mesh->mVertices[i] };
		vertex.Position.x = pos.x;
		vertex.Position.y = pos.y;
		vertex.Position.z = pos.z;

		if (mesh->HasNormals()) {
			aiVector3D norm{ mesh->mNormals[i] };
			vertex.Normal.x = norm.x;
			vertex.Normal.y = norm.y;
			vertex.Normal.z = norm.z;
		}

		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.TexCoords = glm::vec2{ 0.0f };
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh{ vertices, indices, textures };
}

unsigned int TextureFromFile(std::string filePath) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, numChannels;
	unsigned char* data{ stbi_load(filePath.c_str(), &width, &height, &numChannels, 0) };
	if (data) {
		GLint format{ (numChannels == 1) ? GL_RED :
											((numChannels == 3) ? GL_RGB : GL_RGBA) };
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Could not load texture from \"" << filePath << "\"" << std::endl;
	}
	std::cout << "Loading " << filePath << std::endl;
	stbi_image_free(data);

	return textureID;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture tex;
			tex.id = TextureFromFile(directory + str.C_Str());
			tex.type = typeName;
			tex.path = std::string(str.C_Str());
			textures.push_back(tex);
			texturesLoaded.push_back(tex);
		}
	}
	return textures;
}

void Model::Draw(Shader& shader) {
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
}