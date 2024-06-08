#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.h"

Camera::Camera() {
	updateVectors();
}

void Camera::startMove(CameraDirection direction) {
	movementKeys[(int) direction] = true;
}

void Camera::endMove(CameraDirection direction) {
	movementKeys[(int) direction] = false;
}

void Camera::moveMouse(glm::vec2 newPosition) {
	if (mouseJustEntered) {
		prevMousePosition = newPosition;
		mouseJustEntered = false;
	}

	glm::vec2 offset = newPosition - prevMousePosition;
	prevMousePosition = newPosition;

	yaw += offset.x * mouseSensitivity;
	pitch -= offset.y * mouseSensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
	updateVectors();
}

void Camera::mouseExit(int exitedWindow) {
	if (exitedWindow) {
		for (int i = 0; i < 6; i++) {
			movementKeys[i] = false;
		}
	}
}

void Camera::frameUpdate(float elapsedTime) {
	glm::vec3 direction = glm::vec3(0.0f);
	if (movementKeys[(int) CAMERA_UP]) direction += up;
	if (movementKeys[(int) CAMERA_DOWN]) direction -= up;
	if (movementKeys[(int) CAMERA_RIGHT]) direction += right;
	if (movementKeys[(int) CAMERA_LEFT]) direction -= right;
	if (movementKeys[(int) CAMERA_FORWARD]) direction += forward;
	if (movementKeys[(int) CAMERA_BACKWARD]) direction -= forward;

	if (glm::length(direction) == 0) return;

	position += glm::normalize(direction) * elapsedTime * speed;
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + forward, up);
}

void Camera::updateVectors() {
	forward = glm::normalize(glm::vec3(
		cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
		sin(glm::radians(pitch)),
		sin(glm::radians(yaw)) * cos(glm::radians(pitch))
	));
	right = glm::normalize(glm::cross(forward, worldUp));
	up = glm::normalize(glm::cross(right, forward));
}
