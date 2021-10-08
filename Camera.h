#pragma once

#include <glm/glm.hpp>

class Camera {
private:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	float yaw;
	float pitch;
	float sensitivity;
	float speed;

public:
	Camera(	glm::vec3 initPos = { 0.0f, 0.0f, 3.0f }, 
			glm::vec3 initFront = { 0.0f, 0.0f, -1.0f }, 
			glm::vec3 initUp = { 0.0f, 1.0f, 0.0f },
			float initSens = 0.3f,
			float initYaw = -90.0f,
			float initPitch = 0.0f,
			float initSpeed = 5.0f
	);

	// GETTERS
	const glm::vec3 getPos() const;
	const glm::vec3 getFront() const;
	const glm::vec3 getUp() const;
	const float getSensitivity() const;
	const float getSpeed() const;

	glm::vec3 getRight() const;
	glm::mat4 getView() const;

	// MUTATORS
	void setPos(glm::vec3 targetPos);
	void updatePos(glm::vec3 delta);

	void setFront(glm::vec3 target);
	void setFront(float yaw, float pitch);
	void updateFront(float yaw, float pitch);
};