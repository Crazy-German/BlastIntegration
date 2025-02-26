#pragma once
#include "Component.h"
#include <array>
#include "CommonUtilities/Intersection.hpp"
#include "CommonUtilities/Input.h"
#include <unordered_map>
#include <filesystem>
#include <CommonUtilities/Vector.hpp>

enum class eEnemyState
{
	Idle,
	Alert,
	TakePosition,
	Attack,
	Dead,
	Stun
};

enum class EnemySoundID
{
	FootStep,
	Death,
	Death2,
	Death3,
	Hurt,
	Hurt2,
	Hurt3,
	Spawn,
	Engage,
	Attack,
	AngerAttack,
	SadnessLaunchProjectile,
	AnxietySpawnEnemies,
	BossIdle,
	BossSummonEnemies,
	BossSpawnEnemy,
	BossSwipe,
	BossSpawnProjectiles,
	BossHeadTwist,
	BossProjectile,
	BossProjectile2,
	BossProjectile3,
	BossTransition1,
	BossTransition2,
	BossTransitionFinal,
};

struct EnemyStats
{
	float AngerEnemyHealth;
	float AngerEnemySpeed;
	float AngerEnemyAttackRange;
	float AngerEnemyDamage;
	float AngerEnemyAggroRange;
	float AngerEnemyCooldown;

	float EliteAngerEnemyHealth;
	float EliteAngerEnemySpeed;
	float EliteAngerEnemyAttackRange;
	float EliteAngerEnemyDamage;
	float EliteAngerEnemyAggroRange;
	float EliteAngerEnemyCooldown;

	float MinionAngerEnemyHealth;
	float MinionAngerEnemySpeed;
	float MinionAngerEnemyAttackRange;
	float MinionAngerEnemyDamage;
	float MinionAngerEnemyAggroRange;
	float MinionAngerEnemyCooldown;

	float SadnessEnemyHealth;
	float SadnessEnemySpeed;
	float SadnessEnemyAttackRange;
	float SadnessEnemyDamage;
	float SadnessEnemyAggroRange;
	float SadnessProjectileSpeed;
	int SadnessProjectileAmount;

	float EliteSadnessEnemyHealth;
	float EliteSadnessEnemySpeed;
	float EliteSadnessEnemyAttackRange;
	float EliteSadnessEnemyDamage;
	float EliteSadnessEnemyAggroRange;
	float EliteSadnessProjectileSpeed;
	int EliteSadnessProjectileAmount;

	float AnxietyEnemyHealth;
	float AnxietyEnemySpeed;
	float AnxietyEnemyAttackRange;
	float AnxietyEnemyAggroRange;
	float AnxietySpawnCooldown;
	int AnxietySpawnAmount;

	float EliteAnxietyEnemyHealth;
	float EliteAnxietyEnemySpeed;
	float EliteAnxietyEnemyAttackRange;
	float EliteAnxietyEnemyAggroRange;
	float EliteAnxietySpawnCooldown;
	int EliteAnxietySpawnAmount;
};

#define ENEMY_COMPONENT 400
class EnemyBehavior : public Component
{
public:
	EnemyBehavior();
	~EnemyBehavior() override;
	virtual void Initialize(float aHealthValue, eEnemyState aState);
	void Update(const float& aDeltaTime) override;
	virtual void Attack();
	virtual void Move(CommonUtilities::Vector3f aPosition);
	virtual void Notice();
	CU::Vector4f GetID();
	void Render() override;
	eEnemyState GetState();
	void SetState(const eEnemyState& aState);
	float GetHealth() const;
	float GetFractionalHealth() const;
	static EnemyStats GetEnemyStats();
	void MakeElite();
	void ChangeAnimation(const std::string& aAnimationName);
	void Put(Letter aLetter) override;
	void SetCurrentNode(const unsigned int anId);
	unsigned int GetCurrectNode() const;
	 
	void Disperse(const float& aDeltaTime, const CommonUtilities::Vector3f& aPosition);

	static void LoadEnemyStats(const std::string& aPath);
protected:
	void UpdateRRSound(EnemySoundID anID);
	void Face(const CommonUtilities::Vector3f& aDirection);
	bool myIsBusy;
	float myBusyTimer;
	float myAttackRange;
	float myAttackWindUp;
	float myAttackCoolDown;
	CU::Vector4f myID;
	float myHealth;
	float myMaxHealth;
	eEnemyState myState;
	float mySpeed;
	float myDamage;
	CommonUtilities::Vector3f myMoveDirection;
	std::string myCurrentAnimation;
	unsigned int myCurrentNode;
	std::unordered_map<EnemySoundID, std::filesystem::path> mySoundIDToPath;
	EnemySoundID myCurrentHurtSound = EnemySoundID::Hurt;
	EnemySoundID myCurrentDeathSound = EnemySoundID::Death;
	EnemySoundID myCurrentProjectileSound = EnemySoundID::BossProjectile;
	static EnemyStats ourEnemyStats;
	bool myIsElite;
	bool myHasActivatedHealthBar = false;
	float myInvincibilityTimer;
	float myOnHitInvincibility;
	float myHealing;

	float myDisperseDistance;
	float myDisperseForce;
};