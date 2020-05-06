#include "AudioHandler.h"



AudioHandler::AudioHandler() {
//	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
//
//	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
//#ifdef _DEBUG
//	eflags = eflags | DirectX::AudioEngine_Debug;
//#endif
//	m_audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);
}


AudioHandler::~AudioHandler()
{
}

void AudioHandler::Update() {
	//if (!m_audioEngine->Update()) {
	//	// No audio device is active
	//	if (m_audioEngine->IsCriticalError()) {

	//	}
	//}
}
