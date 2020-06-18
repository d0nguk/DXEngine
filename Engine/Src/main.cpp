#include "Device.h"

int WINAPI WinMain
(
	HINSTANCE hInst,
	HINSTANCE hPrevInst,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	if (LEAKTEST)
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		//_CrtSetBreakAlloc(154);
	}

	TCHAR enter = L'\n';
	int size = sizeof(enter);

	Device device(hInst);

	if (device.Init())
	{
		device.Run();
	}

	device.Release();

	if (LEAKTEST)
		_CrtDumpMemoryLeaks();

	return 0;
}