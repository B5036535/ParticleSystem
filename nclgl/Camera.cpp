#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt)
{
	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0)
		yaw += 360.0f;
	if (yaw > 0)
		yaw -= 360.0f;

	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right   = rotation * Vector3(1, 0, 0);

	float speed = 50.0f * dt;

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_T))
		onTrack = !onTrack;

	onTrack ? TrackMovement(speed) : FreeMovement(forward, right, speed);
}

void Camera::TrackMovement(float speed)
{
	Vector3 dir = waypoints[currentWaypoint] - position;
	Vector3 dirNormal = dir.Normalised();
	position +=  dirNormal * speed;
	
	float currentDist = dir.Length();

	currentWaypoint =  currentDist <= dist ? (currentWaypoint + 1) % 5 : currentWaypoint;
}

void Camera::FreeMovement(Vector3 forward, Vector3 right, float speed)
{
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W))
		position += forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S))
		position -= forward * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A))
		position -= right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D))
		position += right * speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT))
		position.y -= speed;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE))
		position.y += speed;
}

Matrix4 Camera::BuildViewMatrix()
{
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) * Matrix4::Translation(-position);
}
