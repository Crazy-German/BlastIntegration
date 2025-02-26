#include "GraphicsEngine.pch.h"
#include "Texture.h"

CU::Vector2<unsigned> Texture::GetTextureSize() const
{
	ID3D11Resource* resource = nullptr;
	mySRV->GetResource(&resource);

	if (!resource)
	{
		return { 0,0 };
	}

	ID3D11Texture2D* texture = reinterpret_cast<ID3D11Texture2D*>(resource);
	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	CU::Vector2<unsigned> size;
	size.x = desc.Width;
	size.y = desc.Height;

	resource->Release();

	return size;
}