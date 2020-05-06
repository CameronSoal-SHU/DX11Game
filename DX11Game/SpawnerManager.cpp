#include "SpawnerManager.h"
#include "PlayMode.h"


SpawnerManager::SpawnerManager(PlayMode& _playMode) 
	: m_playMode(_playMode), m_spawnDelay(5.f), 
	m_delayRemaining(5.f) {
	m_enemyTypes.reserve(2);

	SetupEnemyTypes();
}


SpawnerManager::~SpawnerManager()
{
}

void SpawnerManager::Update(float _deltaTime) {
	m_delayRemaining -= _deltaTime;

	if (m_delayRemaining <= 0.f) {
		int idx = (rand() % 2);

		std::shared_ptr<Enemy> enemy = nullptr;

		if (idx == 1)
			enemy = std::make_shared<EnemyRedShip>();
		else
			enemy = std::make_shared<EnemyGreenShip>();

		enemy->SetLookAtPlayer(true);
		enemy->SetActive(true);
		enemy->SetParentMode(m_playMode);
		enemy->GetSprite().SetPos(GenerateRandCoords());	// Place enemy at random place

		m_playMode.AddObj(enemy);
		m_delayRemaining = m_spawnDelay;
	}
}

void SpawnerManager::SpawnRandomEnemy() {

}

void SpawnerManager::GenerateSeed() {
	unsigned seed = static_cast<unsigned>(time(NULL));
	srand(seed);
}

DirectX::SimpleMath::Vector2 SpawnerManager::GenerateRandCoords()
{
	// Pick a random point within the play area
	float coordX = (rand() % (int)m_playMode.GetPlayArea().right) + m_playMode.GetPlayArea().left;
	float coordY = (rand() % (int)m_playMode.GetPlayArea().bottom) + m_playMode.GetPlayArea().top;

	return DirectX::SimpleMath::Vector2(coordX, coordY);
}

void SpawnerManager::SetupEnemyTypes() {
	m_enemyTypes.push_back(std::make_shared<EnemyRedShip>());		// Red enemy ship
	m_enemyTypes.push_back(std::make_shared<EnemyGreenShip>());		// Green enemy ship
}
