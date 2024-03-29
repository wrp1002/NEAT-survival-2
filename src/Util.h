#pragma once

#include <Box2D/Box2D.h>

namespace Util {
	b2Vec2 metersToPixels(float xMeters, float yMeters);
	b2Vec2 metersToPixels(b2Vec2 pos);
	float metersToPixels(float val);
	b2Vec2 pixelsToMeters(float xPixels, float yPixels);
	b2Vec2 pixelsToMeters(b2Vec2 pos);

	float pixelsToMeters(float val);

	float mapVal(float input, float input_start, float input_end, float output_start, float output_end);
	int clamp(int val, int min, int max);
	float clamp(float val, float min, float max);

	float DegreesToRadians(int degrees);
	int RadiansToDegrees(float radians);

	double Random();
	float RandomDir();
	int RandomInt(int min, int max);
	int RandomSign();
	float RandomNormal();
	double Round(double num);
	b2Vec2 RandomWorldPosPX();

	void ResetTransform();
}