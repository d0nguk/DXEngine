#pragma once

#ifdef TIMER_EXPORT
#define TIMER_EXPORT __declspec(dllexport)
#else
#define TIMER_EXPORT __declspec(dllimport)
#endif

namespace ENGINEDLL
{
	class TIMER_EXPORT CTIMER
	{
	private:
		CTIMER() {}
		~CTIMER() {}

	public:
		static void Init();
		static void Update();
		static void Release();

		static void SetTimeScale(float fTimeScale);
		static void SpeedUp();
		static void SpeedDown();
		static void Reset();

		static unsigned int NewTime() { return m_iNewTime; }
		static unsigned int OldTime() { return m_iOldTime; }

		static float Time() { return m_fTime; }
		static float UnscaledTime() { return m_fUnscaledTime; }

		static float DeltaTime() { return m_fDeltaTime; }
		static float UnscaledDeltaTime() { return m_fUnscaledDeltaTime; }

	private:
		static float m_fTime;
		static float m_fUnscaledTime;

		static float m_fDeltaTime;
		static float m_fUnscaledDeltaTime;
		static float m_fTimeScale;
		
		static unsigned long m_iNewTime;
		static unsigned long m_iOldTime;
		
		static bool m_bPause;
	};
}