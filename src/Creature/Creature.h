#pragma once

#include "BodyPart.h"
#include <allegro5/transformations.h>
#include <box2d/b2_joint.h>
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/b2_world_callbacks.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class BodySegment;
class Joint;
class NEAT;

using namespace std;


class Creature : public std::enable_shared_from_this<Creature> {
	private:
		static const int extraInputCount = 10;
		static const int extraOutputCount = 10;
		static const int geneLength = 16;
		int baseInputs;
		int baseOutputs;
		bool alive;
		bool isPlayer;

		double health, maxHealth;
		double energy, maxEnergy;


		shared_ptr<NEAT> nn;
		b2Vec2 startingPos;
		string genes;
		vector<shared_ptr<BodyPart>> bodySegments;
		vector<shared_ptr<Joint>> joints;
		shared_ptr<BodySegment> head;

		struct CurrentGenes {
			int r = 100, g = 100, b = 100;
			int width = 50, height = 50;
			int shapeType = 0;
			int geneLength = 16;
			int angleOffset = 0;
			int childAngleGene = 0;
		};

	public:
		Creature(string genes, b2Vec2 pos);
		~Creature();
		void Init();

		void ApplyGenes(string genes);
		void ApplyGenes();
		void SetAsPlayer(bool val);

		void CreateHead(string gene, CurrentGenes &currentGenes, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID);
		void CreateBodySegment(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID);
		void CreateMouth(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID);
		void CreateCilium(string gene, CurrentGenes &currentGenes, vector<shared_ptr<BodyPart>> &parentObjects, unordered_map<int, vector<shared_ptr<BodyPart>>> &symmetryMap, int &symmetryID);


		void Update();
		void Draw();
		void ApplyForce(b2Vec2 force);

		void AddJoint(shared_ptr<Joint> newJoint);
		void DestroyAllJoints();
		void AddPart(shared_ptr<BodyPart> part);

		void TakeDamage(double amount);


		bool IsAlive();
		float decimalFromSubstring(string str, int wholeDigits, int decimalDigits);
		float GetNextGene(string &gene, int wholeDigits, int decimalDigits);
		vector<shared_ptr<BodyPart>> GetAllParts();

		shared_ptr<NEAT> GetNN();


		// player control functions
		void SetBiting(bool val);
};