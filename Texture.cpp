#include "Texture.h"

#include <iostream>
#include <string>

#include "stb_image.h"

Texture::Texture(std::string filePath)
	: data(stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0))
{
	if (!data) {
		std::cout << "Failed to load texture from: " << filePath << std::endl;
	}
}

int Texture::getWidth() { return this->width; }
int Texture::getHeight() { return this->height; }
int Texture::getNrChannels() { return this->nrChannels; }
unsigned char* Texture::getData() { return this->data; }

bool Texture::isRGBA() { return this->nrChannels == 4; }
bool Texture::isLoaded() { return this->data; }

void Texture::freeImage() { stbi_image_free(this->data); this->data = nullptr; }

Texture::~Texture() {
	if (data) freeImage();
}