#include "Animator.h"
#include "Sprite.h"

Animator::Animator(Sprite & _sprite)
	: m_sprite(_sprite)
{}

void Animator::Init(int _sFrame, int _eFrame, float _fRate, bool _loop) {
	m_elapsedTime = 0.f;
	m_startFrame = _sFrame;
	m_endFrame = _eFrame;
	m_frameRate = _fRate;
	m_loop = _loop;
	m_sprite.SetFrame(m_startFrame);
	m_curFrame = m_startFrame;
}

void Animator::Update(float _deltaTime) {
	// Is the animation playing?
	if (m_playing) {
		m_elapsedTime += _deltaTime;

		// Should the frame change?
		if (m_elapsedTime > (1.f / m_frameRate)) {
			m_elapsedTime = 0.f;

			// Increment sprite frame index
			++m_curFrame;
			// Has it exceeded the final frame?
			if (m_curFrame > m_endFrame) {
				// If looping, go back to first frame, else stay on final frame
				m_curFrame = m_loop ? m_startFrame : m_endFrame;
			}
			// Set sprite animation frame
			m_sprite.SetFrame(m_curFrame);
		}
	}

}

void Animator::Play(bool _play) {
	m_playing = _play;
}

Animator& Animator::operator=(const Animator & _anim) {
	m_startFrame = _anim.m_startFrame;
	m_endFrame = _anim.m_endFrame;
	m_curFrame = _anim.m_curFrame;

	m_frameRate = _anim.m_frameRate;
	m_elapsedTime = _anim.m_elapsedTime;

	m_loop = _anim.m_loop;
	m_playing = _anim.m_playing;

	return *this;
}
