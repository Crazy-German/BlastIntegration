#include "FeatherEngine.pch.h"
#include "ColliderComponent.h"

#include "GraphicsEngine/GfxCmd_DrawDebugLines.h"
#include "MainSingleton.h"

void ColliderComponent::SetPosition(const Vec3 aPosition)
{
	myTransformOffset.x = aPosition.x;
	myTransformOffset.y = aPosition.y;
	myTransformOffset.z = aPosition.z;
}

const CollisionLayer& ColliderComponent::GetLayer() const
{
	return myLayer;
}

const float& ColliderComponent::GetValue() const
{
	return myValue;
}

void ColliderComponent::SetValue(float aValue)
{
	myValue = aValue;
}

void ColliderComponent::SetEventType(InternalEvent aEventType)
{
	myEventType = aEventType;
}

InternalEvent ColliderComponent::GetEventType() const
{
	return myEventType;
}

void ColliderComponent::Render()
{
}

void ColliderComponent::DebugRender()
{
	if(!myIsEnabled)
	{
		return;
	}
	CommonUtilities::Vector4f pos = myTransformOffset*myGameObject->GetComponent<Transform>()->GetMatrix();
	CommonUtilities::Matrix4x4f matrix = myGameObject->GetComponent<Transform>()->GetMatrix();
	matrix.SetTranslation(pos.ToVector3());
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_DrawDebugLines>(myDebugMesh,matrix, false, DebugColor::Orange, "Collider");
}
