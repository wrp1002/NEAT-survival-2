#include "Util.h"

#include "Globals.h"
#include <allegro5/transformations.h>
#include <fcntl.h>

b2Vec2 Util::metersToPixels(float xMeters, float yMeters) {
	return b2Vec2(xMeters * Globals::scaling, yMeters * Globals::scaling);
}

b2Vec2 Util::metersToPixels(b2Vec2 pos) {
	return metersToPixels(pos.x, pos.y);
}

float Util::metersToPixels(float val) {
	return val * Globals::scaling;
}

b2Vec2 Util::pixelsToMeters(float xPixels, float yPixels) {
	return b2Vec2(xPixels / Globals::scaling, yPixels / Globals::scaling);
}

b2Vec2 Util::pixelsToMeters(b2Vec2 pos) {
	return pixelsToMeters(pos.x, pos.y);
}

float Util::pixelsToMeters(float val) {
	return val / Globals::scaling;
}

float Util::mapVal(float input, float input_start, float input_end, float output_start, float output_end) {
	float slope = (output_end - output_start) / (input_end - input_start);
	float output = output_start + slope * (input - input_start);
	return output;
}

int Util::clamp(int val, int min, int max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

float Util::clamp(float val, float min, float max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

float Util::DegreesToRadians(int degrees) {
	return degrees * M_PI / 180;
}

int Util::RadiansToDegrees(float radians) {
	return radians * 180 / M_PI;
}

double Util::Random() {
	return double(rand()) / double(RAND_MAX);
}

float Util::RandomDir() {
	return float((rand() % 360 - 180) * (M_PI / 180));
}

int Util::RandomInt(int min, int max) {
	return rand() % (max - min) + min;
}

int Util::RandomSign() {
	if (rand() % 2 == 0)
		return 1;
	else
		return -1;
}

float Util::RandomNormal() {
	float amount = 16;
	double sum = 0;
	for (int i = 0; i < amount; i++)
		sum += Random();
	sum /= amount;
	return float(sum);
}

double Util::Round(double num) {
	return double(int(num * 100) / 100);
}

b2Vec2 Util::RandomWorldPosPX() {
	float angle = Util::RandomDir();
	int dist = Globals::WORLD_SIZE_PX * sqrt(Util::Random());
	return b2Vec2(cos(angle) * dist, sin(angle) * dist);
}


void Util::ResetTransform() {
	ALLEGRO_TRANSFORM identityTransform;
	al_identity_transform(&identityTransform);
	al_use_transform(&identityTransform);
}