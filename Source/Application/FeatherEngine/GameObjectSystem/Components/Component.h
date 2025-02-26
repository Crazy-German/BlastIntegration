#pragma once
#include "GameObjectSystem/PostMaster/PostBox.h"
#include <memory>

class ComponentSystem;
class GameObject;
class ComponentBank;


enum class InternalEvent
{
	eHit,
	eDamage,
	eTouchFound,
	eTouchLost,
	eScreamReached,
	eCustomEvent
};

struct InternalMSGData
{
	GameObject* aExternalGameObject;
	const char* aMSG;
};

struct InternalMSG
{
	InternalEvent myEvent;
	float myValue;
	InternalMSGData myData;
};



class Component : public PostBox
{
public:
	Component();
	virtual ~Component() = default;
	//This may never take input. Create a Init function for that.
	virtual void Awake();
	virtual void Update(const float& aDeltaTime);
	virtual void Render();
	virtual void DebugRender();
	virtual void Destroy();
	virtual bool GetMarkedDestroy() const;
	void Put(Letter aLetter) override;
	GameObject* GetGameObject() const;
	virtual void SetGameObject(std::shared_ptr<GameObject> aGameObject);
	bool GetEnabled() const;
	virtual void SetEnabled(const bool& aIsEnabled);
	virtual void RecieveInternal(InternalMSG aEvent);

	void SetShouldDebugRender(const bool aShouldDebugRender);
	bool& ShouldDebugRender();

	// Rekommenderar inte att pilla med IDn
	void SetId(const int& aId);
	int GetId() const;
	int GetTypeId();
	void SetTypeId(const int& aComponentId);

	Component*& GetLink(void* ComponentPointer);

protected:
	friend class ComponentSystem;
	friend class ComponentBank;
	friend class GameObject;
	GameObject* myGameObject;
	bool myIsEnabled;
	bool myIsMarkedForDestroy;
	int myId;
	int myTypeID;
	bool myShouldDebugRender = false;
private:
};

