#pragma once
#include <unordered_map>
#include <string>
#include <CommonUtilities/Vector.hpp>
#include "../Texture.h"

struct Font
{
	struct Atlas
	{

		int Size;
		int Width;
		int Height;
		float EmSize;
		float LineHeight;
		float Ascender;
		float Descender;
		float UnderlineY;
		float UnderlineThickness;


	} Atlas;

	struct Glyph
	{
		char Character = ' ';
		float Advance = 0.0f;
		CU::Vector4f Planebounds;
		CU::Vector4f UVBounds;
	};

	std::unordered_map<unsigned, Glyph> Glyphs;

	ID3D11ShaderResourceView* SRV;
	std::shared_ptr<Texture> textTexture;

	Glyph operator[](unsigned char c)
	{
		return Glyphs[c];
	}

	const Glyph operator[](unsigned char c) const
	{
		return Glyphs.at(c);
	}
};

