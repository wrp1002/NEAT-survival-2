#include "Camera.h"

#include <allegro5/transformations.h>
#include <box2d/b2_math.h>

#include "../Globals.h"
#include "../UserInput.h"


namespace Camera {
	//weak_ptr<Object> Camera::followObject;
	ALLEGRO_TRANSFORM transform;
	b2Vec2 pos;
	float zoom;
	float zoomFactor;
	float minZoom;


	void Init() {
		pos = b2Vec2(-Globals::SCREEN_WIDTH / 2.0, -Globals::SCREEN_HEIGHT / 2.0);
		zoom = 1.0;
		zoomFactor = 0.2;
		minZoom = 0.15;
	}

	void UpdateTransform() {
		//if (followObject.expired() && GameRules::IsRuleEnabled("FollowRandomAgent"))
		//	FollowObject(dynamic_pointer_cast<Object>(GameManager::GetRandomAgent()));

		al_identity_transform(&transform);

		//al_translate_transform(&t, -followObject->GetPos().x, -followObject->GetPos().y);

		b2Vec2 cameraPos = CalculatePos();

		al_translate_transform(&transform, -cameraPos.x - Globals::SCREEN_WIDTH / 2, -cameraPos.y - Globals::SCREEN_HEIGHT / 2);

		al_scale_transform(&transform, zoom, zoom);
		al_translate_transform(&transform, -cameraPos.x, -cameraPos.y);

		//if (followObject)
		//	al_translate_transform(&t, followObject->GetPos().x, followObject->GetPos().y);
		//else
		al_translate_transform(&transform, cameraPos.x + Globals::SCREEN_WIDTH / 2, cameraPos.y + Globals::SCREEN_HEIGHT / 2);
	}

	void UpdateZoom(int diff) {
		if (diff == 0)
			return;

		if (zoom <= zoomFactor && diff < 0)
			return;

		b2Vec2 mouseDiff = zoomFactor * b2Vec2(Globals::SCREEN_WIDTH / 2, Globals::SCREEN_HEIGHT / 2);  // -UserInput::mousePos;

		if (diff > 0) {
			ZoomIn();
		}
		else {
			ZoomOut();
			mouseDiff *= -1;
		}

		//pos += mouseDiff;// *zoomFactor;

	}

	void ZoomIn() {
		zoom += zoomFactor;
	}

	void ZoomOut() {
		zoom -= zoomFactor;
		if (zoom < minZoom)
			zoom = minZoom;
	}

	b2Vec2 CalculatePos() {
		/*
		if (!followObject.expired()) {
			pos = followObject.lock()->GetPos() - b2Vec2(Globals::screenWidth, Globals::screenHeight) / 2;
			return pos;
		}
		else {
			*/
			if (!UserInput::isDragging)
				return pos;

			b2Vec2 calculatedPos = pos;
			calculatedPos = pos + (1 / zoom) * (UserInput::dragStartPos - UserInput::mousePos);
			return calculatedPos;
		//}
	}

	b2Vec2 ScreenPos2WorldPos(b2Vec2 screenPos) {
		return b2Vec2(
			pos.x + Globals::SCREEN_WIDTH  / 2.0 - Globals::SCREEN_WIDTH  / 2.0 / zoom + screenPos.x / zoom,
			pos.y + Globals::SCREEN_HEIGHT / 2.0 - Globals::SCREEN_HEIGHT / 2.0 / zoom + screenPos.y / zoom
		);
	}
}






//void Camera::FollowObject(weak_ptr<Object> obj) {
//	followObject = obj;
//}





