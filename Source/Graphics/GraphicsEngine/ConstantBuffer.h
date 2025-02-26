#pragma once
#include "wrl.h"

//remove
#include <d3d11.h>

class ConstantBuffer
{
	friend class RenderHardwareInterface;

	Microsoft::WRL::ComPtr<ID3D11Buffer> myBuffer;
	std::string myName;
	size_t mySize = 0;

	unsigned myPipelineStages = 0;
	unsigned mySlotIndex = 0;
};

