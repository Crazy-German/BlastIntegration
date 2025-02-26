#include "GfxCmd_RenderGui.h"
#include <DearImGui/imgui.h>

#include <DearImGui/imgui_impl_dx11.h>

GfxCmd_RenderGui::GfxCmd_RenderGui()
{
}

void GfxCmd_RenderGui::Execute()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
