#include "GameClock.h"
#include <iomanip>
#include <sstream>

GameClock::GameClock()
	: m_minutes(0), m_seconds(0.f)
{}

void GameClock::Update(float _deltaTime) {
	m_seconds += _deltaTime;

	// Has a minute passed?
	if (m_seconds >= 60.f) {
		++m_minutes;
		m_seconds = 0.f;
	}
}


std::string GameClock::GetMinutesFormatted() const {
	std::string minutes;

	if (m_minutes < 10) {
		minutes += '0';
	}
	minutes += std::to_string(m_minutes);

	return minutes;
}

std::string GameClock::GetSecondsFormatted(int _precision) const {
	std::string seconds;
	std::stringstream stream;
	stream << std::fixed << std::setprecision(_precision) << m_seconds;
	if (m_seconds < 10) {
		seconds += '0';
	}
	seconds += stream.str();

	return seconds;
}

GameClock::~GameClock()
{}
