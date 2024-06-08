#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

enum CameraDirection {
	CAMERA_UP = 0,
	CAMERA_DOWN = 1,
	CAMERA_LEFT = 2,
	CAMERA_RIGHT = 3,
	CAMERA_FORWARD = 4,
	CAMERA_BACKWARD = 5
};

class Camera {
private:
	const float mouseSensitivity = 1.0f;
	const float speed = 5.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec2 prevMousePosition;
	bool mouseJustEntered = true;
	bool movementKeys[6];

public:
	Camera();
	void startMove(CameraDirection direction);
	void endMove(CameraDirection direction);
	void moveMouse(glm::vec2 newPosition);
	void mouseExit(int exitedWindow);
	void frameUpdate(float elapsedTime);
	glm::mat4 getViewMatrix();
private:
	void updateVectors();

};
