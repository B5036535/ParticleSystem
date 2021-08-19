#include "Spline.h"
#include "math.h"
#include <iostream>
SplineCoord Spline::GetPointOnSpline(float t)
{

	int p1 = (int)t;
	int p0 = p1 - 1;
	int p2 = p1 + 1;
	int p3 = p2 + 1;

	t = t - (int)t;
	
	float tSquared = t * t;
	float tCubed = tSquared * t;

	float influence[4];
	influence[0] = -tCubed + 2.0f * tSquared - t;
	influence[1] = 3.0f * tCubed - 5.0f * tSquared + 2.0f;
	influence[2] = -3.0f * tCubed + 4.0f * tSquared + t;
	influence[3] = tCubed - tSquared;

	float x = 0.5f * (controlPoints[p0].x * influence[0] + controlPoints[p1].x * influence[1] + controlPoints[p2].x * influence[2] + controlPoints[p3].x * influence[3]);

	float y = 0.5f * (controlPoints[p0].y * influence[0] + controlPoints[p1].y * influence[1] + controlPoints[p2].y * influence[2] + controlPoints[p3].y * influence[3]);

	return { x, y};
}

float Spline::CalculateSplineSegmentLength(int initialPoint)
{
	float segmentLength = 0.f;
	float stepSize = 0.005f;

	SplineCoord oldPoint = GetPointOnSpline((float)initialPoint);
	SplineCoord newPoint;

	for (float t = stepSize; t < 1.0f; t += stepSize)
	{
		newPoint = GetPointOnSpline((float)initialPoint + t);
		
		maxValues[0] = maxValues[0] < abs(newPoint.x) ? abs(newPoint.x) : maxValues[0];
		maxValues[1] = maxValues[1] < abs(newPoint.y) ? abs(newPoint.y) : maxValues[1];

		segmentLength += sqrtf((newPoint.x - oldPoint.x) * (newPoint.x - oldPoint.x) + (newPoint.y - oldPoint.y) * (newPoint.y - oldPoint.y));
		oldPoint = newPoint;
	}

	return segmentLength;
}

float Spline::GetSplineValue(float normalizedAge)
{
	float tracker = 0;
	int i = 1;	// only points 1 - 6 have lengths
	do
	{
		tracker += controlPoints[i].length_x;
		i++;
	} while (tracker < normalizedAge && i < 7);

	if (i < 7  && tracker <= 1.0f)
	{
		tracker -= controlPoints[i - 1].length_x;
		float remainder = normalizedAge - tracker;
		//float ratio = controlPoints[i - 1].length_segment / controlPoints[i - 1].length_x;
		float progressInSection = remainder / controlPoints[i - 1].length_x;
		std::cout<< " remainder = " << remainder << " progress in section = " << progressInSection << " value = ";
		return GetPointOnSpline(i + progressInSection).y;
	}
	else
		return 0.f;
}


//float Spline::GetSplineTime(float totalTime, float maxLifeTime)
//{
//	float lengthPerT = totalLength / maxLifeTime;
//	float currentLengthTravelled = totalTime * lengthPerT;
//
//	float tracker = 0;
//	int i = 1;	// only points 1 - 6 have lengths
//	while (tracker < currentLengthTravelled && i < 7)
//	{
//		tracker += controlPoints[i].length;
//		i++;
//	}
//
//	if (i != 7)
//	{
//		tracker -= controlPoints[i].length;
//		float remainder = currentLengthTravelled - tracker;
//		return float(i - 1) + remainder / controlPoints[i].length;
//	}
//	else
//		return 0.f;
//}
