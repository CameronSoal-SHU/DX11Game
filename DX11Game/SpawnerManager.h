#pragma once
#include "EnemyTypes.h"
#include <random>
#include <time.h>

class SpawnerManager
{
public:
	SpawnerManager(PlayMode& _playMode);
	~SpawnerManager();

	void Update(float _deltaTime);

	void SpawnRandomEnemy();
private:
	PlayMode& m_playMode;
	std::vector<std::shared_ptr<Enemy>> m_enemyTypes;

	float m_spawnDelay;
	float m_delayRemaining;

	void GenerateSeed();
	DirectX::SimpleMath::Vector2 GenerateRandCoords();	// Create a random position in-game
	void SetupEnemyTypes();		// Fill enemy types vector with enemies to spawn
};

