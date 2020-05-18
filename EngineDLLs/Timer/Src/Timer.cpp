#define TIMER_EXPORT __declspec(dllexport)

#include "Timer.h"
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

namespace ENGINEDLL
{
	float CTIMER::m_fTime = 0.0f;
	float CTIMER::m_fUnscaledTime = 0.0f;

	float CTIMER::m_fDeltaTime = 0.0f;
	float CTIMER::m_fUnscaledDeltaTime = 0.0f;
	float CTIMER::m_fTimeScale = 1.0f;

	unsigned long CTIMER::m_iNewTime = 0;
	unsigned long CTIMER::m_iOldTime = 0;

	bool CTIMER::m_bPause = false;

	void TIMER_EXPORT CTIMER::Init()
	{
		::timeBeginPeriod(1);

		Reset();
	}

	void TIMER_EXPORT CTIMER::Update()
	{
		m_iNewTime = ::timeGetTime();
		m_fDeltaTime = (m_iNewTime - m_iOldTime) * 0.001f;
		m_iOldTime = m_iNewTime;

		m_fUnscaledDeltaTime = m_fDeltaTime;
		m_fUnscaledTime += m_fUnscaledDeltaTime;

		if (m_bPause)
		{
			m_fDeltaTime = 0.0f;
			return;
		}

		m_fDeltaTime *= m_fTimeScale;
		m_fTime += m_fDeltaTime;
	}

	void TIMER_EXPORT CTIMER::Release()
	{
		::timeEndPeriod(1);
	}

	void TIMER_EXPORT CTIMER::SetTimeScale(float fTimeScale)
	{
		m_fTimeScale = fTimeScale;
	}

	void TIMER_EXPORT CTIMER::SpeedUp()
	{
		m_fTimeScale += 1.0f * m_fUnscaledDeltaTime;

		if (m_fTimeScale >= 10.0f)
			m_fTimeScale = 10.0f;
	}

	void TIMER_EXPORT CTIMER::SpeedDown()
	{
		m_fTimeScale -= 1.0f * m_fUnscaledDeltaTime;

		if (m_fTimeScale <= 0.01f)
			m_fTimeScale = 0.01f;
	}

	void TIMER_EXPORT CTIMER::Reset()
	{
		m_iNewTime = ::timeGetTime();
		m_iOldTime = ::timeGetTime();
		m_fDeltaTime = 0;
		m_fUnscaledDeltaTime = 0;

		m_fTimeScale = 1.0f;

		m_bPause = false;
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;

	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}

	return TRUE;
}