#include "FeatherEngine.pch.h"
#include "CollisionManager.h"

#include "MainSingleton.h"
#include "Collision/EPA.h"
#include "Collision/GJK.h"
#include "GameObjectSystem/PostMaster/GameObjectPostMaster.h"
#include "Physcics/PhysicsEngine.h"
#include "Physcics/PhysXScene.h"

CollisionManager::CollisionManager()
{
	myCollisionMap.emplace(Static, std::vector<CollisionLayer>({}));
	myCollisionMap.emplace(Player, std::vector<CollisionLayer>({ Static, Enemy,Boss, Trigger, BossHurtBox }));
	myCollisionMap.emplace(Enemy, std::vector<CollisionLayer>({ Static, Player }));
	myCollisionMap.emplace(Boss, std::vector<CollisionLayer>({ Player, Enemy }));
	myCollisionMap.emplace(Trigger, std::vector<CollisionLayer>({}));
	myCollisionMap.emplace(BossHurtBox, std::vector<CollisionLayer>({}));

	myColliders.resize(6);
	myModeToIndex.emplace(Static, 0);
	myModeToIndex.emplace(Player, 1);
	myModeToIndex.emplace(Enemy, 2);
	myModeToIndex.emplace(Boss, 3);
	myModeToIndex.emplace(Trigger, 4);
	myModeToIndex.emplace(BossHurtBox, 5);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::EnableColliderDebug, this);
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::onContact(const physx::PxContactPairHeader& /*pairHeader*/, const physx::PxContactPair* pairs,
	uint32_t numPairs)
{
	for (uint32_t i = 0; i < numPairs; ++i)
        {
            const physx::PxContactPair& contactPair = pairs[i];
            PxActor* actor0 = contactPair.shapes[0]->getActor();
            //PxActor* actor1 = contactPair.shapes[1]->getActor();
            // Loop through all contacts within the pair
		
            for (uint32_t j = 0; j < contactPair.contactCount; ++j)
            {
				PxContactPairPoint* point = new PxContactPairPoint[contactPair.contactCount];
                contactPair.extractContacts(point, contactPair.contactCount);
				printf("Y-Normal: %.6f\n",point->normal.y);
            }
			InternalMSG msg;
			msg.myEvent = InternalEvent::eHit;
			myRigidBodies.at(static_cast<Squish::ActorUserData*>(actor0->userData)->aID)->GetGameObject()->SendInternal(msg);
        }
}

void CollisionManager::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	 for (uint32_t i = 0; i < count; ++i) {
		
            const physx::PxTriggerPair& pair = pairs[i];
            PxActor* actor0 = pair.triggerActor;
		int aActor0ID = static_cast<Squish::ActorUserData*>(actor0->userData)->aID;
		int aActor1ID = static_cast<Squish::ActorUserData*>(pair.otherActor->userData)->aID;
        // Check if the trigger shape was entered or exited
        if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
			InternalMSG msg;
			msg.myEvent = InternalEvent::eTouchFound;
			msg.myValue = static_cast<float>(aActor1ID);
			msg.myData.aExternalGameObject = myRigidBodies.at(aActor1ID)->GetGameObject();
			myRigidBodies.at(aActor0ID)->GetGameObject()->SendInternal(msg);
			msg.myValue = static_cast<float>(aActor0ID);;
			msg.myData.aExternalGameObject = myRigidBodies.at(aActor0ID)->GetGameObject();
			myRigidBodies.at(aActor1ID)->GetGameObject()->SendInternal(msg);
            // Handle trigger enter event
        } else if (pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            // Handle trigger exit event
			InternalMSG msg;
			msg.myEvent = InternalEvent::eTouchLost;
			msg.myValue = static_cast<float>(aActor1ID);
			msg.myData.aExternalGameObject = myRigidBodies.at(aActor1ID)->GetGameObject();
			myRigidBodies.at(aActor0ID)->GetGameObject()->SendInternal(msg);
			msg.myValue = static_cast<float>(aActor0ID);;
			msg.myData.aExternalGameObject = myRigidBodies.at(aActor0ID)->GetGameObject();
			myRigidBodies.at(aActor1ID)->GetGameObject()->SendInternal(msg);
        }
		if(pairs[i].status &physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			//still in trigger
		}
    }
}

void CollisionManager::onContactModify(PxContactModifyPair* const pairs, PxU32 count)
{
	ContactHandler::onContactModify(pairs, count);
}

CollisionManager* CollisionManager::Get()
{
	static CollisionManager collisionManager;

	return &collisionManager;
}

void CollisionManager::AddCollider(ColliderComponent* aCollider)
{
	myColliders.at(myModeToIndex.at(aCollider->GetLayer())).push_back(aCollider);
	
}

void CollisionManager::AddRigidBody(ColliderComponentBase* aRigidBody)
{
	RigidBodyComponent* body = dynamic_cast<RigidBodyComponent*>(aRigidBody);
	if(body != nullptr)
	{
		Squish::PhysicsEngine::Get()->GetScene()->AddActor(body);
	}
	myRigidBodies.emplace(aRigidBody->GetUserData()->aID, aRigidBody);
}

void CollisionManager::Update(float aDeltaTime)
{
	for(auto& vector : myColliders)
	{
		vector.resize(vector.size());
	}
	aDeltaTime;
	for (auto& [layer, toCheck] : myCollisionMap)
	{
		if(layer == Static || layer == Trigger)
		{
			continue;
		}
		for(auto check : toCheck)
		{
			int base = 0;
			for(int baseIndex = 0; baseIndex<myColliders.at(myModeToIndex[layer]).size(); baseIndex++)
			{
				ColliderComponent* baseCollider = myColliders.at(myModeToIndex[layer]).at(baseIndex);
				int active = 0;
				for (int activeIndexd = 0; activeIndexd<myColliders.at(myModeToIndex[check]).size(); activeIndexd++)
				{
					ColliderComponent* collider = myColliders.at(myModeToIndex[check]).at(activeIndexd);
					if (!baseCollider || collider == nullptr)
					{
						std::cout << "Null collider encountered!" << std::endl;
						continue;
					}
					if (baseCollider->GetEnabled() && collider->GetEnabled() && GJK(*collider->GetCollider(), *baseCollider->GetCollider()))
					{
						#ifdef _DEBUG
						std::cout << baseCollider->GetGameObject()->GetName() << " Colliding with " << collider->GetGameObject()->GetName() << std::endl;
						#endif
						//Send internal
						InternalMSG msg;
						msg.myEvent = baseCollider->GetEventType();
						msg.myValue = baseCollider->GetValue();
						collider->GetGameObject()->SendInternal(msg);
						msg.myEvent = collider->GetEventType();
						msg.myValue = collider->GetValue();
						baseCollider->GetGameObject()->SendInternal(msg);
					}
					active++;
				}
				base++;
			}
		}
	}
}

void CollisionManager::Collision(ColliderComponent* aCollider)
{
	CollisionLayer layer = aCollider->GetLayer();

	Simplex OutSimplex;
	CollisionPoints OutPoints;
	for (auto& toCheck : myCollisionMap[layer])
	{
		for (auto& collider : myColliders.at(toCheck))
		{
			if (GJK(*collider->GetCollider(), *aCollider->GetCollider()))
			{
#ifdef _DEBUG
				std::cout << aCollider->GetGameObject()->GetName() << " Colliding with " << collider->GetGameObject()->GetName() << std::endl;
#endif
				//Send internal
				InternalMSG msg;
				msg.myEvent = aCollider->GetEventType();
				msg.myValue = aCollider->GetValue();
				aCollider->GetGameObject()->SendInternal(msg);
			}
		}
	}
}

void CollisionManager::CleanUp()
{
	if(myColliders.empty())
	{
		return;
	}
	for(auto& [layer, index] : myModeToIndex)
	{
		for(int i = 0; i< myColliders.at(index).size(); i++)
		{
			if(myColliders[index][i]->GetMarkedDestroy())
			{
				myColliders.at(index).erase(myColliders.at(index).begin()+i);
				i--;
			}
		}
	}
}

void CollisionManager::RecieveEvent(const ActionEvent& anEvent)
{
	if(anEvent.Id == ActionEventID::EnableColliderDebug)
	{
		myIsDebugRendering = !myIsDebugRendering;
		for(auto& [layer, index] : myModeToIndex)
		{
			for(int i = 0; i< myColliders.at(index).size(); i++)
			{
				myColliders.at(index).at(i)->SetShouldDebugRender(myIsDebugRendering);
			}
		}
	}
}
