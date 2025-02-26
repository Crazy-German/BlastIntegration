#pragma once
struct ViewPort
{
	float TopLeftX;
	float TopLeftY;
	float Width;
	float Height;
	float MinDepth;
	float MaxDepth;
};

static CU::Vector2f GetViewportSize(ViewPort aViewPort)
{
	return CU::Vector2f(aViewPort.Width, aViewPort.Height);
}