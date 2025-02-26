#pragma once
#include <array>
#include <d3d11.h>
#include <wrl.h>
#include "../../Utilities/CommonUtilities/Vector2.hpp"
#include "ViewPort.h"

class RenderHardwareInterface;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;

class Texture
{
	friend RenderHardwareInterface;

public:
	const Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRTV() { return myRTV; }
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDSV() { return myDSV; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() { return mySRV; }

	CU::Vector2<unsigned> GetTextureSize() const;
	CU::Vector2f GetSize() { return {myViewPort.Width, myViewPort.Height}; }
	const void SetSize(CU::Vector2f aSize) { myViewPort.Width = aSize.x, myViewPort.Height = aSize.y; }


	std::string myName;   // Only for debugging why all materials in vertex painting are the same, remove when vertex painting exists in engine // TODO: remove when vertex painting is verified

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> myRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> myDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mySRV;

	struct Properties
	{
		UINT myBindFlags;
		UINT myCPUAccessFlags;
		D3D11_USAGE myUsage;
		UINT mySize;
	};
	Properties myProperties = {};

	std::array<float, 4> myClearColor = { 0, 0, 0, 0 };

	ViewPort myViewPort = { 0, 0, 0, 0, 0, 0 };
};

