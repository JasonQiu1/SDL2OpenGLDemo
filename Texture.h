#pragma once
#include <string>

class Texture {
private:
	int width;
	int height;
	int nrChannels;
	unsigned char* data;

public:
	Texture(std::string filePath);

	int getWidth();
	int getHeight();
	int getNrChannels();
	unsigned char* getData();

	bool isRGBA();
	bool isLoaded();

	void freeImage();

	~Texture();
};