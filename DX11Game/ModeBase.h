#pragma once
#include "SpriteBatch.h"

/*
Base class to be inherited by game modes
*/
class ModeBase
{
public:
	virtual bool Exit() { return true; };
	virtual void Enter() = 0;
	virtual void Start() = 0;
	virtual void Update(float _dTime) = 0;
	virtual void Render(float _dTime, DirectX::SpriteBatch& _sprBatch) = 0;
	virtual std::string GetModeName() const = 0;
protected:
	bool m_canSwitchMode = false;
};

