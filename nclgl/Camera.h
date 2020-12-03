#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera()
	{
		yaw = 0.0f;
		pitch = 0.0f;

		//onTrack = false;
		//currentWaypoint = 0;
		//dist = 5.0f;
		//
		//waypoints.push_back(position);
	}

	Camera(float p, float y, Vector3 pos)
	{
		pitch = p;
		yaw = y;
		position = pos;

		//onTrack = false;
		//currentWaypoint = 0;
		//dist = 5.0f;
		//
		//waypoints.push_back(position);

	}
	~Camera() {};

	void UpdateCamera(float dt = 1.0f);

	void TrackMovement(float speed);
	void FreeMovement(Vector3 forward, Vector3 right, float speed);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; };
	void SetPosition(Vector3 pos) { position = pos; };

	float GetYaw() const { return yaw; };
	void SetYaw(float y) { yaw = y; };

	float GetPitch() const { return pitch; };
	void SetPitch(float p) { pitch = p; };

protected:
	float pitch;
	float yaw;
	Vector3 position;

	//bool onTrack;
	//int currentWaypoint;
	//vector<Vector3> waypoints;
	//float dist;
};