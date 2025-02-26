#include "PlayerComponent.h"

#include "CollisionHandling/CollisionManager.h"
#include "GameObjectSystem/Components/Transform.h"
#include "GameObjectSystem/GameObject.h"
#include <GraphicsEngine/GraphicsCommands.h>

#include "MainSingleton.h"
#include "GameObjectSystem/Components/ControllerComponent.h"
#include "GameObjectSystem/Components/MVCameraComponent.h"
#include "GameObjectSystem/Components/Animations/Animator.h"
#include <GameObjectSystem/PostMaster/GameObjectPostMaster.h>

#include "GameObjectSystem/Components/MeshComponent.h"
#include "GameObjectSystem\Components\AudioSourceComponent.h"
#include "Interface\InterfaceManager.h"
#include "GameObjectSystem\Components\RigidBodyComponent.h"
#include "Physcics/PhysicsEngine.h"
#include "Physcics/PhysXScene.h"

PlayerComponent::PlayerComponent()
{
	myMovementSpeed = 2500.f;
	myJumpLift = 15.f;
	myJumpStartImpulse = 30.f;
	myJumpLiftDuration = 0.5f;
	myCurrentJumpTime = 0;
	myCurrentChargeTime = 0.0f;
	myChargeTimeRequired = 2.0f;
	myScreamTimer = 0.0f;
	myScreamDuration = 0.0f;
	myMaxScreamChargeTime = 2.0f;
	myIsScreaming = false;
	myCamera = nullptr;
	myRigidBody = nullptr;
	myIsMoving = false;
	myIsGrounded = false;
	myIsBonked = false;
	myIsJumpHeld = false;
	myWasWalkingLastFrame = false;
	myIsSuperJumpUnlocked = true;
	myIsCharging = false;
	myIsCharged = false;

	myIsGlidingUnlocked = true;

	myAnimationState = ePlayerAnimationState::Idle;
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Awake()
{
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Jump, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Scream, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Charge, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::MoveForward, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::MoveLeft, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::MoveRight, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::MoveBackwards, this);

	myAnimationState = ePlayerAnimationState::Idle;

	AnimationFromState();

	myRigidBody = myGameObject->GetComponent<CharacterControllerComponent>();
	myGameObject->GetTransform()->SetRotation(CU::Vector3f(0, 0, 0));
}

void PlayerComponent::Destroy()
{
}

void PlayerComponent::Update(const float& aDeltaTime)
{
	Transform* transform = myGameObject->GetTransform();
	if (myIsChargingScream)
	{
		myScreamChargeTimer += aDeltaTime;
		if (myScreamChargeTimer > myMaxScreamChargeTime)
		{
			myIsChargingScream = false;
		}
	}
	if (!myIsChargingScream && myScreamChargeTimer > 0 && !myIsScreaming)
	{
		myIsScreaming = true;
		myScreamTimer = 0;
		myScreamDuration = myScreamChargeTimer + 0.3f;
		myScreamReach = 100.f;
		myAnimationState = ePlayerAnimationState::ShoutingLight;
		if (myScreamChargeTimer > myMaxScreamChargeTime * 0.5f)
		{
			myAnimationState = ePlayerAnimationState::ShoutingHeavy;
		}
		myScreamChargeTimer = 0;
		AnimationFromState();
	}


	if (myIsCharging)
	{
		if (myCurrentChargeTime <= 0)
		{
			myAnimationState = ePlayerAnimationState::Charging;
			AnimationFromState();
		}
		myCurrentChargeTime += aDeltaTime;
		myIsGrounded = true;
		if (myCurrentChargeTime > myChargeTimeRequired)
		{
			myIsCharged = true;
		}
	}
	else
	{
		myIsCharged = false;
		myCurrentChargeTime = 0;
	}

	//debugging space
	//myIsMoving = true;
	//myMovementVector.z += 1.0f;
	//debuggign space end



	CU::Vector3f rotationF;
	if (myIsMoving)
	{
		rotationF.y = atan2(myPhysicsData.velocity.GetNormalized().z, myPhysicsData.velocity.GetNormalized().x);
		SetLocalRotation(rotationF);
	}

	CU::Vector3f position = transform->GetPosition();
	if (position.y < -1000.f)
	{
		std::cout << "stop falling through the map!\n";
		myRigidBody->AddForce({ 0, 6000.f,0 });
		transform->SetPosition({ position.x, 5000.f, position.z });
	}
	if (myIsScreaming)
	{
		myScreamTimer += aDeltaTime;
		myScreamReach += 200 * aDeltaTime;
		Letter screamLetter;
		screamLetter.myType = eLetterRegister::Scream;
		ScreamStruct data;
		data.myRange = myScreamReach;
		data.myPosition = position;
		screamLetter.myData = &data;
		GameObjectPostMaster::Get()->Send(screamLetter);
		if (myScreamTimer > myScreamDuration)
		{
			myScreamChargeTimer = 0;
			myIsScreaming = false;
		}
	}

	if (myRigidBody->GetCollisionFlags() & physx::PxControllerCollisionFlag::eCOLLISION_DOWN && !(myRigidBody->GetCollisionFlags() & physx::PxControllerCollisionFlag::eCOLLISION_SIDES))
	{
		myPhysicsData.velocity.y = 0;
		myIsGrounded = true;
	}
	if (myRigidBody->GetCollisionFlags() & physx::PxControllerCollisionFlag::eCOLLISION_UP)
	{
		myPhysicsData.velocity.y *= -1;
		myIsBonked = true;
	}

	if (myIsCharging || myIsChargingScream)
	{
		myPhysicsData.ApplyNaturalForces(0.5f, aDeltaTime);
	}
	else
	{
		myPhysicsData.ApplyNaturalForces(1.0f, aDeltaTime);
	}

	myCurrentJumpTime += aDeltaTime;
	if (myIsMoving)
	{
		if (!myIsCharging && !myIsChargingScream)
		{
			if (!myWasWalkingLastFrame && myIsGrounded)
			{
				myAnimationState = ePlayerAnimationState::Walking;
				AnimationFromState();
				myWasWalkingLastFrame = true;
			}
			myMovementVector.Normalize();
			auto cameraForward = myCamera->GetTransform()->GetForwardVector();
			auto cameraRight = myCamera->GetTransform()->GetRightVector();
			CU::Vector3f moveRightDirr = CU::Vector3f(myMovementVector.x * myMovementSpeed * cameraRight * aDeltaTime);
			CU::Vector3f moveForwardsDirr = CU::Vector3f(myMovementVector.z * myMovementSpeed * cameraForward * aDeltaTime);
			moveRightDirr.y = 0;
			moveForwardsDirr.y = 0;

			myPhysicsData.AddForce(moveRightDirr);
			myPhysicsData.AddForce(moveForwardsDirr);

			CU::Vector3f flatSpeed = myPhysicsData.velocity;
			flatSpeed.y = 0;
			if (flatSpeed.Length() > myMaxSpeed)
			{
				flatSpeed.Normalize();
				flatSpeed *= myMaxSpeed;
				myPhysicsData.velocity.x = flatSpeed.x;
				myPhysicsData.velocity.z = flatSpeed.z;
			}

			//rotating the player model
			//Viktors kod
			std::string jointName = "progPelvis";
			auto animator = GetGameObject()->GetComponent<Animator>();
			auto meshComp = GetGameObject()->GetComponent<MeshComponent>();
			int jointIndex = meshComp->GetJoint(jointName);
			if (!meshComp->GetMesh()->GetSkeleton().Joints.empty())
			{
				auto& matrix = meshComp->GetMesh()->GetSkeleton().Joints[jointIndex].BindPoseInverse;
				CU::Vector2f xz = CU::Vector2f(myMovementVector.x, myMovementVector.z).GetNormalized();
				float rotation = atan2(xz.y, xz.x) ;
				matrix = CU::Matrix4x4f::CreateRotationAroundY(-rotation + 1.57f);
				animator->GetAnimationBuffer()->JointTransforms[jointIndex] = matrix;
			}


			// Harrys kod
			//auto speedNormalizer = myPhysicsData.velocity;
			//speedNormalizer.y = 0;
			//auto normalizedSpeed = speedNormalizer.GetNormalized();
			//float targetYRotation = atan2(normalizedSpeed.z, normalizedSpeed.x);
			//float currentYRotation = myGameObject->GetTransform()->GetRotationEuler().y * 0.0174533f + 3.14f;
			//if (abs(currentYRotation) > 100)
			//{
			//	myGameObject->GetTransform()->SetRotation(CU::Vector3f(0, 0, 0));
			//	currentYRotation = 3.14f;
			//}
			//float rotationDifference = currentYRotation - targetYRotation;
			//float netRotation = rotationDifference;
			//if (abs(rotationDifference) > 0.03f)
			//{
			//	if (rotationDifference > 3.14f)
			//	{
			//		netRotation = 0.03f;
			//	}
			//	else
			//	{
			//		netRotation = -0.03f;
			//	}
			//}

			//myGameObject->GetTransform()->AddRotationRadian(CU::Vector3f(0, netRotation, 0));

			//myCamera->GetComponent<MVCameraComponent>()->AddYRotation(-netRotation);
		}
	}

	if (myIsJumpHeld)
	{
		if (myIsCharged)
		{
			myCurrentJumpTime = 10.f;
			myCurrentChargeTime = 0;
			myIsCharged = false;
			myIsCharging = false;
			myIsGrounded = false;
			myWasWalkingLastFrame = false;
			CU::Vector3f SuperJumpVector;
			SuperJumpVector.y = mySuperJumpImpulse;
			myPhysicsData.AddForce(SuperJumpVector);
		}
		else
		{
			if (myIsGrounded)
			{
				myAnimationState = ePlayerAnimationState::JumpStart;
				AnimationFromState();

				myWasWalkingLastFrame = false;
				myPhysicsData.velocity.y = 0;
				myCurrentJumpTime = 0.0f;
				myIsGrounded = false;
				CU::Vector3f jumpStartVector;
				jumpStartVector.y = myJumpStartImpulse;
				myPhysicsData.AddForce(jumpStartVector);
				myIsCharging = false;
			}
			if (myCurrentJumpTime <= myJumpLiftDuration)
			{
				myIsCharging = false;
				myIsGrounded = false;
				CU::Vector3f jumpvector;
				jumpvector.y = myJumpLift * aDeltaTime;
				myPhysicsData.AddForce(jumpvector);
			}
		}
	}

	myMovementVector = { 0,0,0 };
	if (myIsJumpHeld && myIsGlidingUnlocked && myPhysicsData.velocity.y < myMaxGlideSpeed)
	{
		myPhysicsData.velocity.y = myMaxGlideSpeed;
	}
	if (myCurrentJumpTime > myJumpLiftDuration * 0.5f)
	{
		myIsJumpHeld = false;
	}
	CU::Vector3f flatVelocity = myPhysicsData.velocity;
	flatVelocity.y = 0;
	if (flatVelocity.LengthSqr() < 10.f)
	{
		myPhysicsData.Stop();
		myWasWalkingLastFrame = false;
		if (myIsGrounded && myIsMoving)
		{
			myAnimationState = ePlayerAnimationState::WalkStop;
			AnimationFromState();
		}
		myIsMoving = false;
	}
	else
	{
		myIsMoving = true;
	}

	myRigidBody->AddForce(myPhysicsData.velocity * aDeltaTime);
	RayCastBuffer out;
	if(Squish::PhysicsEngine::Get()->GetScene()->Raycast(myGameObject->GetTransform()->GetPosition(), {0,-1,0}, 200, out, PxFilterData(1<<1, 0,0,0), physx::PxQueryFlag::eSTATIC))
	{
		if(out.hasBlock)
		{
			std::cout <<  "Raycast hit" << out.Block.Point.y << std::endl;
		}
	}
	UpdateSounds();

	myIsCharging = false;
	myIsJumpHeld = false;
	myIsChargingScream = false;
}



void PlayerComponent::Render()
{
	MainSingleton::Get().GetRenderer().Enqueue<GfxCmd_SetMiscBuffer>(*MainSingleton::Get().GetMiscBuffer());
}

void PlayerComponent::Put(Letter aLetter)
{
	switch (aLetter.myType)
	{
	case eLetterRegister::LoadingLevel:
		Prepare();
		break;
	case eLetterRegister::Collision:
		break;
	default:
		break;
	}
}

void PlayerComponent::Prepare()
{
}



CU::Vector3f PlayerComponent::GetPosition()
{
	CU::Vector3f position = GetGameObject()->GetComponent<Transform>()->GetPosition();
	return position;
}

void PlayerComponent::SetCamera(GameObject& aCamera)
{
	myCamera = &aCamera;
}

void PlayerComponent::SetLocalRotation(const CU::Vector3f& aRotation)
{
	myRotation = aRotation;

	// joint name is pelvis
	std::string jointName = "pelvis";
	auto meshComp = myGameObject->GetComponent<MeshComponent>();
	int jointIndex = meshComp->GetJoint(jointName);
	if (jointIndex < 0)
	{
		printf("Player component SetLocalRotation joint not exist");
		return;
	}
	auto& joint = meshComp->GetMesh()->GetSkeletonRef().Joints[jointIndex].BindPoseInverse;
	joint = CU::Matrix4x4f::CreateRotationAroundX(myRotation.x);
	joint *= CU::Matrix4x4f::CreateRotationAroundY(myRotation.y);
	joint *= CU::Matrix4x4f::CreateRotationAroundZ(myRotation.z);
}

void PlayerComponent::AddLocalRotation(const CU::Vector3f& aDeltaRotation)
{
	myRotation += aDeltaRotation;

	SetLocalRotation(myRotation);
}

void PlayerComponent::RecieveEvent(const ActionEvent& anEvent)
{
	if (myGameObject->GetEnabled() == false)
	{
		return;
	}
	switch (anEvent.Id)
	{
	case ActionEventID::MoveForward:
		myIsMoving = true;
		myMovementVector.z += 1.0f;
		break;
	case ActionEventID::MoveLeft:
		myIsMoving = true;
		myMovementVector.x -= 1.0f;
		break;
	case ActionEventID::MoveRight:
		myIsMoving = true;
		myMovementVector.x += 1.0f;
		break;
	case ActionEventID::MoveBackwards:
		myIsMoving = true;
		myMovementVector.z -= 1.0f;
		break;
	case ActionEventID::Jump:
		Jump();
		break;
	case ActionEventID::Charge:
		if (myIsGrounded && myIsSuperJumpUnlocked)
		{
			myIsCharging = true;
		}
		break;
	case ActionEventID::Scream:
		Scream();
		break;
	default:
		break;
	}
}

void PlayerComponent::LoadSettings(const std::string& aPath)
{
	GaiJsonParser parse;
	json mydata = parse.GetFileContents(aPath);

	if (mydata.contains("JumpLift"))
	{
		myJumpLift = mydata.at("JumpLift");
	}
	if (mydata.contains("JumpStartForce"))
	{
		myJumpStartImpulse = mydata.at("JumpStartForce");
	}
	if (mydata.contains("SuperJumpForce"))
	{
		mySuperJumpImpulse = mydata.at("SuperJumpForce");
	}
	if (mydata.contains("ChargeTime"))
	{
		myChargeTimeRequired = mydata.at("ChargeTime");
	}
	if (mydata.contains("JumpLiftDuration"))
	{
		myJumpLiftDuration = mydata.at("JumpLiftDuration");
	}
	if (mydata.contains("MovementAcceleration"))
	{
		myMovementSpeed = mydata.at("MovementAcceleration");
	}
	if (mydata.contains("MaxSpeed"))
	{
		myMaxSpeed = mydata.at("MaxSpeed");
	}
	if (mydata.contains("Friction"))
	{
		myPhysicsData.frictionalValue = mydata.at("Friction");
	}
	if (mydata.contains("Gravity"))
	{
		myPhysicsData.gravity = mydata.at("Gravity");
	}
	if (mydata.contains("MaxFallSpeed"))
	{
		myPhysicsData.maxFallSpeed = mydata.at("MaxFallSpeed");
	}
	if (mydata.contains("MaxGlideSpeed"))
	{
		myMaxGlideSpeed = mydata.at("MaxGlideSpeed");
	}
}

void PlayerComponent::Init()
{
	auto spheres = myGameObject->GetComponents<SphereColliderComponent>();

	GameObjectPostMaster::Get()->Register(eLetterRegister::LoadingLevel, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Left_Click, this);
	MainSingleton::Get().GetInputMapper().Subscribe(ActionEventID::Walk, this);
	InitSounds();
}

void PlayerComponent::RecieveInternal(InternalMSG aEvent)
{
	if (aEvent.myEvent == InternalEvent::eHit)
	{
	}
}

void PlayerComponent::TurnAround()
{
	GetGameObject()->GetTransform()->AddRotation(CU::Vector3f(0, 180, 0));
}

void PlayerComponent::Scream()
{
	if (!myIsScreaming)
	{
		myAnimationState = ePlayerAnimationState::ChargingShout;
		AnimationFromState();
		myIsChargingScream = true;
		myScreamTimer = 0;
	}
}

void PlayerComponent::AnimationFromState()
{
	switch (myAnimationState) {
	case ePlayerAnimationState::Idle:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Idle");
		break;
	case ePlayerAnimationState::Walking:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Walk");
		break;
	case ePlayerAnimationState::JumpStart:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Jump");
		break;
	case ePlayerAnimationState::Falling:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Jump");
		break;
	case ePlayerAnimationState::Landing:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Land");
		break;
	case ePlayerAnimationState::WalkingUpSlope:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Climb Slope");
		break;
	case ePlayerAnimationState::WalkStop:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Walk Stop");
		break;
	case ePlayerAnimationState::ShoutingLight:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Shout");
		break;
	case ePlayerAnimationState::Slide:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Slide");
		break;
	case ePlayerAnimationState::GettingTape:
		myGameObject->GetComponent<Animator>()->PlayAnimation("Player Get Tape");
		break;
	}
	std::cout << "Animating player\n";
	//ensure animation resets properly
	/*if (!myIsMoving)
	{
		myGameObject->GetComponent<Animator>()->ForceResetAnimation();
	}*/

	//switch (myAnimationState) {
	//case ePlayerAnimationState::Idle:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Idle");
	//	break;
	//case ePlayerAnimationState::Walking:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Walk");
	//	break;
	//case ePlayerAnimationState::JumpStart:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Jump");
	//	break;
	//case ePlayerAnimationState::Falling:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Jump");
	//	break;
	//case ePlayerAnimationState::WalkingUpSlope:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Climb Slope");
	//	break;
	//case ePlayerAnimationState::WalkStop:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Walk Stop");
	//	break;
	//case ePlayerAnimationState::Shouting:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Shout");
	//	break;
	//case ePlayerAnimationState::Slide:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Slide");
	//	break;
	//case ePlayerAnimationState::GettingTape:
	//	myGameObject->GetComponent<Animator>()->PlayAnimation("Player Get Tape");
	//	break;
	//}
	//if (myAnimationState != ePlayerAnimationState::Idle && myGameObject->GetComponent<Animator>()->GetAnimatorState() == Animator::AnimatorState::Stopped)
	//{
	//	myAnimationState = ePlayerAnimationState::Idle;
	//	AnimationFromState();
	//}
}

void PlayerComponent::StopAnimation()
{
	myGameObject->GetComponent<Animator>()->StopAnimation();
}

void PlayerComponent::Jump()
{
	myIsJumpHeld = true;
}

void PlayerComponent::InitSounds()
{
	if (!GetGameObject()->GetComponent<AudioSourceComponent>())
	{
		return;
	}
	std::filesystem::path attack1 = "PlayerComboAttack1_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(attack1, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::ComboAttack1, attack1.stem() });
	}


	std::filesystem::path attack2 = "PlayerComboAttack2_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(attack2, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::ComboAttack2, attack2.stem() });
	}


	std::filesystem::path attack3 = "PlayerComboAttack3_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(attack3, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::ComboAttack3, attack3.stem() });
	}


	std::filesystem::path heavyAttack = "PlayerHeavyAttack_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(heavyAttack, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::HeavyAttack, heavyAttack.stem() });
	}


	std::filesystem::path jumpAttack = "PlayerJumpAttack_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(jumpAttack, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::JumpAttack, jumpAttack.stem() });
	}


	std::filesystem::path warCry = "PlayerWarcry_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(warCry, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::WarCry, warCry.stem() });
	}


	std::filesystem::path projectile = "PlayerProjectile_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(projectile, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::ProjectileAttack, projectile.stem() });
	}


	std::filesystem::path hurt = "PlayerHurt1_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(hurt, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Hurt, hurt.stem() });
	}


	std::filesystem::path hurt2 = "PlayerHurt2_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(hurt2, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Hurt2, hurt2.stem() });
	}


	std::filesystem::path hurt3 = "PlayerHurt3_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(hurt3, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Hurt3, hurt3.stem() });
	}


	std::filesystem::path death = "PlayerDeath1_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(death, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Death, death.stem() });
	}

	std::filesystem::path death2 = "PlayerDeath2_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(death2, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Death2, death2.stem() });
	}

	std::filesystem::path death3 = "PlayerDeath3_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(death3, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Death3, death3.stem() });
	}

	std::filesystem::path heal = "PlayerHeal_SFX3D.wav";
	if (GetGameObject()->GetComponent<AudioSourceComponent>()->AddSound(heal, SoundType::SFX3D, SoundPriority::High))
	{
		mySoundIDToPath.insert({ PlayerSoundID::Healing, heal.stem() });
	}
}

void PlayerComponent::UpdateSounds()
{
	CU::Vector3f position = GetGameObject()->GetTransform()->GetPosition();
	for (auto& ID : mySoundIDToPath)
	{
		MainSingleton::Get().GetAudioEngine().Set3DPosition(ID.second, position);
	}
}

void PlayerComponent::UpdateRRSound(PlayerSoundID anID)
{
	switch (anID)
	{
	case PlayerSoundID::Hurt:
		myCurrentHurtSound = (static_cast<unsigned>(myCurrentHurtSound)) < static_cast<unsigned>(PlayerSoundID::Hurt3) ?
			static_cast<PlayerSoundID>(static_cast<unsigned>(myCurrentHurtSound) + 1) : PlayerSoundID::Hurt;
		break;
	case PlayerSoundID::Death:
		myCurrentDeathSound = (static_cast<unsigned>(myCurrentDeathSound)) < static_cast<unsigned>(PlayerSoundID::Death3) ?
			static_cast<PlayerSoundID>(static_cast<unsigned>(myCurrentDeathSound) + 1) : PlayerSoundID::Death;
		break;

	default:
		break;
	}
}
