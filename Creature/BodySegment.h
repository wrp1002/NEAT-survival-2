#pragma once

#include <allegro5/allegro5.h>

#include <box2d/box2d.h>

#include <vector>

using namespace std;


class BodySegment {
    private:

    public:
        b2Body *body;
        int size;
        int angleOnBody;
        int angleFromBody;
        vector<BodySegment *> children;
        ALLEGRO_COLOR color;

        BodySegment(b2World &world, b2Vec2 pos, int size, ALLEGRO_COLOR color) {
            this->angleOnBody = 0;
            this->angleFromBody = 0;
            this->size = size;
            this->color = color;

            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = pos;
            body = world.CreateBody(&bodyDef);

            b2CircleShape circle;
            circle.m_radius = pixelsToMeters(size);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circle;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 0.5f;

            body->CreateFixture(&fixtureDef);
        }

        BodySegment(b2World &world, BodySegment &parent, int angleOnBody, int angleFromBody, int size, ALLEGRO_COLOR color) {
            this->angleOnBody = angleOnBody;
            this->angleFromBody = angleFromBody;
            this->size = size;
            this->color = color;


            b2BodyDef bodyDef;
            bodyDef.type = b2_dynamicBody;

            b2Vec2 parentPos = parent.body->GetPosition();
            float angleF = angleOnBody * M_PI / 180;
            float distance = parent.size + size - 5;
            bodyDef.position = parentPos + pixelsToMeters(cos(angleF) * distance, sin(angleF) * distance);
            body = world.CreateBody(&bodyDef);


            b2CircleShape circle;
            circle.m_radius = pixelsToMeters(size);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circle;
            fixtureDef.density = 1.0f;
            fixtureDef.friction = 0.3f;
            fixtureDef.restitution = 0.5f;

            body->CreateFixture(&fixtureDef);


            // joint together
            b2RevoluteJointDef jointDef;
            b2Vec2 jointPos = parentPos + pixelsToMeters(cos(angleF) * parent.size, sin(angleF) * parent.size);
            jointDef.Initialize(body, parent.body, jointPos);
            jointDef.lowerAngle = -0.5f * b2_pi;
            jointDef.upperAngle = 0.5f * b2_pi;
            jointDef.enableLimit = true;
            jointDef.maxMotorTorque = 2.0f;
            jointDef.motorSpeed = 0.0f;
            jointDef.enableMotor = false;
            jointDef.collideConnected = false;

            b2RevoluteJoint *joint = (b2RevoluteJoint *)world.CreateJoint(&jointDef);
            joints.push_back(joint);

        }

        void AddChild(BodySegment *child) {
            children.push_back(child);
        }

        bool childAngleValid(int angle) {
            for (auto child : children) {
                if (child->angleOnBody == angle)
                    return false;
            }
            return true;
        }

        void Draw() {
            for (auto child : children) {
                child->Draw();
            }

            float angle = body->GetAngle();
            b2Vec2 pos = metersToPixels(body->GetPosition());

            ALLEGRO_TRANSFORM t;
            al_identity_transform(&t);

            al_rotate_transform(&t, angle);
            al_translate_transform(&t, pos.x, pos.y);
            al_use_transform(&t);

            al_draw_filled_circle(0, 0, size, color);

            al_identity_transform(&t);
            al_use_transform(&t);
        }

};