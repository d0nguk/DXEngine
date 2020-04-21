#include "Device.h"

int WINAPI WinMain
(
	HINSTANCE hInst,
	HINSTANCE hPrevInst,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(157);

	Device device(hInst);

	if (device.Init())
	{
		device.Run();
	}

	device.Release();

	_CrtDumpMemoryLeaks();
	return 0;
}