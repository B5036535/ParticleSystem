#include "Spline.h"
#include "math.h"
SplineCoord Spline::GetPointOnSpline(float t)
{
	
	float tSquared = t * t;
	float tCubed = tSquared * t;

	float influence[4];
	influence[0] = -tCubed + 2.0f * tSquared - t;
	influence[1] = 3.0f * tCubed - 5.0f * tSquared + 2.0f;
	influence[3] = -3.0f * tCubed + 4.0f * tSquared + t;
	influence[3] = tCubed - tSquared;

	int p1 = (int)t + 1;
	int p0 = p1 - 1;
	int p2 = p1 + 1;
	int p3 = p2 + 1;
	
	float x = 0.5f * (controlPoints[p0].x * influence[0] + controlPoints[p1].x * influence[1] + controlPoints[p2].x * influence[2] + controlPoints[p3].x * influence[3]);
	float y = 0.5f * (controlPoints[p0].y * influence[0] + controlPoints[p1].y * influence[1] + controlPoints[p2].y * influence[2] + controlPoints[p3].y * influence[3]);

	return { x, y };
}

float Spline::CalculateSplineSegmentLength(int initialPoint)
{
	float segmentLength = 0.f;
	float stepSize = 0.005f;

	SplineCoord oldPoint = GetPointOnSpline(initialPoint);
	SplineCoord newPoint;

	for (float t = 0.f; t < 1.0f; t += stepSize)
	{
		newPoint = GetPointOnSpline((float)initialPoint + t);
		segmentLength += sqrtf((newPoint.x - oldPoint.x) * (newPoint.x - oldPoint.x) + (newPoint.y - oldPoint.y) * (newPoint.y - oldPoint.y));
		oldPoint = newPoint;
	}
}
