#pragma once
#include <Audio.h>
#include <memory>

#include "Singleton.h"


class AudioHandler : Singleton<AudioHandler>
{
public:
	struct SoundEffect {
		std::unique_ptr<SoundEffect> soundEffect;
	};

	AudioHandler();
	~AudioHandler();

	void Update();
private:
	std::unique_ptr<DirectX::AudioEngine> m_audioEngine;
};

