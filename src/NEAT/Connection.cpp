#include "Connection.h"

#include "Node.h"
#include "../Globals.h"
#include "../Util.h"

Connection::Connection(shared_ptr<Node> from, shared_ptr<Node> to) {
	this->fromNode = from;
	this->toNode = to;

	this->weight = 0;
	this->enabled = true;
	this->innovationNumber = 0;
	RandomWeight();
}

Connection::~Connection() {

}

void Connection::Print() {
	printf("Connection{ ");
	this->fromNode.lock()->Print();
	printf("  =>  ");
	this->toNode.lock()->Print();
	printf(", Weight: %f, Enabled: %s, inn#:%i }", weight, (enabled ? "True" : "False"), innovationNumber);
}

void Connection::RandomWeight() {
	this->weight = Util::Random() * 4 - 2;
}

void Connection::ToggleEnabled() {
	enabled = !enabled;
}
