#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>

#include <box2d/b2_math.h>
#include <iostream>
#include <string>
#include <typeinfo>
#include <memory>
#include <vector>

class Object;
class NEAT;
class Node;
class Vector2f;

using namespace std;

namespace InfoDisplay {
	extern ALLEGRO_DISPLAY* display;
	extern ALLEGRO_EVENT_QUEUE* event_queue;
	extern weak_ptr<Object> selectedObject;
	extern b2Vec2 screenSize;
	extern b2Vec2 mousePos;

	void Init(ALLEGRO_EVENT_QUEUE *ev);
	void Shutdown();

	void Toggle();
	void Show();
	void Hide();
	void Draw();
	void SelectObject(weak_ptr<Object>);
	void DrawNodes(vector<shared_ptr<Node>> nodes, shared_ptr<Node> hoveredNode, float alphaCoef, bool drawWeights);
	string DrawNN(shared_ptr<NEAT> nn);

	b2Vec2 CalculateNodePos(shared_ptr<Node> node);

	bool IsVisible();
};

