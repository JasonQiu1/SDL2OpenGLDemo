#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

static glm::vec3 calcFront(float yaw, float pitch) {
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	return glm::normalize(direction);
}

// CONSTRUCTORS
Camera::Camera(glm::vec3 initPos, glm::vec3 initFront, glm::vec3 initUp, 
	float initSens, float initYaw, float initPitch, float initSpeed)
	: pos(initPos), front(initFront), up(initUp),
		sensitivity(initSens), yaw(initYaw), pitch(initPitch), speed(initSpeed)
{
	this->front = calcFront(this->yaw, this->pitch);
}

// GETTERS
const glm::vec3 Camera::getPos() const { return this->pos; }
const glm::vec3 Camera::getFront() const { return this->front; }
const glm::vec3 Camera::getUp() const { return this->up; }
const float Camera::getSensitivity() const { return this->sensitivity; }
const float Camera::getSpeed() const { return this->speed; }

glm::vec3 Camera::getRight() const {
	return glm::normalize(glm::cross(this->front, this->up));
}
glm::mat4 Camera::getView() const {
	return glm::lookAt(this->pos, this->pos + this->front, this->up);
	/* LOOK AT IMPLEMENTATION
	glm::vec3 right{ -getRight() };
	glm::mat4 rot = glm::transpose(
		glm::mat4{ glm::vec4{right, 0.0f },
					glm::vec4{glm::normalize(glm::cross(this->front, -right)), 0.0f},
					glm::vec4{this->front, 0.0f},
					glm::vec4{0.0f, 0.0f, 0.0f, 1.0f} });
	glm::mat4 pos{ 1.0f };
	pos[3] = glm::vec4{ this->pos, 1.0f };
	return rot * pos; 
	*/
}

// MUTATORS
void Camera::setPos(glm::vec3 targetPos) { this->pos = targetPos; }
void Camera::updatePos(glm::vec3 deltaPos) { this->pos += deltaPos; }
void Camera::setFront(glm::vec3 target) { this->front = glm::normalize(target); }
void Camera::setFront(float targetYaw, float targetPitch) {
	this->yaw = targetYaw;
	this->pitch = targetPitch;
	this->front = calcFront(this->yaw, this->pitch);
}
void Camera::updateFront(float deltaYaw, float deltaPitch) {
	this->yaw = glm::mod(yaw + deltaYaw, 360.0f);
	this->pitch = std::clamp(pitch + deltaPitch, -89.0f, 89.0f);
	this->front = calcFront(this->yaw, this->pitch);
}