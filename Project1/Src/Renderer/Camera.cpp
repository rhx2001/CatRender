#include <iostream>
#include <Renderer/Camera.hpp>

void Camera::initCamera()
{
	setTranslation(glm::vec3(-5.0f, 0, 0));
	
	setPerspective(60.0f, 16.0f/9.0f, 0.01f, 1000.0f);
	setMovementSpeed(1.0f);
	setRotationSpeed(0.25f);

}
