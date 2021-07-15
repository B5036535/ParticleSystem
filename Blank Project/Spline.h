#pragma once
#include "../nclgl/Vector3.h"

struct SplineCoord
{
	float x;
	float y;
	float z;	
	float length;

	friend std::ostream& operator<<(std::ostream& os, const SplineCoord& sc)
	{
		os << "{" << sc.x << ", " << sc.y << ", " << sc.z << "}";
		return os;
	}
};


class Spline
{
public:
	Spline()
	{
		//controlPoints[0] = { 0.f , 0.f , 0.f, 0.f  };
		//controlPoints[1] = { 10.f, 30.f, 30.f, 0.f }; 
		//controlPoints[2] = { 10.f, 31.f, 40.f, 0.f }; 
		//controlPoints[3] = { 20.f, 20.f, 30.f, 0.f }; 
		//controlPoints[4] = { 30.5, 80.5, 5.f, 0.f  };
		//controlPoints[5] = { 35.5, 50.5, 50.f, 0.f };
		//controlPoints[6] = { 42.f, 20.f, 20.f, 0.f }; 
		//controlPoints[7] = { 50.f, 2.f , 10.f, 0.f };

		controlPoints[0] = { 10.f, 10.f , 10.f, 0.f };
		controlPoints[1] = { -1.f, -1.f, -1.f, 0.f };
		controlPoints[2] = {5.f,5.f,5.f, 0.f };
		controlPoints[3] = {8.f,8.f,8.f, 0.f };
		controlPoints[4] = { 3.f, 3.f, 3.f, 0.f };
		controlPoints[5] = { -70.f, -70.f, -70.f, 0.f };
		controlPoints[6] = {1550.f, 1550.f, 1550.f, 0.f };
		controlPoints[7] = { 2500.f, 2500.f, 2500.f, 0.f };

		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].length = i < 6 && i > 0 ? CalculateSplineSegmentLength(i) : 0.f;
			totalLength += controlPoints[i].length;
		}
	};

	Spline(Vector3 points[8])
	{
		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].x = points[i].x;
			controlPoints[i].y = points[i].y;
			controlPoints[i].z = points[i].z;
		}
		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].length = i < 6 && i > 0 ? CalculateSplineSegmentLength(i) : 0.f;
			totalLength += controlPoints[i].length;
		}
	}

	SplineCoord controlPoints[8];
	float totalLength = 0.f;
	float maxValues[3] = { 0.f,0.f,0.f };
	SplineCoord GetPointOnSpline(float t);
	float CalculateSplineSegmentLength(int initialPoint);
	float GetSplineTime(float totalTime, float maxLifeTime);
};