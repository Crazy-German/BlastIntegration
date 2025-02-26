#include "GraphicsEngine.pch.h"
#include "Material.h"

#include "Texture.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

Material::Material()
{
	myAlbedoTexture = std::make_shared<Texture>();
	myNormalTexture = std::make_shared<Texture>();
	myMaterialTexture = std::make_shared<Texture>();
}

Material::Material(const std::filesystem::path& aMaterialFile)
{

	FILE* stream;
	auto relativePath = u8"../Assets/" + aMaterialFile.u8string();
	const char* fileName = reinterpret_cast<const char*>(relativePath.c_str());
	fopen_s(&stream, fileName, "rb");

	char readBuffer[10000];
	rapidjson::FileReadStream is(stream, readBuffer, sizeof(readBuffer));

	rapidjson::Document materialData;
	materialData.ParseStream(is);

	if (stream != nullptr)
	{
		fclose(stream);
	}

	myMaterialBuffer.AlbedoTint = {
		materialData["AlbedoTint"]["R"].GetFloat(),
		materialData["AlbedoTint"]["G"].GetFloat(),
		materialData["AlbedoTint"]["B"].GetFloat(),
		materialData["AlbedoTint"]["A"].GetFloat()};

	{
		std::filesystem::path relativeTexturePath = materialData["AlbedoTexturePath"].GetString();
		myAlbedoTexture = std::make_shared<Texture>();
		GraphicsEngine::Get().LoadTexture(relativeTexturePath, *myAlbedoTexture);
	}
	{
		std::filesystem::path relativeTexturePath = materialData["NormalTexturePath"].GetString();
		myNormalTexture = std::make_shared<Texture>();
		GraphicsEngine::Get().LoadTexture(relativeTexturePath, *myNormalTexture);
	}
	{
		std::filesystem::path relativeTexturePath = materialData["MaterialTexturePath"].GetString();
		myMaterialTexture = std::make_shared<Texture>();
		GraphicsEngine::Get().LoadTexture(relativeTexturePath, *myMaterialTexture);
	}
}

Material::~Material()
{
	myAlbedoTexture = nullptr;
	myFXTexture = nullptr;
	myMaterialTexture = nullptr;
	myNormalTexture = nullptr;
}

void Material::SetAlbedoTexture(const std::filesystem::path& aPath)
{
	myAlbedoTexture = std::make_shared<Texture>();
	GraphicsEngine::Get().LoadTexture(aPath, *myAlbedoTexture);
}

void Material::SetNormalTexture(const std::filesystem::path& aPath)
{
	myNormalTexture = std::make_shared<Texture>();
	GraphicsEngine::Get().LoadTexture(aPath, *myNormalTexture);
}

void Material::SetMaterialTexture(const std::filesystem::path& aPath)
{
	myMaterialTexture = std::make_shared<Texture>();
	GraphicsEngine::Get().LoadTexture(aPath, *myMaterialTexture);
}

void Material::SetAlbedoTexture(std::shared_ptr<Texture> aTexture)
{
	myAlbedoTexture = aTexture;
}

void Material::SetNormalTexture(std::shared_ptr<Texture> aTexture)
{
	myNormalTexture = aTexture;
}

void Material::SetMaterialTexture(std::shared_ptr<Texture> aTexture)
{
	myMaterialTexture = aTexture;
}

void Material::SetFXTexture(std::shared_ptr<Texture> aTexture)
{
	myFXTexture = aTexture;
}

MaterialBuffer& Material::GetMaterialBuffer()
{
	return myMaterialBuffer;
}

std::shared_ptr<Texture> Material::GetAlbedoTexture()
{
	return myAlbedoTexture;
}

std::shared_ptr<Texture> Material::GetNormalTexture()
{
	return myNormalTexture;
}

std::shared_ptr<Texture> Material::GetMaterialTexture()
{
	return myMaterialTexture;
}

std::shared_ptr<Texture> Material::GetFXTexture()
{
	return myFXTexture;
}

CU::Vector4f& Material::GetAlbedoTint()
{
	return myMaterialBuffer.AlbedoTint;
}

void Material::SetAlbedoTint(CU::Vector4f aAlbedoTint)
{
	myMaterialBuffer.AlbedoTint = aAlbedoTint;
}
