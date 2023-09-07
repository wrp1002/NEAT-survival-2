#pragma once

#include <vector>
#include <memory>

class Node;

using namespace std;

class Connection
{
private:
	double weight;
	bool enabled;
	int innovationNumber;
	weak_ptr<Node> fromNode;
	weak_ptr<Node> toNode;

public:
	Connection(shared_ptr<Node> from, shared_ptr<Node> to);
	~Connection();

	void Print();
	void RandomWeight();

	void ToggleEnabled();
	void SetFrom(shared_ptr<Node> val) { this->fromNode = val; }
	void SetTo(shared_ptr<Node> val) { this->toNode = val; }
	void SetEnabled(bool val) { this->enabled = val; }
	void SetInnovationNumber(int val) { this->innovationNumber = val; }
	void SetWeight(double val) { this->weight = val; }

	shared_ptr<Node> GetFrom() { return fromNode.lock(); }
	shared_ptr<Node> GetTo() { return toNode.lock(); }
	double GetWeight() { return weight; }
	bool GetEnabled() { return enabled; }
};

