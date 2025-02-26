#pragma once
#include "Texture.h"

struct GBuffer
{
	std::shared_ptr<Texture> Albedo;
	std::shared_ptr<Texture> WorldNormal;
	std::shared_ptr<Texture> Material;
	std::shared_ptr<Texture> WorldPosition;
	std::shared_ptr<Texture> Effects;
};