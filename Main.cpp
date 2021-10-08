#include <glad.c>
#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "Model.h"
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

void updateCamera(Camera& cam, float deltaTime, keyMap& keyDown, floatPair& deltaMove) {
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

	cam.updateFront(deltaMove.first * cam.getSensitivity(), deltaMove.second * cam.getSensitivity());
}

void render(std::vector<Model> models, Shader* prog, Camera& cam) {
	glClearColor(185.0 / 255, 203.0 / 255, 153.0 / 255, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float slowness = 1;
	float radius = 1.0f;
	glm::vec3 lightPos{ sin(SDL_GetTicks() / (slowness * 1000.0)) * radius * 5, cos(SDL_GetTicks() / (slowness * 1000.0)) * radius, cos(SDL_GetTicks() / (slowness * 1000.0)) * sin(SDL_GetTicks() / (slowness * 1000.0)) * radius };
	//lightPos = glm::vec3{ -0.0f, -1.0f, -0.0f };

	glm::vec3 lightColor{ abs(sin(SDL_GetTicks() / 1000.0)), abs(cos(SDL_GetTicks() / 1000.0)), abs(cos(SDL_GetTicks() / 1000.0) * sin(SDL_GetTicks() / 1000.0)) };
	//lightColor = glm::vec3{ 1.0f,1.0f,1.0f };

	// set uniforms
	prog[0].use();

	prog[0].set3fv("pntLights[0].position", 1, lightPos);
	prog[0].set3fv("pntLights[0].ambient", 1, lightColor * glm::vec3(0.2));
	prog[0].set3fv("pntLights[0].diffuse", 1, lightColor * glm::vec3(0.5));
	prog[0].set3fv("pntLights[0].specular", 1, lightColor * glm::vec3(1.0));

	prog[0].set3fv("spotLight.position", 1, cam.getPos());
	prog[0].set3fv("spotLight.direction", 1, cam.getFront() - cam.getPos());

	prog[0].set3fv("viewPos", 1, cam.getPos());

	// transform matrices
	glm::mat4 model{ 1.0f };
	model = glm::scale(model, glm::vec3{ 0.3f });
	glm::mat4 view{ cam.getView() };
	glm::mat4 projection{ glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f) };

	prog[0].setMat4fv("model", 1, false, model);
	prog[0].setMat4fv("view", 1, false, view);
	prog[0].setMat4fv("projection", 1, false, projection);

	for (int i = 0; i < models.size(); i++) {
		models[i].Draw(prog[0]);
	}
}

int main(int argc, char* args[]) {
	// INITIALIZE SDL AND OPENGL
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

	// SHADERS
	Shader prog[1] = { *new Shader((shaderFolderPath + "vShader1.vert").c_str(), (shaderFolderPath + "fShader1.frag").c_str()) };

	stbi_set_flip_vertically_on_load(true);
	Model backpack(textureFolderPath + "backpack\\backpack.obj");

	// LIGHTS
	prog[0].use();
	prog[0].setFloat("material.shininess", 25.0f);
	prog[0].setFloat("pntLights[0].constant", 1.0f);
	prog[0].setFloat("pntLights[0].linear", 0.09f);
	prog[0].setFloat("pntLights[0].specular", 0.032f);

	glm::vec3 lightColor = { (210 / 255.0), (108 / 255.0), (29 / 255.0) };
	prog[0].set3fv("dirLight.direction", 1, glm::vec3{ 0.0f, -1.0f, -0.2f });
	prog[0].set3fv("dirLight.ambient", 1, lightColor * glm::vec3(0.2));
	prog[0].set3fv("dirLight.diffuse", 1, lightColor * glm::vec3(0.5));
	prog[0].set3fv("dirLight.specular", 1, lightColor * glm::vec3(1.0));

	lightColor = { 0.0f, 0.0f, 0.0f };
	prog[0].setFloat("spotLight.cutoff", cos(glm::radians(45.0f)));

	prog[0].set3fv("spotLight.ambient", 1, lightColor * glm::vec3(0.8));
	prog[0].set3fv("spotLight.diffuse", 1, lightColor * glm::vec3(0.8));
	prog[0].set3fv("spotLight.specular", 1, lightColor * glm::vec3(1.0));
	prog[0].setFloat("spotLight.constant", 1.0f);
	prog[0].setFloat("spotLight.linear", 0.09f);
	prog[0].setFloat("spotLight.specular", 0.032f);

	// EVENT-RENDER LOOP
	Camera cam{};

	keyMap keyDown{};
	floatPair lastMousePos{};
	floatPair currMousePos{};

	float lastFrame;
	float currFrame = SDL_GetTicks();
	float deltaTime = 1;

	glEnable(GL_DEPTH_TEST);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	while (running) {
		lastFrame = currFrame;
		currFrame = SDL_GetTicks();
		deltaTime = (currFrame - lastFrame) / 1000;

		// FRAME COUNT
		SDL_SetWindowTitle(window, ("SDL/OpenGL | msPF: " + std::to_string((int) (currFrame - lastFrame))).c_str());

		// EVENTS
		lastMousePos.first = currMousePos.first;
		lastMousePos.second = currMousePos.second;
		processEvents(running, &event, keyDown, currMousePos);

		// update camera
		float deltaX = (currMousePos.first == lastMousePos.first) ? 0.0f : currMousePos.first;
		float deltaY = (currMousePos.second == lastMousePos.second) ? 0.0f : -currMousePos.second; // flipped since y coords rise from bottom to top
		updateCamera(cam, deltaTime, keyDown, *new floatPair{ deltaX, deltaY });

		// RENDER
		render(std::vector<Model>{ backpack }, prog, cam);

		SDL_GL_SwapWindow(window);
	}

	// COLLECT GARBAGE
	std::cout << "Quitting SDL.\n";
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	window = nullptr;
	SDL_Quit();

	return 0;
}