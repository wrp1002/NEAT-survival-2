#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <memory>

class Node;
class Connection;

using namespace std;

class NEAT {
private:
	vector<shared_ptr<Node>> allNodes;
	vector<shared_ptr<Node>> inputNodes;
	vector<shared_ptr<Node>> outputNodes;
	vector<shared_ptr<Node>> hiddenNodes;
	vector<shared_ptr<Connection>> connections;
	int currentNodeID = 0;

	//const double PROBABILITY_MUTATE_REMOVE_CONNECTION = 0.1;
	const float PROBABILITY_MUTATE_WEIGHT_SHIFT = 0.25;
	const float PROBABILITY_MUTATE_WEIGHT_RANDOM = 0.1;

	const float PROBABILITY_MUTATE_TOGGLE = 0.2;
	const float PROBABILITY_MUTATE_ADD_CONNECTION = 0.15;

	const float PROBABILITY_MUTATE_ADD_NODE = 0.1;
	const float PROBABILITY_MUTATE_REMOVE_NODE = 0.1;

	const unsigned MUTATE_ADD_CONNECTION_TRIES = 10;

	//float MUTATE_COEF;


	void _RemoveNode(vector<shared_ptr<Node>>* nodes, shared_ptr<Node> nodeToRemove);

public:
	static shared_ptr<NEAT> RandomNN(vector<string> inputLabels, vector<string> outputLabels, int mutations);

	NEAT(vector<string> inputLabels, vector<string> outputLabels, float mutate_coef=1.0);
	NEAT();
	~NEAT();

	shared_ptr<NEAT> Copy();
	vector<shared_ptr<Node>> GetInputNodes();
	vector<shared_ptr<Node>> GetNodes();
	vector<shared_ptr<Connection>> GetConnections();
	void PrintNN();

	vector<double> Calculate(vector<double> inputs);
	vector<double> GetOutputs();

	void AddNode(shared_ptr<Node> node);
	void RemoveNode(shared_ptr<Node> node);

	shared_ptr<Connection> AddConnection(shared_ptr<Connection> connection);
	shared_ptr<Connection> AddConnection(shared_ptr<Node> from, shared_ptr<Node> to);
	void RemoveConnection(shared_ptr<Connection> connection);
	bool ConnectionExists(shared_ptr<Connection> connection);
	bool ConnectionExists(shared_ptr<Node> from, shared_ptr<Node> to);

	void Mutate(float MUTATE_COEF);
	void MutateAddConnection();
	void MutateAddNode();
	void MutateRemoveNode();

	int GetCurrentNodeID();
	int GetInputsCount();
	int GetOutputsCount();
};
