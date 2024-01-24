#include "Node.h"


#include "Connection.h"


void Node::_RemoveConnection(shared_ptr<Connection> connection, vector<shared_ptr<Connection>>* connections) {
	for (unsigned i = 0; i < connections->size(); i++) {
		if (connections->at(i) == connection) {
			connections->erase(connections->begin() + i);
			break;
		}
	}
}

double Node::Tanh(double input) {
	return tanh(input);
}

double Node::Sigmoid(double input) {
	return 1 / (1 + exp(-input));	// sigmoid
}


Node::Node(float x, float y, int type, int ID, string name, string activationFuncStr) : pos(x, y) {
	this->type = type;
	this->name = name;
	this->output = 0;
	this->ID = ID;
	this->activationFuncStr = activationFuncStr;

	if (activationFuncStr == "sigmoid")
		this->ActivationFunction = &Node::Sigmoid;
	else if (activationFuncStr == "tanh")
		this->ActivationFunction = &Node::Tanh;
}


Node::~Node() {
}

void Node::Print() {
	cout << "node " << ID << " type:" << type << " X:" << pos.x << " Y:" << pos.y << endl;
	cout << "connections:" << allConnections.size() << " toConnections:" << toConnections.size() << " fromConnections:" << fromConnections.size() << endl;
}

void Node::Calculate() {
	/*
	if (fromConnections.size() == 0) {
		this->output = (this->*ActivationFunction)(1.0);
		return;
	}
	*/

	double sum = 0;
	for (auto connection : fromConnections) {
		if (connection->GetEnabled()) {
			sum += connection->GetFrom()->GetOutput() * connection->GetWeight();
		}
	}
	this->output = (this->*ActivationFunction)(sum);
}

void Node::AddConnection(shared_ptr<Connection> connection) {
	allConnections.push_back(connection);

	if (connection->GetFrom().get() == this)
		toConnections.push_back(connection);
	if (connection->GetTo().get() == this)
		fromConnections.push_back(connection);
}

void Node::RemoveConnection(shared_ptr<Connection> connection) {
	_RemoveConnection(connection, &allConnections);
	_RemoveConnection(connection, &fromConnections);
	_RemoveConnection(connection, &toConnections);
}

void Node::ClearConnections() {
	allConnections.clear();
	toConnections.clear();
	fromConnections.clear();
}

b2Vec2 Node::GetPos() {
	return pos;
}

vector<shared_ptr<Connection>> Node::GetFromConnections() {
	return fromConnections;
}

vector<shared_ptr<Connection>> Node::GetToConnections() {
	return toConnections;
}

vector<shared_ptr<Connection>> Node::GetAllConnections() {
	return allConnections;
}

int Node::GetID() {
	return ID;
}
