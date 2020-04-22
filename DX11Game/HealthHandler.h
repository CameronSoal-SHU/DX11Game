#pragma once
#include <algorithm>

class HealthHandler
{
public:
	HealthHandler(float _maxHealth = 100.f, float _curHealth = 100.f)
		: m_maxHealth(_maxHealth), m_curHealth(_curHealth)
	{}

	/* Public Accessors */
	float GetCurHealth() const { return m_curHealth; }
	void SetCurHealth(float _newHealth) { m_curHealth = _newHealth; }
	float GetMaxHealth() const { return m_maxHealth; }
	void SetMaxHealth(float _newHealth) { m_maxHealth = _newHealth; }
	float GetHealthRatio() const { return (m_curHealth / m_maxHealth); }

	void TakeDamage(float _damageAmount) { m_curHealth -= _damageAmount; }
	void Heal(float _healthAmount) {
		// Clamp current health so it cannot exceed maximum health
		m_curHealth = std::clamp(m_curHealth + _healthAmount, 0.f, m_maxHealth);
	}
	// Has the current health dropped to or below 0?
	bool IsDead() const { return (m_curHealth <= 0.f); }
private:
	float m_maxHealth;
	float m_curHealth;
};

