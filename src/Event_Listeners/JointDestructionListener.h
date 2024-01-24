#include <Box2D/Box2D.h>

class JointDestructionListener : public b2DestructionListener {
    public:
		void SayGoodbye(b2Joint* joint);
		void SayGoodbye(b2Fixture* fixture);
};
