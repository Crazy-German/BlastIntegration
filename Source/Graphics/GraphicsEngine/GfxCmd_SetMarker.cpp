#include "GraphicsEngine.pch.h"
#include "GfxCmd_SetMarker.h"

GfxCmd_SetMarker::GfxCmd_SetMarker(std::string_view aMarker)
{

	  myMarker = aMarker;
}

void GfxCmd_SetMarker::Execute()
{
	GraphicsEngine::Get().SetMarker(myMarker);
}

void GfxCmd_SetMarker::Destroy()
{
	myMarker.~basic_string_view();
}

