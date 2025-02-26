#include "FeatherEngine.pch.h"
#include "JsonParser.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

GaiJsonParser::~GaiJsonParser()
{
}


void GaiJsonParser::ParseFile(const std::string& aFilePath)
{
	std::ifstream readingStream(aFilePath.c_str());

	using namespace nlohmann;
	myData = { json::parse(readingStream) };
	readingStream.close();
}

void GaiJsonParser::Reset()
{
	myData.clear();
}

json GaiJsonParser::GetFileContents(const std::string& aFilePath)
{
	std::ifstream readingStream(aFilePath.c_str());

	using namespace nlohmann;
	json data = { json::parse(readingStream) };
	readingStream.close();
	return data;
}

float GaiJsonParser::GetSideSpeed() const
{
	return myData["speedSideWays"];
}


std::string GaiJsonParser::GetPlayerSpritePath() const
{
	return myData["playerImagePath"];
}

std::string GaiJsonParser::GetEnemySpritePath() const
{
	return myData["enemyImageName"];
}

float GaiJsonParser::GetEnemySpeedMultiplier() const
{
	return myData["enemySpeedMultiplier"];
}

float GaiJsonParser::GetPlayerSpeed()
{
	return myData["playerSpeed"];
}

int GaiJsonParser::GetPlayerHealth()
{
	return myData["playerHealth"];
}

float GaiJsonParser::GetBallStartSpeed()
{
	return myData["ballStartSpeed"];
}

std::string GaiJsonParser::GetProjectileSpritePath() const
{
	return myData["shotSpritePath"];
}

std::string GaiJsonParser::GetHorizontalBorderSpritePath() const
{
	return myData["horizontalBorderSpritePath"];
}

std::string GaiJsonParser::GetVerticalBorderSpritePath() const
{
	return myData["verticalBorderSpritePath"];
}

std::string GaiJsonParser::GetTiledBackgroundSpritePath() const
{
	return myData["tiledBackgroundPath"];
}

std::string GaiJsonParser::GetShelterSpritePath() const
{
	return myData["shelterSpritePath"];
}

std::string GaiJsonParser::GetBoxSpritePath() const
{
	return myData["boxSpritePath"];
}

std::string GaiJsonParser::GetCrewSpritePath() const
{
	return myData["crewSpritePath"];
}


std::vector<std::array<std::string, 3>> GaiJsonParser::GetIntroScreenText() const
{

	std::vector<std::array<std::string, 3>> returnVector;
	for (auto& elem : myData["IntroScreenTexts"])
	{
		std::array<std::string, 3> insertArray;
		insertArray[0] = elem["Text"];
		insertArray[1] = elem["xPosition"];
		insertArray[2] = elem["yPosition"];
		returnVector.push_back(insertArray);
	}
	return returnVector;
}

std::vector<std::array<std::string, 3>> GaiJsonParser::GetIntroScreenSpritePaths() const
{
	std::vector<std::array<std::string, 3>> returnVector;
	for (auto& elem : myData["MainScreenSprites"])
	{
		std::array<std::string, 3> insertArray;
		insertArray[0] = elem["SpritePath"];
		insertArray[1] = elem["xPosition"];
		insertArray[2] = elem["yPosition"];
		returnVector.push_back(insertArray);
	}
	return returnVector;
}

std::vector<std::array<std::string, 3>> GaiJsonParser::GetBackgroundSpritePaths() const
{
	std::vector<std::array<std::string, 3>> returnVector;
	for (auto& elem : myData["BackgroundImages"])
	{
		std::array<std::string, 3> insertArray;
		insertArray[0] = elem["SpritePath"];
		insertArray[1] = elem["xPosition"];
		insertArray[2] = elem["yPosition"];
		returnVector.push_back(insertArray);
	}
	return returnVector;
}

std::vector<std::array<std::string, 3>> GaiJsonParser::GetHighScores() const
{
	std::vector<std::array<std::string, 3>> returnVector;
	for (auto& elem : myData["HighScores"])
	{
		std::array<std::string, 3> insertArray;
		insertArray[0] = elem["topOne"];
		insertArray[1] = elem["topTwo"];
		insertArray[2] = elem["topThree"];
		returnVector.push_back(insertArray);
	}
	return returnVector;
}

std::vector<std::array<std::array<int, 256>, 2>> GaiJsonParser::GetLevelLayouts()
{
	std::vector<std::array<std::array<int, 256>, 2>> returnVector;
	for (auto& elem : myData["LevelLayouts"])
	{

		std::array<std::array<int, 256>, 2> midwayInsert;
		midwayInsert[0].fill(-1);
		midwayInsert[1].fill(-1);



		std::array<int, 256> insertBlockArray;
		std::array<int, 256> insertHealthArray;
		insertBlockArray.fill(-1);
		insertHealthArray.fill(-1);

		for (int elemBlockIndex = 0; elemBlockIndex < elem["Blocks"].size(); elemBlockIndex++)
		{
			insertBlockArray[elemBlockIndex] = elem["Blocks"][elemBlockIndex];
		}
		for (int elemHealthIndex = 0; elemHealthIndex < elem["Healths"].size(); elemHealthIndex++)
		{
			insertHealthArray[elemHealthIndex] = elem["Healths"][elemHealthIndex];
		}

		midwayInsert[0] = insertBlockArray;
		midwayInsert[1] = insertHealthArray;

		returnVector.push_back(midwayInsert);
	}
	return returnVector;
}

