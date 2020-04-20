#pragma once
class HealthHandler
{
public:
	HealthHandler(float _maxHealth = 100.f, float _curHealth = 100.f);

	/* Public Accessors */
	float GetCurHealth() const;
	float GetMaxHealth() const;
	float GetHealthRatio() const;

	void TakeDamage(float _damageAmount);
	void Heal(float _healthAmount);
	// Has the current health dropped to or below 0?
	bool IsDead() const;

	~HealthHandler();
private:
	float m_maxHealth;
	float m_curHealth;
};

