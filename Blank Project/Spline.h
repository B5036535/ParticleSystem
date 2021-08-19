#pragma once
#include "../nclgl/Vector3.h"
#include "../nclgl/Vector2.h"

struct SplineCoord
{
	float x;
	float y;
	float length_segment;
	float length_x;

	friend std::ostream& operator<<(std::ostream& os, const SplineCoord& sc)
	{
		os << "{" << sc.x << ", " << sc.y << ", " << "}";
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

		controlPoints[0] = {-0.1f, 0.0f};
		controlPoints[1] = {0.0f, 0.0f};
		controlPoints[2] = {0.15f,0.0f};
		controlPoints[3] = {0.3f, 0.0f};
		controlPoints[4] = {0.6f, 0.0f};
		controlPoints[5] = {0.8f, 0.0f};
		controlPoints[6] = {1.0f, 0.0f};
		controlPoints[7] = {1.1f, 0.0f};

		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].length_segment = i < 6 && i > 0 ? CalculateSplineSegmentLength(i) : 0.f;
			totalLength += controlPoints[i].length_segment;
		}

		for (int i = 1; i < 6; i++)
		{
			controlPoints[i].length_x = controlPoints[i + 1].x - controlPoints[i].x;
		}
	};

	Spline(const Spline& s)
	{
		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].x = s.controlPoints[i].x;
			controlPoints[i].y = s.controlPoints[i].y;
			controlPoints[i].length_segment = s.controlPoints[i].length_segment;
			controlPoints[i].length_x = s.controlPoints[i].length_x;
		}

		totalLength = s.totalLength;
	}

	Spline(float y)
	{
		controlPoints[0] = { -0.1f, 0.0f };
		controlPoints[1] = { 0.0f, 0.0f };
		controlPoints[2] = { 0.15f,0.0f };
		controlPoints[3] = { 0.3f, 0.0f };
		controlPoints[4] = { 0.6f, 0.0f };
		controlPoints[5] = { 0.8f, 0.0f };
		controlPoints[6] = { 1.0f, 0.0f };
		controlPoints[7] = { 1.1f, 0.0f };

		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].y = y;
		}
		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].length_segment = i < 6 && i > 0 ? CalculateSplineSegmentLength(i) : 0.f;
			totalLength += controlPoints[i].length_segment;
		}
		for (int i = 1; i < 6; i++)
		{
			controlPoints[i].length_x = controlPoints[i + 1].x - controlPoints[i].x;
		}
	}

	Spline(Vector2 points[8])
	{

		controlPoints[0] = { -0.1f, 0.0f };
		controlPoints[1] = { 0.0f, 0.0f };
		controlPoints[2] = { 0.15f,0.0f };
		controlPoints[3] = { 0.3f, 0.0f };
		controlPoints[4] = { 0.6f, 0.0f };
		controlPoints[5] = { 0.8f, 0.0f };
		controlPoints[6] = { 1.0f, 0.0f };
		controlPoints[7] = { 1.1f, 0.0f };

		for (int i = 0; i < 8; i++)
		{
			if (((i > 0 && i < 7) && ((points[i].x > 1.0f) ||  points[i].x < 0.0f)) || (i > 0 && points[i].x < points[i - 1].x))
				return;

			controlPoints[i].x = points[i].x;
			controlPoints[i].y = points[i].y;
		}
		for (int i = 0; i < 8; i++)
		{
			controlPoints[i].length_segment = i < 6 && i > 0 ? CalculateSplineSegmentLength(i) : 0.f;
			totalLength += controlPoints[i].length_segment;
		}
		for (int i = 1; i < 6; i++)
		{
			controlPoints[i].length_x = controlPoints[i + 1].x - controlPoints[i].x;
		}
	}

	SplineCoord controlPoints[8];
	float totalLength = 0.f;
	float maxValues[3] = { 0.f,0.f,0.f };
	SplineCoord GetPointOnSpline(float t);
	float CalculateSplineSegmentLength(int initialPoint);
	float GetSplineValue(float normalizedAge);

	friend Spline& operator*(const Spline& lhs, const float rhs)
	{
		Spline result = lhs;

		for (int i = 0; i < 8; i++)
		{
			result.controlPoints[i].y = lhs.controlPoints[i].y * rhs;
			result.controlPoints[i].length_segment = lhs.controlPoints[i].length_segment * rhs;
		}

		return result;
	}

	friend Spline& operator*=(Spline& lhs, const float rhs)
	{

		for (int i = 0; i < 8; i++)
		{
			lhs.controlPoints[i].y *= rhs;
			lhs.controlPoints[i].length_segment *=  rhs;
		}

		return lhs;
	}
	//float GetSplineTime(float totalTime, float maxLifeTime);
};