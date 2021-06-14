#pragma once

struct SplineCoord
{
	float x;
	float y;
	float length;
};

class Spline
{
public:
	SplineCoord controlPoints[8];

	SplineCoord GetPointOnSpline(float t);
	float CalculateSplineSegmentLength(int initialPoint);
};