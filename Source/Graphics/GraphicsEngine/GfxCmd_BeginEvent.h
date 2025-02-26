#pragma once
#include "GraphicsCommandList.h"

class GfxCmd_BeginEvent : public GraphicsCommandBase
{
	public:
		explicit GfxCmd_BeginEvent(std::string_view aName) { myName = aName; }
		~GfxCmd_BeginEvent();
		void Execute() override;

		void Destroy() override;

	private:
		std::string_view myName;
};

