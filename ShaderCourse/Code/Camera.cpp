#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

glm::vec3 Camera::GetForwardVector()
{
	glm::quat YawQuat = glm::angleAxis(glm::radians(Yaw), glm::vec3(0.f, 1.f, 0.f));
	glm::quat PitchQuat = glm::angleAxis(glm::radians(Pitch), glm::vec3(1.f, 0.f, 0.f));

	glm::quat FinalQuat = YawQuat * PitchQuat;
	return FinalQuat * glm::vec3(0.f, 0.f, -1.f);
}

glm::vec3 Camera::GetRightVector()
{
	return glm::normalize(glm::cross(GetForwardVector(), glm::vec3(0.f, 1.f, 0.f)));
}

void Camera::AddYaw(float YawAngle)
{
	Yaw += YawAngle;
}

void Camera::AddPitch(float PitchAngle)
{
	Pitch += PitchAngle;
	Pitch = glm::clamp(Pitch, -85.f, 85.f);
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + GetForwardVector(), glm::vec3(0.f, 1.f, 0.f));
}

Transform Camera::GetTransform() const
{
	Transform Result;
	Result.Position = Position;

	// Calculate rotation
	glm::quat YawQuat = glm::angleAxis(glm::radians(Yaw), glm::vec3(0.f, 1.f, 0.f));
	glm::quat PitchQuat = glm::angleAxis(glm::radians(Pitch), glm::vec3(1.f, 0.f, 0.f));
	Result.Rotation = YawQuat * PitchQuat;

	return Result;
}