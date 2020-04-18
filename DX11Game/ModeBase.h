#pragma once
#include "SpriteBatch.h"

/*
Base class to be inherited by game modes
*/
class ModeBase
{
public:
	virtual ~ModeBase() {};

	virtual bool Exit() { return true; };
	virtual void Enter() {};
	virtual void Update(float _dTime) = 0;
	virtual void Render(float _dTime, DirectX::SpriteBatch& _sprBatch) = 0;
	virtual std::string GetModeName() const = 0;
	virtual void ProcessKey(char _key) {};
};

