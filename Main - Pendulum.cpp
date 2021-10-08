#include <SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

#include <iostream>
#include <unordered_map>

#include "Shader.h"
#include "Camera.h"

using keyMap = std::unordered_map<SDL_KeyCode, bool>;
using floatPair = std::pair<float, float>;

void showErrorBox(const char* title, const char* msg = NULL) {
	if (msg == NULL) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, title, NULL);
	}
	else {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, msg, NULL);
	}
}

void processEvents(bool& running, SDL_Event* event, keyMap& keyDown, floatPair& currMousePos) {
	while (SDL_PollEvent(event) != 0) {
		switch (event->type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
			case SDLK_ESCAPE:
				running = false;
				break;
			case SDLK_DOWN:
				GLint data[2];
				glGetIntegerv(GL_POLYGON_MODE, data);
				if (data[1] == GL_LINE)
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				else
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case SDLK_w:
				keyDown[SDLK_w] = true;
				break;
			case SDLK_a:
				keyDown[SDLK_a] = true;
				break;
			case SDLK_s:
				keyDown[SDLK_s] = true;
				break;
			case SDLK_d:
				keyDown[SDLK_d] = true;
				break;
			case SDLK_SPACE:
				keyDown[SDLK_SPACE] = true;
				break;
			case SDLK_LSHIFT:
				keyDown[SDLK_LSHIFT] = true;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event->key.keysym.sym) {
			case SDLK_w:
				keyDown[SDLK_w] = false;
				break;
			case SDLK_a:
				keyDown[SDLK_a] = false;
				break;
			case SDLK_s:
				keyDown[SDLK_s] = false;
				break;
			case SDLK_d:
				keyDown[SDLK_d] = false;
				break;
			case SDLK_SPACE:
				keyDown[SDLK_SPACE] = false;
				break;
			case SDLK_LSHIFT:
				keyDown[SDLK_LSHIFT] = false;
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			currMousePos.first = event->motion.xrel;
			currMousePos.second = event->motion.yrel;
			break;
		case SDL_WINDOWEVENT:
			switch (event->window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				glViewport(0, 0, event->window.data1, event->window.data2);
				break;
			case SDL_WINDOWEVENT_ENTER:
				std::cout << "Mouse entered.\n";
				break;
			}
			break;
		default:
			break;
		}
	}
}

float gravity = 9.8f;
float radius = 4.0f;
float theta = -15.0f;
void render(unsigned int* VAO, Shader* prog, Camera& cam) {
	glClearColor(185.0/255, 203.0/255, 153.0/255, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float greenColor = sin(SDL_GetTicks() / 1000.0 * 2.0) / 2.0 + 0.5;

	glm::mat4 view{ cam.getView() };

	// TRANSFORM MATRICES
	glm::mat4 model{ 1.0f };
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 projection{ 1.0f };
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(800 / 600), 0.1f, 100.0f);

	prog[0].use();
	prog[0].setMat4fv("view", 1, GL_FALSE, view);
	prog[0].setMat4fv("projection", 1, GL_FALSE, projection);

	float dt = glm::radians(theta * sin(sqrt(gravity / radius) * SDL_GetTicks() / 1000.0f));

	glm::vec3 cubePositions[] = {
		glm::vec3(sin(dt) * radius*2,  cos(dt) * -radius*2 + radius/2, -radius * 5.0f),
		glm::vec3(sin(dt) * radius, -cos(dt) * radius + radius/2, -radius * 5.0f)
	};

	glBindVertexArray(VAO[0]);
	// swinging box
	glm::mat4 m{ 1.0f };
	m = glm::translate(m, cubePositions[0]);
	m = glm::rotate(m, dt, glm::vec3{ cubePositions[0].x, cubePositions[0].y, 1.0f });
	prog[0].setMat4fv("model", 1, GL_FALSE, m);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// rope
	glm::mat4 m2{ 1.0f };
	m2 = glm::translate(m2, cubePositions[1]);
	m2 = glm::rotate(m2, dt, glm::vec3{0.0f, 0.0f, 1.0f});
	m2 = glm::scale(m2, glm::vec3(0.1f, 2 * radius, 0.1f));
	prog[0].setMat4fv("model", 1, GL_FALSE, m2);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

int main(int argc, char* args[]) {
	std::cout << "Initializing SDL.\n";

	SDL_Window* window;
	int width = 800;
	int height = 600;
	SDL_Surface* screenSurface;
	SDL_GLContext context;
	SDL_Event event;
	bool running = true;
	std::string shaderFolderPath = "C:\\Users\\Jason\\Source\\Repos\\SDL2 OpenGL Tests\\SDL2 OpenGL Tests\\Shaders\\";
	std::string textureFolderPath = "C:\\Users\\Jason\\Source\\Repos\\SDL2 OpenGL Tests\\SDL2 OpenGL Tests\\Textures\\";

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		showErrorBox("Could not initialize SDL: ", SDL_GetError());
		return -1;
	}

	atexit(SDL_Quit);

	std::cout << "SDL initialized.\n";

	window = SDL_CreateWindow("SDL/OpenGL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		showErrorBox("Failed to create window: ", SDL_GetError());
		return -1;
	}

	screenSurface = SDL_GetWindowSurface(window);
	if (screenSurface == NULL) {
		showErrorBox("Failed to create surface: ", SDL_GetError());
		return -1;
	}

	context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		showErrorBox("Failed to create context: ", SDL_GetError());
		return -1;
	}

	// load OpenGL functions from GLAD
	if (gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress) < 0) {
		showErrorBox("Failed to initialze GLAD.");
		return -1;
	}

	// create shaders
	Shader prog[2] = { *new Shader((shaderFolderPath + "vShader1.txt").c_str(), (shaderFolderPath + "fShader1.txt").c_str()) ,
					   *new Shader((shaderFolderPath + "vShader2.txt").c_str(), (shaderFolderPath + "fShader2.txt").c_str()) };

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VAO[1];
	unsigned int VBO[1];
	glGenVertexArrays(1, VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// SET VERTEX ATTRIBUTE POINTERS
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// TEXTURES
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//float borderColor[]{ 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int nrChannels;
	unsigned char* data = stbi_load((textureFolderPath + "cobblestone.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		showErrorBox("Failed to load texture.");
	}
	stbi_image_free(data);

	// TRANSFORM MATRICES
	glm::mat4 model{ 1.0f };
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 view{ 1.0f };
	view = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 projection{ 1.0f };
	projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width / height), 0.1f, 100.0f);

	// UNIFORM VARIABLES
	prog[0].use();
	prog[0].setInt("texture1", 0);

	// CAMERA
	Camera cam{};

	// LOOP
	keyMap keyDown{};
	floatPair lastMousePos{};
	floatPair currMousePos{};

	float lastFrame = SDL_GetTicks();
	float currFrame = SDL_GetTicks();
	float deltaTime = 1;

	glEnable(GL_DEPTH_TEST);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (running) {
		lastFrame = currFrame;
		currFrame = SDL_GetTicks();
		deltaTime = (currFrame - lastFrame) / 1000;

		lastMousePos.first = currMousePos.first;
		lastMousePos.second = currMousePos.second;
		processEvents(running, &event, keyDown, currMousePos);

		// handle keyDown events
		for (auto p : keyDown) {
			if (!p.second) continue;
			switch (p.first) {
				case SDLK_w:
					cam.updatePos(deltaTime * cam.getSpeed() * cam.getFront());
					break;
				case SDLK_a:
					cam.updatePos(deltaTime * cam.getSpeed() * -cam.getRight());
					break;
				case SDLK_s:
					cam.updatePos(deltaTime * cam.getSpeed() * -cam.getFront());
					break;
				case SDLK_d:
					cam.updatePos(deltaTime * cam.getSpeed() * cam.getRight());
					break;
				case SDLK_SPACE:
					cam.updatePos({ 0.0f, deltaTime * cam.getSpeed(), 0.0f });
					break;
				case SDLK_LSHIFT:
					cam.updatePos({ 0.0f, deltaTime * -cam.getSpeed(), 0.0f });
					break;
				default:
					break;
			}
		}

		// Lock y position
		// cam.setPos({ cam.getPos().x, 0.0f, cam.getPos().z });

		// handle mouse movement
		float deltaX = (currMousePos.first == lastMousePos.first) ? 0.0f : currMousePos.first;
		float deltaY = (currMousePos.second == lastMousePos.second) ? 0.0f : -currMousePos.second; // flipped since y coords rise from bottom to top
		cam.updateFront(deltaX * cam.getSensitivity(), deltaY * cam.getSensitivity());

		// RENDER
		render(VAO, prog, cam);

		SDL_GL_SwapWindow(window);
	}

	std::cout << "Quitting SDL.\n";

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	window = nullptr;
	SDL_Quit();

	return 0;
}