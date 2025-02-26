#include <map>
#include <unordered_map>
#include <vector>

#include "Collision/Collider.h"
#include "CommonUtilities/InputObserver.h"
#include "GameObjectSystem/Components/ColliderComponent.h"
#include "GameObjectSystem/Components/RigidBodyComponent.h"
#include "Physcics/ContactHandler.h"

class CollisionManager : public InputObserver, public ContactHandler
{
public:
	static CollisionManager* Get();
	void AddCollider(ColliderComponent* aCollider);
	void AddRigidBody(ColliderComponentBase* aRigidBody);
	void Update(float aDeltaTime);
	void Collision(ColliderComponent* aCollider);
	void CleanUp();
	void RecieveEvent(const ActionEvent& anEvent) override;
private:
	CollisionManager();
	~CollisionManager();

public:
	// These are callback functions do not call manually
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs,
		uint32_t numPairs) override; //Callback don´t call manually
	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;//Callback don´t call manually
	void onContactModify(PxContactModifyPair* const pairs, PxU32 count) override;//Callback don´t call manually

private:
	bool myIsDebugRendering = false;
	std::vector<std::vector<ColliderComponent*>> myColliders;
	std::unordered_map<CollisionLayer, size_t> myModeToIndex;
	std::unordered_map<CollisionLayer, std::vector<CollisionLayer>> myCollisionMap;
	std::unordered_map<uint32_t,ColliderComponentBase*> myRigidBodies;
	//std::map<CollisionMode, std::vector<ColliderComponent* >> myColliders;
};
