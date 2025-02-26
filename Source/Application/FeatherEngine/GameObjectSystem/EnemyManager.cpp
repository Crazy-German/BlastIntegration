#include "FeatherEngine.pch.h"
#include "EnemyManager.h"

void EnemyManager::Update()
{
}

void EnemyManager::Render()
{
}

void EnemyManager::AddEnemy(EnemyBehavior* aEnemy)
{
	myEnemies.push_back(aEnemy);
}

void EnemyManager::AddEnemyToGroup(EnemyBehavior* aEnemy, int aGroupIndex)
{
	while (myEnemies.size() < aGroupIndex - 1)
	{
		myEnemyGroups.push_back(std::vector<EnemyBehavior*>());
	}
	myEnemyGroups[aGroupIndex].push_back(aEnemy);
}
