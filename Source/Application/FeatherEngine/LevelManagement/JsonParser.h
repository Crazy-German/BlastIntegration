#pragma once
#include <string>
#include <fstream>
#include <map>
#include <array>
#include <nlohmann/json.hpp>
#include <windows.data.json.h>
#include <vector>


using namespace nlohmann;

class GaiJsonParser
{
public:
	GaiJsonParser() = default;
	~GaiJsonParser();

	void ParseFile(const std::string& aFilePath);
	void Reset();
	json GetFileContents(const std::string& aFilePath);

	float GetSideSpeed() const;
	std::string GetPlayerSpritePath() const;

	std::string GetEnemySpritePath() const;
	float GetEnemySpeedMultiplier() const;
	float GetPlayerSpeed();
	int GetPlayerHealth();
	float GetBallStartSpeed();

	std::string GetProjectileSpritePath() const;
	std::string GetHorizontalBorderSpritePath() const;
	std::string GetVerticalBorderSpritePath() const;
	std::string GetTiledBackgroundSpritePath() const;
	std::string GetShelterSpritePath() const;
	std::string GetBoxSpritePath() const;
	std::string GetCrewSpritePath() const;


	std::vector<std::array<std::string, 3>> GetIntroScreenText() const;
	std::vector<std::array<std::string, 3>> GetIntroScreenSpritePaths() const;

	std::vector<std::array<std::string, 3>> GetBackgroundSpritePaths() const;

	std::vector<std::array<std::string, 3>> GetHighScores() const;
	std::vector<std::array<std::array<int, 256>, 2>> GetLevelLayouts();

private:
	json myData;
};