#pragma once
#include <filesystem>

#include "MaterialBuffer.h"

struct PipelineStateObject;
class Texture;

class Material
{
public:
	Material();
	Material(const std::filesystem::path& aMaterialFile);
	~Material();
	MaterialBuffer& GetMaterialBuffer();

	void SetAlbedoTexture(const std::filesystem::path& aPath);
	void SetNormalTexture(const std::filesystem::path& aPath);
	void SetMaterialTexture(const std::filesystem::path& aPath);

	void SetAlbedoTexture(std::shared_ptr<Texture> aTexture);
	void SetNormalTexture(std::shared_ptr<Texture> aTexture);
	void SetMaterialTexture(std::shared_ptr<Texture> aTexture);
	void SetFXTexture(std::shared_ptr<Texture> aTexture);

	std::shared_ptr<Texture> GetAlbedoTexture();
	std::shared_ptr<Texture> GetNormalTexture();
	std::shared_ptr<Texture> GetMaterialTexture();
	std::shared_ptr<Texture> GetFXTexture();

	CU::Vector4f& GetAlbedoTint();
	void SetAlbedoTint(CU::Vector4f aAlbedoTint);

private:
	MaterialBuffer myMaterialBuffer;
	std::shared_ptr<Texture> myAlbedoTexture =nullptr;
	std::shared_ptr<Texture> myNormalTexture = nullptr;
	std::shared_ptr<Texture> myMaterialTexture = nullptr;
	std::shared_ptr<Texture> myFXTexture = nullptr;
};

