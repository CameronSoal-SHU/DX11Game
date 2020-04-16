#pragma once
#include "SpriteBatch.h"
#include "D3D.h"

class Sprite;

class Animator
{
public:
	Animator(Sprite& _sprite);

	void Init(int _sFrame, int _eFrame, float _fRate, bool _loop);
	void Update(float _deltaTime);
	void Play(bool _play);

	Animator& operator=(const Animator& _anim);
private:
	int m_startFrame;
	int m_endFrame;
	int m_curFrame;

	float m_frameRate;
	float m_elapsedTime;
	
	bool m_playing;
	bool m_loop;

	Sprite& m_sprite;
};

