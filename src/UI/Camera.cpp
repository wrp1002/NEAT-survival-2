#include "Camera.h"

#include <Box2D/Common/b2Math.h>
#include <allegro5/transformations.h>
#include <Box2D/Box2D.h>

#include "../Globals.h"
#include "../Input/UserInput.h"


namespace Camera {
	weak_ptr<Object> followedObject;
	ALLEGRO_TRANSFORM transform;
	b2Vec2 pos;
	float zoom;
	float zoomFactor;
	float minZoom;


	void Init() {
		pos = b2Vec2(-Globals::SCREEN_WIDTH / 2.0, -Globals::SCREEN_HEIGHT / 2.0);
		zoom = 0.1;
		zoomFactor = 1.2;
		minZoom = 0.03;
	}

	void UpdateTransform() {
		b2Vec2 cameraPos = CalculatePos();

		al_identity_transform(&transform);
		al_translate_transform(&transform, -cameraPos.x - Globals::SCREEN_WIDTH / 2.0, -cameraPos.y - Globals::SCREEN_HEIGHT / 2.0);
		al_scale_transform(&transform, zoom, zoom);
		al_translate_transform(&transform, -cameraPos.x, -cameraPos.y);
		al_translate_transform(&transform, cameraPos.x + Globals::SCREEN_WIDTH / 2.0, cameraPos.y + Globals::SCREEN_HEIGHT / 2.0);
	}

	void UpdateZoom(int diff) {
		if (diff == 0)
			return;

		b2Vec2 mouseWorldBefore = ScreenPos2WorldPos(UserInput::mousePos);

		if (diff > 0)
			ZoomIn();
		else
			ZoomOut();

		b2Vec2 mouseWorldAfter = ScreenPos2WorldPos(UserInput::mousePos);

		// Move camera so the point under the mouse stays fixed
		pos += mouseWorldBefore - mouseWorldAfter;
	}

	void ZoomIn() {
		zoom *= zoomFactor;
	}

	void ZoomOut() {
		zoom /= zoomFactor;
		if (zoom < minZoom)
			zoom = minZoom;
	}

	b2Vec2 CalculatePos() {
		if (GameRules::IsRuleEnabled(GameRules::RuleName::FOLLOW_RANDOM_AGENT)) {
			bool followNew = true;

			if (auto objPtr = Camera::followedObject.lock()) {
				if (auto bodyPartPtr = dynamic_pointer_cast<BodyPart>(objPtr)) {
					followNew = bodyPartPtr->GetParentCreature().expired();
				}
				else {
					followNew = false;
				}
			}

			if (followNew) {
				auto randomCreature = GameManager::GetRandomExistingCreature();
				if (randomCreature) {
					if (auto followObj = randomCreature->GetHead().lock()) {
						Camera::FollowObject(followObj);
						InfoDisplay::SelectObject(followObj);
					}
				}
			}
		}

		if (shared_ptr<Object> object = followedObject.lock()) {
			pos = object->GetPosPX()- b2Vec2(Globals::SCREEN_WIDTH / 2.0, Globals::SCREEN_HEIGHT / 2.0);
			return pos;
		}
		else {
			if (!UserInput::isDragging)
				return pos;

			b2Vec2 calculatedPos = pos;
			calculatedPos = pos + (1 / zoom) * (UserInput::dragStartPos - UserInput::mousePos);
			return calculatedPos;
		}
	}

	b2Vec2 ScreenPos2WorldPos(b2Vec2 screenPos) {
		return b2Vec2(
			pos.x + Globals::SCREEN_WIDTH  / 2.0 - Globals::SCREEN_WIDTH  / 2.0 / zoom + screenPos.x / zoom,
			pos.y + Globals::SCREEN_HEIGHT / 2.0 - Globals::SCREEN_HEIGHT / 2.0 / zoom + screenPos.y / zoom
		);
	}

	bool ShouldDrawObject(b2Vec2 objectPos) {
		b2Vec2 cameraPos = CalculatePos();

		return (
			objectPos.x > cameraPos.x - Globals::SCREEN_WIDTH / 2.0 / zoom &&
			objectPos.x < cameraPos.x + Globals::SCREEN_WIDTH + Globals::SCREEN_WIDTH / 2.0 / zoom &&
			objectPos.y > cameraPos.y - Globals::SCREEN_HEIGHT / 2.0 / zoom  &&
			objectPos.y < cameraPos.y + Globals::SCREEN_HEIGHT + Globals::SCREEN_HEIGHT / 2.0 / zoom
		);
	}

	void FollowObject(weak_ptr<Object> obj) {
		followedObject = obj;
	}
}









