#include "NEAT.h"

#include <box2d/b2_math.h>

#include <stdexcept>

#include "../Util.h"
#include "Node.h"
#include "Connection.h"



void NEAT::_RemoveNode(vector<shared_ptr<Node>>* nodes, shared_ptr<Node> nodeToRemove) {
	for (unsigned int i = 0; i < nodes->size(); i++) {
		if (nodes->at(i) == nodeToRemove) {
			nodes->erase(nodes->begin() + i);
			return;
		}
	}
}

shared_ptr<NEAT> NEAT::RandomNN(vector<string> inputLabels, vector<string> outputLabels, int mutations) {
	shared_ptr<NEAT> newNN = make_shared<NEAT>(NEAT(inputLabels, outputLabels));
	for (int i = 0; i < mutations; i++)
		newNN->MutateAddConnection();
	return newNN;
}

NEAT::NEAT(vector<string> inputLabels, vector<string> outputLabels) {
	for (string label : inputLabels) {
		shared_ptr<Node> inputNode = make_shared<Node>(Node(0, float(inputNodes.size()) / inputLabels.size(), Node::INPUT, currentNodeID, label));
		AddNode(inputNode);
	}

	for (string label : outputLabels) {
		shared_ptr<Node> outputNode = make_shared<Node>(Node(1, float(outputNodes.size()) / outputLabels.size(), Node::OUTPUT, currentNodeID, label));
		AddNode(outputNode);
	}
}

NEAT::NEAT() {
}


NEAT::~NEAT() {
}

shared_ptr<NEAT> NEAT::Copy() {
	shared_ptr<NEAT> nn = make_unique<NEAT>(NEAT());
	vector<shared_ptr<Node>> newNodes;

	for (unsigned int i = 0; i < allNodes.size(); i++) {
		shared_ptr<Node> node = allNodes[i];
		shared_ptr<Node> newNode = make_shared<Node>(Node(node->GetPos().x, node->GetPos().y, node->GetType(), node->GetID(), node->GetName()));
		newNode->ClearConnections();

		for (unsigned j = 0; j < newNodes.size(); j++) {
			if (newNodes[j]->GetID() == newNode->GetID()) {
				cout << "allnodes: " << endl;
				for (unsigned i = 0; i < allNodes.size(); i++)
					allNodes[i]->Print();

				cout << "new nodes:" << endl;
				for (unsigned i = 0; i < newNodes.size(); i++)
					newNodes[i]->Print();

				nn->PrintNN();
				cout << "TWO ID" << endl;
				newNode->Print();

			}
		}

		nn->AddNode(newNode);
		newNodes.push_back(newNode);
	}

	for (unsigned int i = 0; i < allNodes.size(); i++) {
		shared_ptr<Node> node = allNodes[i];
		shared_ptr<Node> newNode = newNodes[i];

		for (shared_ptr<Connection> connection : node->GetToConnections()) {
			int fromID = connection->GetFrom()->GetID();
			int toID = connection->GetTo()->GetID();
			shared_ptr<Node> fromNode;
			shared_ptr<Node> toNode;

			for (shared_ptr<Node> testNode : newNodes) {
				int ID = testNode->GetID();
				if (ID == fromID)
					fromNode = testNode;
				if (ID == toID)
					toNode = testNode;
				if (fromNode && toNode)
					break;
			}

			if (fromNode && toNode) {
				shared_ptr<Connection> newConnection = make_shared<Connection>(Connection(fromNode, toNode));

				// Not sure why this happens. this is kinda just a band aid
				if (nn->ConnectionExists(newConnection)) {
					//nn->PrintNN();
					//cout << "Connection exists!!!  " << fromID << " --> " << toID << endl;
					continue;
				}
				nn->AddConnection(newConnection);
				newConnection->SetWeight(connection->GetWeight());
			}
			else
				cout << "Couldnt find node!" << endl;
		}

	}


	return nn;
}

vector<shared_ptr<Node>> NEAT::GetNodes() {
	return allNodes;
}

vector<shared_ptr<Node>> NEAT::GetInputNodes() {
	return inputNodes;
}

vector<shared_ptr<Connection>> NEAT::GetConnections() {
	return connections;
}

void NEAT::PrintNN() {
	cout << "nodes:" << allNodes.size() << " input:" << inputNodes.size() << " output:" << outputNodes.size() << endl;

	for (auto node : allNodes) {
		cout << "node " << node->GetID() << " type:" << node->GetType() << " X:" << node->GetPos().x << " Y:" << node->GetPos().y << endl;
		cout << "connections:" << node->GetAllConnections().size() << " toConnections:" << node->GetToConnections().size() << " fromConnections:" << node->GetFromConnections().size() << endl;
		for (auto connection : node->GetAllConnections()) {
			cout << connection->GetFrom()->GetID() << " --> " << connection->GetTo()->GetID() << endl;
		}
		cout << endl;
	}

	cout << endl;
	cout << "connections:" << connections.size() << endl;
	for (auto connection : connections) {
		cout << connection->GetFrom()->GetID() << " --> " << connection->GetTo()->GetID() << endl;
	}
}

vector<double> NEAT::Calculate(vector<double> inputs) {
	vector<double> outputs;

	if (inputs.size() != inputNodes.size()) {
		cout << "input nodes:" << inputNodes.size() << " given inputs:" << inputs.size() << endl;
		throw runtime_error("Input sizes don't match!");
	}

	for (unsigned i = 0; i < inputs.size(); i++)
		inputNodes[i]->SetOutput(inputs[i]);

	for (shared_ptr<Node> node : allNodes) {
		if (node->GetType() == Node::INPUT)
			continue;

		node->Calculate();
	}

	for (shared_ptr<Node> outputNode : outputNodes) {
		outputs.push_back(outputNode->GetOutput());
	}

	return outputs;
}

vector<double> NEAT::GetOutputs() {
	vector<double> outputs;
	for (shared_ptr<Node> outputNode : outputNodes)
		outputs.push_back(outputNode->GetOutput());
	return outputs;
}

void NEAT::AddNode(shared_ptr<Node> node) {
	currentNodeID++;

	if (node->GetType() == Node::INPUT)
		inputNodes.push_back(node);
	else if (node->GetType() == Node::OUTPUT)
		outputNodes.push_back(node);
	else if (node->GetType() == Node::HIDDEN)
		hiddenNodes.push_back(node);

	if (allNodes.size() == 0) {
		allNodes.push_back(node);
		return;
	}
	for (unsigned i = 0; i < allNodes.size(); i++) {
		if (node->GetPos().x < allNodes[i]->GetPos().x) {
			allNodes.insert(allNodes.begin() + i, node);
			return;
		}
	}
	allNodes.push_back(node);
}

void NEAT::RemoveNode(shared_ptr<Node> node) {
	vector<shared_ptr<Connection>> nodeConnections = node->GetAllConnections();
	for (unsigned i = 0; i < nodeConnections.size(); i++) {
		shared_ptr<Connection> connection = nodeConnections[i];
		shared_ptr<Node> otherNode = nullptr;
		if (connection->GetFrom() == node)
			otherNode = connection->GetTo();
		else if (connection->GetTo() == node)
			otherNode = connection->GetFrom();

		RemoveConnection(connection);

		if (otherNode && otherNode->GetType() == Node::HIDDEN && otherNode != node && (otherNode->GetFromConnections().size() == 0 || otherNode->GetToConnections().size() == 0))
			RemoveNode(otherNode);
	}

	node->ClearConnections();
	_RemoveNode(&allNodes, node);
	_RemoveNode(&inputNodes, node);
	_RemoveNode(&hiddenNodes, node);
	_RemoveNode(&outputNodes, node);
}

shared_ptr<Connection> NEAT::AddConnection(shared_ptr<Connection> connection) {
	connections.push_back(connection);
	connection->GetTo()->AddConnection(connection);
	connection->GetFrom()->AddConnection(connection);
	return connection;
}

shared_ptr<Connection> NEAT::AddConnection(shared_ptr<Node> from, shared_ptr<Node> to) {
	shared_ptr<Connection> connection = make_shared<Connection>(Connection(from, to));
	connections.push_back(connection);
	from->AddConnection(connection);
	to->AddConnection(connection);
	return connection;
}

void NEAT::RemoveConnection(shared_ptr<Connection> connection) {
	shared_ptr<Node> fromNode = connection->GetFrom();
	shared_ptr<Node> toNode = connection->GetTo();

	if (fromNode)
		connection->GetFrom()->RemoveConnection(connection);
	if (toNode)
		connection->GetTo()->RemoveConnection(connection);

	for (unsigned i = 0; i < connections.size(); i++) {
		if (connections[i] == connection) {
			connections.erase(connections.begin() + i);
			return;
		}
	}
}

bool NEAT::ConnectionExists(shared_ptr<Connection> connection) {
	for (auto compare : connections) {
		if (compare->GetFrom() == connection->GetFrom() && compare->GetTo() == connection->GetTo())
			return true;
	}
	return false;
}

bool NEAT::ConnectionExists(shared_ptr<Node> from, shared_ptr<Node> to) {
	for (auto compare : connections) {
		if (compare->GetFrom() == from && compare->GetTo() == to)
			return true;
		if (compare->GetFrom() == to && compare->GetTo() == from)
			return true;
	}
	return false;
}

void NEAT::Mutate() {
	for (auto connection : connections) {
		if (Util::Random() < PROBABILITY_MUTATE_TOGGLE * MUTATE_COEF) {
			connection->ToggleEnabled();
		}
		if (Util::Random() < PROBABILITY_MUTATE_WEIGHT_SHIFT * MUTATE_COEF) {
			double weight = connection->GetWeight();
			weight *= Util::Random() + 0.5;
			connection->SetWeight(weight);
		}
		if (Util::Random() < PROBABILITY_MUTATE_WEIGHT_RANDOM * MUTATE_COEF) {
			connection->RandomWeight();
		}
	}


	if (Util::Random() < PROBABILITY_MUTATE_ADD_CONNECTION * MUTATE_COEF)
		MutateAddConnection();

	if (Util::Random() < PROBABILITY_MUTATE_ADD_NODE * MUTATE_COEF)
		MutateAddNode();


	//if (Globals::Random() < PROBABILITY_MUTATE_REMOVE_NODE * MUTATE_COEF)
	//	MutateRemoveNode();
}


void NEAT::MutateAddConnection() {
	if (allNodes.size() == 0) {
		cout << "NN has no nodes?" << endl;
		return;
	}
	for (unsigned i = 0; i < MUTATE_ADD_CONNECTION_TRIES; i++) {
		shared_ptr<Node> node1 = allNodes[rand() % allNodes.size()];
		shared_ptr<Node> node2 = allNodes[rand() % allNodes.size()];
		shared_ptr<Connection> connection = nullptr;

		if (node1 == node2)
			continue;

		if (ConnectionExists(node1, node2))
			continue;

		if (node1->GetPos().x == node2->GetPos().x)
			continue;

		if (node1->GetPos().x < node2->GetPos().x)
			connection = make_shared<Connection>(Connection(node1, node2));
		else
			connection = make_shared<Connection>(Connection(node2, node1));

		AddConnection(connection);
		return;
	}
}

void NEAT::MutateAddNode() {
	if (connections.size() == 0)
		return;

	// pick a random connection
	shared_ptr<Connection> connection = connections[rand() % connections.size()];
	double weight = connection->GetWeight();
	shared_ptr<Node> fromNode = connection->GetFrom();
	shared_ptr<Node> toNode = connection->GetTo();
	b2Vec2 fromPos = fromNode->GetPos();
	b2Vec2 toPos = toNode->GetPos();

	// create new node at center of connection
	shared_ptr<Node> newNode = make_shared<Node>(Node((fromPos.x + toPos.x) / 2, (fromPos.y + toPos.y) / 2 + (Util::Random() * 0.2) - 0.1, Node::HIDDEN, currentNodeID, "Hidden Node"));
	AddNode(newNode);

	RemoveConnection(connection);

	shared_ptr<Connection>connection1 = AddConnection(fromNode, newNode);
	connection1->SetWeight(1.0);

	shared_ptr<Connection> connection2 = AddConnection(newNode, toNode);
	connection2->SetWeight(weight);
}

void NEAT::MutateRemoveNode() {
	if (hiddenNodes.size() == 0)
		return;

	shared_ptr<Node> node = hiddenNodes[rand() % hiddenNodes.size()];
	RemoveNode(node);
}

int NEAT::GetCurrentNodeID() {
	return currentNodeID;
}

int NEAT::GetInputsCount() {
	return inputNodes.size();
}

int NEAT::GetOutputsCount() {
	return outputNodes.size();
}
