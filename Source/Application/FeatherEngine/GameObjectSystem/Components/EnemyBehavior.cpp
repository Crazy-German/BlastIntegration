#include "FeatherEngine.pch.h"
#include "EnemyBehavior.h"

#include "AssetManagement/AssetManager.h"
#include "GraphicsEngine/GfxCmd_RenderEnemy.h"
#include "AssetManagement/AssetTypes/MaterialAsset.h"
#include "GameObjectSystem/Components/MeshComponent.h"
#include "Transform.h"
#include "GameObjectSystem/Components/Animations/Animator.h"
#include "MainSingleton.h"
#include <Interface/InterfaceManager.h>

EnemyStats EnemyBehavior::ourEnemyStats;

EnemyBehavior::EnemyBehavior()
{
	myHealth = 1.0f;
	myState = eEnemyState::Idle;
	mySpeed = 0.0f;
	myMoveDirection = CommonUtilities::Vector3f();
	myDamage = 0;
	myAttackWindUp = 0.0f;
	myAttackCoolDown = 1.0f;
	myIsBusy = false;
	myBusyTimer = 0;
	myAttackRange = 100.0f;
	myIsElite = false;
	myInvincibilityTimer = 0;
	myOnHitInvincibility = 0.4f;
	myHealing = 15.0f;
	myCurrentNode = INT_MAX;
	myMaxHealth = myHealth;

	myDisperseDistance = 200.0f;
	myDisperseForce = 1000.0f;
}

EnemyBehavior::~EnemyBehavior()
{
}

void EnemyBehavior::Initialize(float aHealthValue, eEnemyState aState)
{
	myHealth = aHealthValue;
	myState = aState;
}

void EnemyBehavior::Update(const float& aDeltaTime)
{
	if (myBusyTimer > 0)
	{
		myBusyTimer -= aDeltaTime;
	}
	if (myInvincibilityTimer > 0)
	{
		myInvincibilityTimer -= aDeltaTime;
	}
	
	if (GetGameObject()->GetComponent<Transform>() != nullptr && GetGameObject()->GetComponent<Transform>()->GetEnabled())
	{
		if (myCurrentNode != -1)
		{
			//CommonUtilities::Vector3f position = GetGameObject()->GetComponent<Transform>()->GetPosition();
			//position.y = MainSingleton::Get().GetMeshNavigator().GetHeight({ position.x, position.z }, myCurrentNode);
			//if (position.y != 0)
			//{
			//	GetGameObject()->GetComponent<Transform>()->SetPosition(position);
			//}
		}
	}
}

void EnemyBehavior::Attack()
{
	if (myIsBusy || myState == eEnemyState::Attack)
	{
		return;
	}
	myState = eEnemyState::Attack;
	myIsBusy = true;
	myBusyTimer = myAttackWindUp;
}

void EnemyBehavior::Move(CommonUtilities::Vector3f aPosition)
{
	if (myIsBusy || myState == eEnemyState::Attack)
	{
		return;
	}
	myMoveDirection = CommonUtilities::Vector3f(aPosition.x, 0, aPosition.z) - CommonUtilities::Vector3f(GetGameObject()->GetComponent<Transform>()->GetPosition().x, 0, GetGameObject()->GetComponent<Transform>()->GetPosition().z);
	myMoveDirection.Normalize();

	myState = eEnemyState::TakePosition;

	Face(myMoveDirection);
}

void EnemyBehavior::Notice()
{
}

CU::Vector4f EnemyBehavior::GetID()
{
	return myID;
}

void EnemyBehavior::Render()
{
	//Needs to call renderer instead
	//GraphicsCommandList::Get().Enqueue<GfxCmd_RenderEnemy>(GetGameObject()->GetComponent<MeshComponent>()->GetMesh(), GetGameObject()->GetComponent<Transform>()->GetMatrixScale(), GetGameObject()->GetComponent<MeshComponent>()->GetMaterials(), myID, CU::Vector2f());
}

eEnemyState EnemyBehavior::GetState()
{
	return myState;
}

void EnemyBehavior::SetState(const eEnemyState& aState)
{
	myState = aState;
}

float EnemyBehavior::GetHealth() const
{
	return myHealth;
}

float EnemyBehavior::GetFractionalHealth() const
{
	return myHealth / myMaxHealth;
}

EnemyStats EnemyBehavior::GetEnemyStats()
{
	return ourEnemyStats;
}

void EnemyBehavior::MakeElite()
{
	myIsElite = true;
}

void EnemyBehavior::LoadEnemyStats(const std::string& aPath)
{
	GaiJsonParser parse;
	json mydata = parse.GetFileContents(aPath);

	ourEnemyStats.AngerEnemyHealth = mydata.at("AngerEnemyHealth");
	ourEnemyStats.AngerEnemySpeed = mydata.at("AngerEnemySpeed");
	ourEnemyStats.AngerEnemyAttackRange = mydata.at("AngerEnemyAttackRange");
	ourEnemyStats.AngerEnemyDamage = mydata.at("AngerEnemyDamage");
	ourEnemyStats.AngerEnemyAggroRange = mydata.at("AngerEnemyAggroRange");
	ourEnemyStats.AngerEnemyCooldown = mydata.at("AngerEnemyCooldown");

	ourEnemyStats.EliteAngerEnemyHealth = mydata.at("EliteAngerEnemyHealth");
	ourEnemyStats.EliteAngerEnemySpeed = mydata.at("EliteAngerEnemySpeed");
	ourEnemyStats.EliteAngerEnemyAttackRange = mydata.at("EliteAngerEnemyAttackRange");
	ourEnemyStats.EliteAngerEnemyDamage = mydata.at("EliteAngerEnemyDamage");
	ourEnemyStats.EliteAngerEnemyAggroRange = mydata.at("EliteAngerEnemyAggroRange");
	ourEnemyStats.EliteAngerEnemyCooldown = mydata.at("EliteAngerEnemyCooldown");

	ourEnemyStats.MinionAngerEnemyHealth = mydata.at("MinionAngerEnemyHealth");
	ourEnemyStats.MinionAngerEnemySpeed = mydata.at("MinionAngerEnemySpeed");
	ourEnemyStats.MinionAngerEnemyAttackRange = mydata.at("MinionAngerEnemyAttackRange");
	ourEnemyStats.MinionAngerEnemyDamage = mydata.at("MinionAngerEnemyDamage");
	ourEnemyStats.MinionAngerEnemyAggroRange = mydata.at("MinionAngerEnemyAggroRange");
	ourEnemyStats.MinionAngerEnemyCooldown = mydata.at("MinionAngerEnemyCooldown");

	ourEnemyStats.SadnessEnemyHealth = mydata.at("SadnessEnemyHealth");
	ourEnemyStats.SadnessEnemySpeed = mydata.at("SadnessEnemySpeed");
	ourEnemyStats.SadnessEnemyAttackRange = mydata.at("SadnessEnemyAttackRange");
	ourEnemyStats.SadnessEnemyDamage = mydata.at("SadnessEnemyDamage");
	ourEnemyStats.SadnessEnemyAggroRange = mydata.at("SadnessEnemyAggroRange");
	ourEnemyStats.SadnessProjectileSpeed = mydata.at("SadnessProjectileSpeed");
	ourEnemyStats.SadnessProjectileAmount = mydata.at("SadnessProjectileAmount");

	ourEnemyStats.EliteSadnessEnemyHealth = mydata.at("EliteSadnessEnemyHealth");
	ourEnemyStats.EliteSadnessEnemySpeed = mydata.at("EliteSadnessEnemySpeed");
	ourEnemyStats.EliteSadnessEnemyAttackRange = mydata.at("EliteSadnessEnemyAttackRange");
	ourEnemyStats.EliteSadnessEnemyDamage = mydata.at("EliteSadnessEnemyDamage");
	ourEnemyStats.EliteSadnessEnemyAggroRange = mydata.at("EliteSadnessEnemyAggroRange");
	ourEnemyStats.EliteSadnessProjectileSpeed = mydata.at("EliteSadnessProjectileSpeed");
	ourEnemyStats.EliteSadnessProjectileAmount = mydata.at("EliteSadnessProjectileAmount");

	ourEnemyStats.AnxietyEnemyHealth = mydata.at("AnxietyEnemyHealth");
	ourEnemyStats.AnxietyEnemySpeed = mydata.at("AnxietyEnemySpeed");
	ourEnemyStats.AnxietyEnemyAttackRange = mydata.at("AnxietyEnemyAttackRange");
	ourEnemyStats.AnxietyEnemyAggroRange = mydata.at("AnxietyEnemyAggroRange");
	ourEnemyStats.AnxietySpawnAmount = mydata.at("AnxietySpawnAmount");
	ourEnemyStats.AnxietySpawnCooldown = mydata.at("AnxietySpawnCooldown");

	ourEnemyStats.EliteAnxietyEnemyHealth = mydata.at("EliteAnxietyEnemyHealth");
	ourEnemyStats.EliteAnxietyEnemySpeed = mydata.at("EliteAnxietyEnemySpeed");
	ourEnemyStats.EliteAnxietyEnemyAttackRange = mydata.at("EliteAnxietyEnemyAttackRange");
	ourEnemyStats.EliteAnxietyEnemyAggroRange = mydata.at("EliteAnxietyEnemyAggroRange");
	ourEnemyStats.EliteAnxietySpawnAmount = mydata.at("EliteAnxietySpawnAmount");
	ourEnemyStats.EliteAnxietySpawnCooldown = mydata.at("EliteAnxietySpawnCooldown");
}

void EnemyBehavior::UpdateRRSound(EnemySoundID anID)
{
	switch (anID)
	{
	case EnemySoundID::Hurt:
		myCurrentHurtSound = (static_cast<unsigned>(myCurrentHurtSound)) < static_cast<unsigned>(EnemySoundID::Hurt3) ?
			static_cast<EnemySoundID>(static_cast<unsigned>(myCurrentHurtSound) + 1) : EnemySoundID::Hurt;
		break;
	case EnemySoundID::Death:
		myCurrentDeathSound = (static_cast<unsigned>(myCurrentDeathSound)) < static_cast<unsigned>(EnemySoundID::Death3) ?
			static_cast<EnemySoundID>(static_cast<unsigned>(myCurrentDeathSound) + 1) : EnemySoundID::Death;
		break;
	case EnemySoundID::BossProjectile:
		myCurrentProjectileSound = (static_cast<unsigned>(myCurrentProjectileSound)) < static_cast<unsigned>(EnemySoundID::BossProjectile3) ?
			static_cast<EnemySoundID>(static_cast<unsigned>(myCurrentProjectileSound) + 1) : EnemySoundID::BossProjectile;
		break;

	default:
		break;
	}
}

void EnemyBehavior::Face(const CommonUtilities::Vector3f& aDirection)
{
	float yRotation = atan2(aDirection.x, aDirection.z);
	CU::Vector3f rotation = { 0, yRotation, 0 };
	GetGameObject()->GetComponent<Transform>()->SetRotation(rotation);
}

void EnemyBehavior::ChangeAnimation(const std::string& aAnimationName)
{
	GetGameObject()->GetComponent<Animator>()->PlayAnimation(aAnimationName);
	/*GetGameObject()->GetComponent<Animator>()->ForceResetAnimation();
	GetGameObject()->GetComponent<Animator>()->PlayAnimation(aAnimationName);*/
	myCurrentAnimation = aAnimationName;
}

void EnemyBehavior::Put(Letter aLetter)
{
	if (aLetter.myType == eLetterRegister::NavMeshSwitch)
	{
		if (GetGameObject()->GetComponent<Transform>() != nullptr)
		{
			//CommonUtilities::Vector3f position = GetGameObject()->GetComponent<Transform>()->GetPosition();
			//position.y = MainSingleton::Get().GetMeshNavigator().GetHeight({ position.x, position.z }, myCurrentNode);
			//if (position.y != 0)
			//{
			//	GetGameObject()->GetComponent<Transform>()->SetPosition(position);
			//}
		}
	}
}

void EnemyBehavior::SetCurrentNode(const unsigned int anId)
{
	myCurrentNode = anId;
}

unsigned int EnemyBehavior::GetCurrectNode() const
{
	return myCurrentNode;
}

void EnemyBehavior::Disperse(const float& aDeltaTime, const CommonUtilities::Vector3f& aPosition)
{
	aDeltaTime;
	if (myIsBusy == false)
	{
		CommonUtilities::Vector3f toPosition = (GetGameObject()->GetComponent<Transform>()->GetPosition() - aPosition);
		if (toPosition.Length() < myDisperseDistance)
		{
			//const CommonUtilities::Vector3f forceToAdd = toPosition.GetNormalized() * (((myDisperseDistance - toPosition.Length()) / myDisperseDistance) * myDisperseForce * aDeltaTime);
			//const CommonUtilities::Vector3f nextPosition = GetGameObject()->GetComponent<Transform>()->GetPosition() + forceToAdd;
			//if (MainSingleton::Get().GetMeshNavigator().IsInsideNavMesh(CommonUtilities::Vector2f(nextPosition.x, nextPosition.z)))
			//{
			//	GetGameObject()->GetComponent<Transform>()->AddPosition(forceToAdd);
			//}
		}
	}
}
