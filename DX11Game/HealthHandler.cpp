#include "HealthHandler.h"
#include <algorithm>



HealthHandler::HealthHandler(float _maxHealth, float _curHealth) 
	: m_maxHealth(_maxHealth), m_curHealth(_curHealth)
{}

float HealthHandler::GetCurHealth() const {
	return m_curHealth;
}

float HealthHandler::GetMaxHealth() const {
	return m_maxHealth;
}

float HealthHandler::GetHealthRatio() const {
	return (m_curHealth / m_maxHealth);
}

void HealthHandler::TakeDamage(float _damageAmount) {
	m_curHealth -= _damageAmount;
}

void HealthHandler::Heal(float _healthAmount) {
	// Clamp current health so it cannot exceed maximum health
	m_curHealth = std::clamp(m_curHealth + _healthAmount, 0.f, m_maxHealth);
}

bool HealthHandler::IsDead() const {
	return (m_curHealth <= 0.f);
}

HealthHandler::~HealthHandler()
{}
