#include "GraphicsEngine.pch.h"
#include "GfxCmd_BeginEvent.h"

GfxCmd_BeginEvent::~GfxCmd_BeginEvent()
{

}

void GfxCmd_BeginEvent::Execute()
{
	GraphicsEngine::Get().BeginEvent(myName);
}

void GfxCmd_BeginEvent::Destroy()
{
	myName.~basic_string_view();
}
