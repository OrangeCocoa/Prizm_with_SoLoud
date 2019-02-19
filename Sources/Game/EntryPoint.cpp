
#include<windows.h>
#include"BaseSystem.h"

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Prizm::BaseSystem app;
	if (app.Initialize()) app.Run();
	app.Finalize();
	return 0;
}