#pragma once
#include "Singleton.h"
#include <string>

class GameClock : public Singleton<GameClock>
{
public:
	GameClock();
	void Update(float _deltaTime);

	/* Public accessors */
	int GetMinutes() const { return m_minutes; };
	std::string GetMinutesFormatted() const;
	float GetSeconds() const { return m_seconds; }
	std::string GetSecondsFormatted(int _precision) const;

	~GameClock();
private:
	int m_minutes;
	float m_seconds;
};

