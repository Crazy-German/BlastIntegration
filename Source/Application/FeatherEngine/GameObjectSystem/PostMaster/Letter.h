#pragma once
#include "CommonUtilities/Vector3.hpp"

enum class eLetterRegister
{
	Test = 0,
	GroundClick,
	EnemyClick,
	ShiftClick,
	ComboAttack1,
	ComboAttack2,
	ComboAttack3,
	HeavyAttack,
	PlayerShoot,
	WarCry,
	EnemyShoot,				   //Should be sent with an index for identification of projectile effect that calls play
	EnemySummon,			   //Should be sent with an index for identification of summoner
	BossSwipe,
	AbilityOne,
	Scream,
	AbilityThree,
	AbilityFour,
	PlayerProjectileDespawned,
	PlayerHit,
	Collision,
	EnemyDied,
	ScriptedEvent,				//shouldn't have data
	StartGame,					//shouldn't have data
	BackToGame,					//shouldn't have data
	GoToMainMenu,				//shouldn't have data
	GoToLevelSelect,			//shouldn't have data
	GoToSettingsMenu,			//shouldn't have data
	ReturnFromSettings,			//shouldn't have data
	GoToCreditsMenu,			//shouldn't have data
	ExitGame,					//shouldn't have data
	SetMasterVolume,			//should be sent with a 0.0-1.0 float
	SetMusicVolume,				//should be sent with a 0.0-1.0 float
	SetEffectsVolume,			//should be sent with a 0.0-1.0 float
	SetEffects3DVolume,			//should be sent with a 0.0-1.0 float
	SetVoiceVolume,				//should be sent with a 0.0-1.0 float
	SetResolution,				//should be sent with Resolution struct
	SetMuteAll,					//should be sent with bool
	LoadLevel,		//should be sent with level index, unsigned
	LoadingLevel,	//shouldn't have data
	LoadingDone,	//should be sent with a c-string
	LoadingExit,	//shouldn't have data
	LoadLights,		//shouldn't have data
	LoadFadeDone,	//shouldn't have data
	SetLoadingScreenMask, //Should be send with level index unsigned
	AbilityUnlocked, //should be sent with ability index, int
	AbilityLocked,   //should be sent with ability index, int
	BossActivated, //should be sent with a bool and boss name, std::pair<bool, std::wstring>
	BossHealthChanged, //should be sent as a float, normalized boss health (or whatever you'd call it)
	PlayVideo,		//okay, hear me out. should be sent with a std::pair<std::string*, Letter*>*. the string is the path to the cutscene and the letter is what event to send upon completion
	VideoOver,		//shouldn't have data
	SkipVideo,		//shouldn't have data
	NavMeshSwitch, //shouldn't have date
	TapeStart, //TEMP: Should be sent with a std::string filename //need to structure this properly
	InventoryScroll, //TEMP: Should be sent with signed int - for scroll up and + for scroll down
	SpawnParticleEmitter, //Should be sent with a ParticleMessage found in ParticleMessageStruct.h
	_Last
};

struct Letter
{
	eLetterRegister myType;
	void* myData;
};

struct ScreamStruct
{
	float myRange;
	CU::Vector3f myPosition;
};