#pragma once
#include "GameObjectSystem\Components\EnemyBehavior.h"

class EnemyManager
{
public:
	void Update();
	void Render();
	void AddEnemy(EnemyBehavior* aEnemy);
	void AddEnemyToGroup(EnemyBehavior* aEnemy, int aGroupIndex);

private:
	std::vector<EnemyBehavior*> myEnemies;
	std::vector<std::vector<EnemyBehavior*>> myEnemyGroups;
};