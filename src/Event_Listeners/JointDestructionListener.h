#include <box2d/b2_joint.h>
#include <box2d/b2_world_callbacks.h>

class JointDestructionListener : public b2DestructionListener {
    public:
		void SayGoodbye(b2Joint* joint);
		void SayGoodbye(b2Fixture* fixture);
};
