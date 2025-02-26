#include "TextFactory.h"
#include "nlohmann/json.hpp"
#include <SpriteVertex.h>
#include <codecvt>

TextFactory* TextFactory::myInstance;

TextFactory::TextFactory()
{
}

void TextFactory::Init()
{
	//HRESULT result;

	//const std::wstring fontFileName = L"skibidi";
	//const std::wstring atlasFileName = fontFileName + L".dds";
	//const std::wstring fontDefinition = fontFileName + L".json";

	//std::ifstream fontDefStream(fontDefinition);

	//using nlohmann::json;
	//json fontDef;

	//fontDefStream >> fontDef;
	//fontDefStream.close();

	//Font font;

	//font.Atlas.Size = fontDef["atlas"]["size"];
	//font.Atlas.Width = fontDef["atlas"]["width"];
	//font.Atlas.Height = fontDef["atlas"]["height"];
	//font.Atlas.EmSize = fontDef["metrics"]["emSize"];
	//font.Atlas.LineHeight = fontDef["metrics"]["lineHeight"];
	//font.Atlas.Ascender = fontDef["metrics"]["ascender"];
	//font.Atlas.Descender = fontDef["metrics"]["descender"];

	//size_t glyphCount = fontDef["glyphs"].size();

	//for (size_t g = 0; g < glyphCount; g++)
	//{
	//	unsigned unicode = fontDef["glyphs"][g].value("unicode", 0);
	//	float advance = fontDef["glyphs"][g].value("advance", -1.0f);

	//	CU::Vector4f planeBounds = CU::Vector4f(0, 0, 0, 0);

	//	if (fontDef["glyphs"][g].find("planeBounds") != fontDef["glyphs"][g].end())
	//	{
	//		planeBounds.x = fontDef["glyphs"][g]["planeBounds"]["left"];
	//		planeBounds.y = fontDef["glyphs"][g]["planeBounds"]["bottom"];
	//		planeBounds.z = fontDef["glyphs"][g]["planeBounds"]["right"];
	//		planeBounds.w = fontDef["glyphs"][g]["planeBounds"]["top"];
	//	}

	//	CU::Vector4f uvBounds = CU::Vector4f(0, 0, 0, 0);

	//	if (fontDef["glyphs"][g].find("atlasBounds") != fontDef["glyphs"][g].end())
	//	{
	//		const float UVStartX =	static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["left"])		/ static_cast<float>(font.Atlas.Width);
	//		const float UVStartY =	static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["bottom"])	/ static_cast<float>(font.Atlas.Height);
	//		const float UVEndX =	static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["right"])	/ static_cast<float>(font.Atlas.Width);
	//		const float UVEndY =	static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["top"])		/ static_cast<float>(font.Atlas.Height);

	//		uvBounds = { UVStartX, UVStartY, UVEndX, UVEndY };
	//	}

	//	font.Glyphs.insert({ unicode,{static_cast<char>(unicode), advance, planeBounds, uvBounds} }); //what the fuck
	//}

	//font.textTexture = std::make_shared<Texture>();
	////GraphicsEngine::Get().LoadTexture(std::filesystem::path("comic-sans.dds"), *font.textTexture);

	//myFonts[std::wstring(L"comic-sans")] = font;

}

void TextFactory::LoadFont(const std::wstring aFontPath, std::shared_ptr<Font> aOutFont)
{
	const std::wstring fontDefinition = aFontPath;

	std::ifstream fontDefStream(fontDefinition);

	using nlohmann::json;
	json fontDef;

	fontDefStream >> fontDef;
	fontDefStream.close();



	aOutFont->Atlas.Size = fontDef["atlas"]["size"];
	aOutFont->Atlas.Width = fontDef["atlas"]["width"];
	aOutFont->Atlas.Height = fontDef["atlas"]["height"];
	aOutFont->Atlas.EmSize = fontDef["metrics"]["emSize"];
	aOutFont->Atlas.LineHeight = fontDef["metrics"]["lineHeight"];
	aOutFont->Atlas.Ascender = fontDef["metrics"]["ascender"];
	aOutFont->Atlas.Descender = fontDef["metrics"]["descender"];

	size_t glyphCount = fontDef["glyphs"].size();

	for (size_t g = 0; g < glyphCount; g++)
	{
		unsigned unicode = fontDef["glyphs"][g].value("unicode", 0);
		float advance = fontDef["glyphs"][g].value("advance", -1.0f);

		CU::Vector4f planeBounds = CU::Vector4f(0, 0, 0, 0);

		if (fontDef["glyphs"][g].find("planeBounds") != fontDef["glyphs"][g].end())
		{
			planeBounds.x = fontDef["glyphs"][g]["planeBounds"]["left"];
			planeBounds.y = fontDef["glyphs"][g]["planeBounds"]["bottom"];
			planeBounds.z = fontDef["glyphs"][g]["planeBounds"]["right"];
			planeBounds.w = fontDef["glyphs"][g]["planeBounds"]["top"];
		}

		CU::Vector4f uvBounds = CU::Vector4f(0, 0, 0, 0);

		if (fontDef["glyphs"][g].find("atlasBounds") != fontDef["glyphs"][g].end())
		{
			const float UVStartX = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["left"]) / static_cast<float>(aOutFont->Atlas.Width);
			const float UVStartY = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["bottom"]) / static_cast<float>(aOutFont->Atlas.Height);
			const float UVEndX = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["right"]) / static_cast<float>(aOutFont->Atlas.Width);
			const float UVEndY = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["top"]) / static_cast<float>(aOutFont->Atlas.Height);

			uvBounds = { UVStartX, UVStartY, UVEndX, UVEndY };
		}

		aOutFont->Glyphs.insert({ unicode,{static_cast<char>(unicode), advance, planeBounds, uvBounds} }); //what the fuck
	}

	//myFonts[std::wstring(L"comic-sans")] = font;
}

Text* TextFactory::CreateText(const std::wstring aText, const int aWorldSize)
{
	std::string str;
	size_t size;
	str.resize(aText.length());
	wcstombs_s(&size, &str[0], str.size() + 1, aText.c_str(), aText.size());
	//return str;

	const std::string asciiString = str;

	std::wstring fontName = L"comic-sans";
	auto It = myFonts.find(fontName);
	Font font = It->second;

	//worldsize is the size in world of the largest glyph in the font`??

	std::vector<SpriteVertex> vertices;
	std::vector<unsigned> indices;
	float x = 0;
	float actualWorldSize = static_cast<float>(aWorldSize) / 1.0f;
	actualWorldSize = static_cast<float>(font.Atlas.Size);

	for (auto c : asciiString)
	{
		const float charAdvance = font[c].Advance * actualWorldSize;
		const unsigned currentVertexCount = static_cast<unsigned>(vertices.size());

		const CU::Vector4f bounds = font[c].UVBounds;
		CU::Vector4f offsets = font[c].Planebounds;

		offsets.x = font[c].Planebounds.x * charAdvance;
		offsets.z = font[c].Planebounds.z * actualWorldSize * 0.65f; //magicn ubmer
		offsets.y = font[c].Planebounds.y * charAdvance;
		offsets.w = font[c].Planebounds.w * actualWorldSize;

		if (abs(offsets.w) > 0.005f)
		{
			offsets.w += font.Atlas.Descender * actualWorldSize;
		}



		//bottom left
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.x, 1 - offsets.w, 0, 1 };
			johnVertex.uv ={ bounds.x, 1 - bounds.w };
			vertices.push_back(johnVertex);
		}

		//top left
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position= { x + offsets.x, 1 - offsets.y, 0, 1 };
			johnVertex.uv = { bounds.x, 1 - bounds.y };
			vertices.push_back(johnVertex);
		}

		//bottom right
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.z, 1 - offsets.w, 0, 1 };
			johnVertex.uv = { bounds.z, 1 - bounds.w };
			vertices.push_back(johnVertex);
		}

		//top right
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.z, 1 - offsets.y, 0, 1 };
			johnVertex.uv = { bounds.z, 1 - bounds.y };
			vertices.push_back(johnVertex);
		}

		x += charAdvance * 0.65f; // uh oh magic number alert!
		
		indices.push_back(currentVertexCount + 1);
		indices.push_back(currentVertexCount);
		indices.push_back(currentVertexCount + 2);

		indices.push_back(currentVertexCount + 3);
		indices.push_back(currentVertexCount + 1);
		indices.push_back(currentVertexCount + 2);
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	GraphicsEngine::Get().CreateVertexBuffer<SpriteVertex>("Text vertex buffer", vertices, vertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer("Text index buffer", indices, indexBuffer);

	Text::TextData textData;
	textData.VertexAmount = static_cast<UINT>(vertices.size());
	textData.VertexBuffer = vertexBuffer;
	textData.IndexAmount = static_cast<UINT>(indices.size());
	textData.IndexBuffer = indexBuffer;

	textData.Stride = sizeof(SpriteVertex);
	textData.Offset = 0;
	textData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Text* text = new Text();
	text->Init(textData, aText, &It->second);

	//myTextObjects.push_back(text);

	return text;
}

std::shared_ptr<Text> TextFactory::CreateText(const std::wstring aText, const int aWorldSize, std::shared_ptr<Font> aFont)
{
	std::string str;
	size_t size;
	str.resize(aText.length());
	wcstombs_s(&size, &str[0], str.size() + 1, aText.c_str(), aText.size());
	//return str;

	const std::string asciiString = str;

	//worldsize is the size in world of the largest glyph in the font`??

	std::vector<SpriteVertex> vertices;
	std::vector<unsigned> indices;
	float x = 0;
	float actualWorldSize = static_cast<float>(aWorldSize) / 1.0f;
	actualWorldSize = static_cast<float>(aFont->Atlas.Size);

	for (unsigned char c : asciiString)
	{
		const float charAdvance = aFont->operator[](c).Advance * actualWorldSize;
		const unsigned currentVertexCount = static_cast<unsigned>(vertices.size());

		const CU::Vector4f bounds = aFont->operator[](c).UVBounds;
		CU::Vector4f offsets = aFont->operator[](c).Planebounds;

		offsets.x = aFont->operator[](c).Planebounds.x * charAdvance;
		offsets.z = aFont->operator[](c).Planebounds.z * actualWorldSize * 0.65f; //magicn ubmer
		offsets.y = aFont->operator[](c).Planebounds.y * charAdvance;
		offsets.w = aFont->operator[](c).Planebounds.w * actualWorldSize;

		if (abs(offsets.w) > 0.005f)
		{
			offsets.w += aFont->Atlas.Descender * actualWorldSize;
		}



		//bottom left
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.x, 1 - offsets.w, 0, 1 };
			johnVertex.uv = { bounds.x, 1 - bounds.w };
			vertices.push_back(johnVertex);
		}

		//top left
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.x, 1 - offsets.y, 0, 1 };
			johnVertex.uv = { bounds.x, 1 - bounds.y };
			vertices.push_back(johnVertex);
		}

		//bottom right
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.z, 1 - offsets.w, 0, 1 };
			johnVertex.uv = { bounds.z, 1 - bounds.w };
			vertices.push_back(johnVertex);
		}

		//top right
		{
			SpriteVertex johnVertex(1, 1, 1, 1);
			johnVertex.tint = { 1,1,1,1 };
			johnVertex.position = { x + offsets.z, 1 - offsets.y, 0, 1 };
			johnVertex.uv = { bounds.z, 1 - bounds.y };
			vertices.push_back(johnVertex);
		}

		x += charAdvance * 0.65f; // uh oh magic number alert!

		indices.push_back(currentVertexCount + 1);
		indices.push_back(currentVertexCount);
		indices.push_back(currentVertexCount + 2);

		indices.push_back(currentVertexCount + 3);
		indices.push_back(currentVertexCount + 1);
		indices.push_back(currentVertexCount + 2);
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	GraphicsEngine::Get().CreateVertexBuffer("Text vertex buffer", vertices, vertexBuffer);
	GraphicsEngine::Get().CreateIndexBuffer("Text index buffer", indices, indexBuffer);

	Text::TextData textData;
	textData.VertexAmount = static_cast<UINT>(vertices.size());
	textData.VertexBuffer = vertexBuffer;
	textData.IndexAmount = static_cast<UINT>(indices.size());
	textData.IndexBuffer = indexBuffer;

	textData.Stride = sizeof(SpriteVertex);
	textData.Offset = 0;
	textData.PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::shared_ptr<Text> text = std::make_shared<Text>();
	text->Init(textData, aText, aFont.get());

	//myTextObjects.push_back(text);

	return text;
}

float TextFactory::GetWidthOfText(Text* aText)
{
	const Font& font = aText->GetFont();
	const std::wstring& text = aText->GetText();

	std::string str;
	size_t size;
	str.resize(text.length());
	wcstombs_s(&size, &str[0], str.size() + 1, text.c_str(), text.size());

	const std::string asciiString = str;

	float x = 0;
	float actualWorldSize = static_cast<float>(1) / 1.0f;
	actualWorldSize = static_cast<float>(font.Atlas.Size);

	for (auto c : asciiString)
	{
		const float charAdvance = font.operator[](c).Advance * actualWorldSize;

		const CU::Vector4f bounds = font.operator[](c).UVBounds;
		CU::Vector4f offsets = font.operator[](c).Planebounds;

		offsets.x = font.operator[](c).Planebounds.x * charAdvance;
		offsets.z = font.operator[](c).Planebounds.z * actualWorldSize * 0.65f; //magicn ubmer
		offsets.y = font.operator[](c).Planebounds.y * charAdvance;
		offsets.w = font.operator[](c).Planebounds.w * actualWorldSize;

		if (abs(offsets.w) > 0.005f)
		{
			offsets.w += font.Atlas.Descender * actualWorldSize;
		}


		x += charAdvance * 0.65f; // uh oh magic number alert!
	}

	return x;

}

float TextFactory::GetWidthOfText(const std::wstring& aText, std::shared_ptr<Font> aFont)
{
	std::string str;
	size_t size;
	str.resize(aText.length());
	wcstombs_s(&size, &str[0], str.size() + 1, aText.c_str(), aText.size());

	const std::string asciiString = str; 

	float x = 0;
	float actualWorldSize = static_cast<float>(1) / 1.0f;
	actualWorldSize = static_cast<float>(aFont->Atlas.Size);

	for (auto c : asciiString)
	{
		const float charAdvance = aFont->operator[](c).Advance * actualWorldSize;

		const CU::Vector4f bounds = aFont->operator[](c).UVBounds;
		CU::Vector4f offsets = aFont->operator[](c).Planebounds;

		offsets.x = aFont->operator[](c).Planebounds.x * charAdvance;
		offsets.z = aFont->operator[](c).Planebounds.z * actualWorldSize * 0.65f; //magicn ubmer
		offsets.y = aFont->operator[](c).Planebounds.y * charAdvance;
		offsets.w = aFont->operator[](c).Planebounds.w * actualWorldSize;

		if (abs(offsets.w) > 0.005f)
		{
			offsets.w += aFont->Atlas.Descender * actualWorldSize;
		}


		x += charAdvance * 0.65f; // uh oh magic number alert!
	}

	return x;
}

