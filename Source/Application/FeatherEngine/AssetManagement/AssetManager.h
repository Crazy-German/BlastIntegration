#pragma once
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <ranges>

#include "AssetTypes/Asset.h"
#include "Logger/Logger.h"
#include "CommonUtilities\Vector4.hpp"
#include "mutex"

#if _DEBUG

DECLARE_LOG_CATEGORY_WITH_NAME(LogAssetManager, AssetManager, Verbose);
#else
DECLARE_LOG_CATEGORY_WITH_NAME(LogAssetManager, AssetManager, Warning);
#endif

inline DEFINE_LOG_CATEGORY(LogAssetManager);

namespace TGA
{
	namespace FBX
	{
		struct Mesh;
	}
}

class Mesh;
struct Skeleton;
struct Asset;
struct AssetRegister
{
	std::filesystem::path myPath;
	bool myAssetLoaded = false;
	AssetType myType;
	std::shared_ptr<Asset> myAsset;
};

class AssetManager
{
public:
	static AssetManager& Get()
	{
		static AssetManager myInstance;
		return myInstance;
	}
	template<typename T>
	std::shared_ptr<T> GetAsset(const std::filesystem::path& aPath);
	template<typename T>
	std::shared_ptr<T> GetAsset(const std::string& aName);
	template<typename T>
	std::shared_ptr<T> GetAsset(const char* aName);

	bool Initialize(const std::filesystem::path& aContentRootPath);

	bool ExportFolderToBinary(const std::filesystem::path& aSource, const std::filesystem::path& aTarget, const bool aRetainStructure);

	bool RegisterMaterialBin(std::filesystem::path& aPath);
	bool RegisterTexture(std::filesystem::path& aPath);
	bool RegisterAsset(const std::filesystem::path& aPath);
	
	const std::filesystem::path& GetContentRoot() { return myContentRoot; }

	std::filesystem::path MakeRelative(const std::filesystem::path& aPath);
	bool CreateMaterial(std::string_view aMaterialName, std::string_view anAlbedoTexture, std::string_view aNormalTexture, std::string_view aMaterialTexture, std::string_view anFXTexture, const CU::Vector4f& anAlbedoTint = { 1.0f, 1.0f, 1.0f, 1.0f }, const float& anEmissiveStrength = 3.0f);


	template <typename Type>
	std::vector<std::shared_ptr<Type>> GetAllAssetsOfType(AssetType aAssetType);

	void Reset();
private:
	AssetManager();
	~AssetManager();

	std::string toLower(std::string aInput);

	static std::pair<bool, std::shared_ptr<Mesh>> GenerateMesh(TGA::FBX::Mesh& aTgaMesh,
	                                                           const bool& aCreateBuffers = true);
	static void GenerateSkeleton(TGA::FBX::Mesh& aTgaMesh, Skeleton& outSkeleton);

	std::shared_ptr<Asset> LoadAsset(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadTexture(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadAsset(const AssetRegister& aAssetRegister);

	bool RegisterMesh(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadMesh(const std::filesystem::path& aPath, const bool& aCreateBuffers = true);

	bool RegisterAnimation(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadAnimation(const std::filesystem::path& aPath) const;

	bool RegisterMaterial(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadMaterial(const std::filesystem::path& aPath);
	bool RegisterMaterial(std::string_view aMaterialName, std::string_view anAlbedoTexture, std::string_view aNormalTexture, std::string_view aMaterialTexture, std::string_view anFXTexture, const CU::Vector4f& anAlbedoTint = { 1.0f, 1.0f, 1.0f, 1.0f }, const float& anEmissiveStrength = 3.0f);
	std::shared_ptr<Asset> LoadMaterial(std::string_view aMaterialName, std::string_view anAlbedoTexture, std::string_view aNormalTexture, std::string_view aMaterialTexture, std::string_view anFXTexture, const CU::Vector4f& anAlbedoTint = { 1.0f, 1.0f, 1.0f, 1.0f }, const float& anEmissiveStrength = 3.0f);

	bool RegisterSprite(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadSprite(const std::filesystem::path& aPath);

	bool RegisterFont(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadFont(const std::filesystem::path& aPath);

	bool RegisterColliderAsset(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadCollider(const std::filesystem::path& aPath);

	bool RegisterScriptAsset(const std::filesystem::path& aPath);
	std::shared_ptr<Asset> LoadScript(const std::filesystem::path& aPath);

	std::unordered_map<std::filesystem::path, AssetRegister> myAssets;

	void CreateDefaultMaterial();
	void CreateDefaultCube();
	void CreateDefaultShameTriangle();
	void CreateDefaultPlane();
	void CreateDefaultCollider();

	std::filesystem::path myContentRoot;
};

template <typename T>
std::shared_ptr<T> AssetManager::GetAsset(const std::filesystem::path& aPath)
{
	std::string key = toLower(aPath.string());
	if (!myAssets.contains(key))
	{
		if (std::dynamic_pointer_cast<T>(myAssets.at(L"defaultcube").myAsset)) // Very temp this shit is illegal
		{
			std::cout << aPath.string() << " is not a valid Mesh. Here have this cube instead\n";
			key = "defaultcube";
		}
		else if (std::dynamic_pointer_cast<T>(myAssets.at(L"t_default_c").myAsset))
		{
			std::cout << aPath.string() << " is not a valid texture. Here have this default instead\n";
			key = "t_default_c";
		}
		else if(aPath.string().starts_with("U"))
		{
			std::cout << aPath.string() << " is not a valid collider. Here have this default instead\n";
			key = "collisionbox";
		}
		else
		{
			std::cout << aPath.string() << " is not a valid material. Here have this default instead\n";
			key = "DefaultMaterial";
		}
	}
	AssetRegister& active = myAssets.at(key);

	if(!active.myAssetLoaded)
	{
		active.myAsset = LoadAsset(active);
		active.myAssetLoaded = true;
	}
	/*if(!active.myAsset.expired()) // With the current structure just using shared_ptr are simpler
	{
		asset = std::dynamic_pointer_cast<T>(active.myAsset.lock());
	}
	else
	{
		auto temp = LoadAsset(active.myPath);
		if(temp)
		{

			active.myAsset = temp;
			if(active.myAsset.expired())
			{
				std::cout << "Error asset expires immediately";
			}
			asset = std::dynamic_pointer_cast<T>(active.myAsset.lock());
		}
	}*/
	return std::dynamic_pointer_cast<T>(active.myAsset);

}

template <typename T>
std::shared_ptr<T> AssetManager::GetAsset(const std::string& aName)
{
	std::filesystem::path aPath(aName);
	return GetAsset<T>(aPath);
}

template <typename T>
std::shared_ptr<T> AssetManager::GetAsset(const char* aName)
{
	return GetAsset<T>(std::filesystem::path(aName));
}

template <typename Type>
std::vector<std::shared_ptr<Type>> AssetManager::GetAllAssetsOfType(AssetType aAssetType)
{
	std::vector<std::shared_ptr<Type>> retVal;
	for (auto& val : myAssets | std::views::values)
	{
		if (val.myAsset->myType == aAssetType)
		{
			retVal.push_back(std::dynamic_pointer_cast<Type>(val));
		}
	}
	return retVal;
}
