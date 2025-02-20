#include <Renderer/Camera.hpp>

void Camera::initCamera()
{
	setPosition(glm::vec3(0, 0, 1));
	setPerspective(60.0f, 16.0f/9.0f, 0.1f, 256.0f);

	setMovementSpeed(1.0f);
	setRotationSpeed(0.25f);
}
