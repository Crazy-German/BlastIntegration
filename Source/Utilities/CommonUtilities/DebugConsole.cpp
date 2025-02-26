#include "pch.h"
#include "DebugConsole.h"

BOOL CommonUtilities::DebugConsole::InitInstance(HINSTANCE, int)
{
	AllocConsole();
	FILE* fIn, * fOut, * fErr;
	freopen_s(&fIn, "conin$", "r", stdin);
	freopen_s(&fOut, "conout$", "w", stdout);
	freopen_s(&fErr, "conout$", "w", stderr);
	printf("Debugging Window:\n");

	if (fIn == nullptr)
	{
		fclose(fIn);
	}
	if (fOut == nullptr)
	{
		fclose(fOut);
	}
	if (fErr == nullptr)
	{
		fclose(fErr);
	}
	return true;
}

