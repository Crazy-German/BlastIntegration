#include "GraphicsEngine.pch.h"
#include "GfxCmd_EndEvent.h"

GfxCmd_EndEvent::~GfxCmd_EndEvent()
{
}

void GfxCmd_EndEvent::Execute()
{
	GraphicsEngine::Get().EndEvent();
}

void GfxCmd_EndEvent::Destroy()
{
	myName.~basic_string_view();
}
