#include "AssetManager.h"

#include <TGAFbx/include/TGAFbx.h>

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

#include <GraphicsEngine/Mesh.h>
#include "AssetTypes/AssetTypeInclude.h"
//#include "AssetTypes/MeshAsset.h"
//#include "AssetTypes/AnimationAsset.h"
//#include "AssetTypes/DecalAsset.h"
//#include "AssetTypes/MaterialAsset.h"
#include "GraphicsEngine/GraphicsEngine.h"
#include "GraphicsEngine/Texture.h"
#include "nlohmann/json.hpp"
#include <cmath>

#include "CommonUtilities/Serializer.h"

#include "GraphicsEngine/Text/Text.h"
#include "AssetTypes/FontAsset.h"
#include <GraphicsEngine/Text/TextFactory.h>

bool AssetManager::Initialize(const std::filesystem::path& aContentRootPath)
{
	TGA::FBX::Importer::InitImporter();

	myContentRoot = absolute(aContentRootPath);
	if (!exists(aContentRootPath) && !create_directories(aContentRootPath))
	{
		LOG(LogAssetManager, Error, "Content root path doesn't exist or can't be created");
		LOG(LogAssetManager, Error, aContentRootPath.string().data());

		return false;
	}
	CreateDefaultMaterial();
	CreateDefaultCube();
	CreateDefaultShameTriangle();
	CreateDefaultPlane();
	std::filesystem::path materialPath = myContentRoot;
	materialPath.append("Materials");
	for (const auto& file : std::filesystem::recursive_directory_iterator(materialPath))
	{
		if (file.path().has_filename() && file.path().has_extension())
		{
			RegisterAsset(file.path());
		}
	}
	for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
	{
		if (file.path().filename().string() != "Materials")
		{
			if (file.path().has_filename() && file.path().has_extension())
			{
				RegisterAsset(file.path());
			}
		}
	}
	return true;
}

bool AssetManager::ExportFolderToBinary(const std::filesystem::path& aSource, const std::filesystem::path& aTarget,
	const bool aRetainStructure)
{
	TGA::FBX::Importer::InitImporter();

	myContentRoot = absolute(aSource);
	if (!std::filesystem::exists(aSource) && !std::filesystem::create_directories(aSource))
	{
		LOG(LogAssetManager, Error, "Content root path doesn't exist or can't be created");
		return false;
	}
	if (!std::filesystem::exists(aTarget) && !std::filesystem::create_directories(aTarget))
	{
		LOG(LogAssetManager, Error, "Content root path doesn't exist or can't be created");
		return false;
	}
	Serializer writer;
	for (const auto& file : std::filesystem::recursive_directory_iterator(myContentRoot))
	{
		std::filesystem::path assetPath = MakeRelative(file);
		std::filesystem::path targetPath = aTarget;
		auto filename = assetPath.filename().u8string();
		if(aRetainStructure)
		{
			targetPath.append(assetPath.c_str());
		}
		else
		{
			targetPath.append(filename);
		}
		if(!targetPath.has_extension())
		{
			if (!std::filesystem::exists(targetPath) && !std::filesystem::create_directories(targetPath))
			{
				LOG(LogAssetManager, Error, "Content root path doesn't exist or can't be created");
				LOG(LogAssetManager, Error, targetPath.string().data());
				return false;
			}	
		}
		if (filename.rfind(u8"A_", 0) == 0)
		{
			targetPath.replace_extension(Serialization::format);
			writer.Open(std::filesystem::absolute(targetPath), Serializer::Mode::Write);
			std::dynamic_pointer_cast<AnimationAsset>(LoadAnimation(assetPath))->myAnimation->Serialize(writer);
			writer.Flush();
		}
		if (filename.rfind(u8"SK_", 0) == 0 || filename.rfind(u8"SM_", 0) == 0)
		{
			targetPath.replace_extension(Serialization::format);
			writer.Open(targetPath, Serializer::Mode::Write);
			std::dynamic_pointer_cast<MeshAsset>(LoadMesh(assetPath, false))->myMesh->Serialize(writer);
			writer.Flush();
		}
	}

	return true;
}


bool AssetManager::RegisterAsset(const std::filesystem::path& aPath)
{
	if (!aPath.has_extension() || !exists(myContentRoot / aPath))
	{
		LOG(LogAssetManager, Warning, "File doesn't exist {}", aPath.string());
		return false;
	}

	std::filesystem::path assetPath = MakeRelative(aPath);

	const std::string ext = assetPath.extension().string();
	if (ext.ends_with("fbx") || ext.ends_with("FBX"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"A_", 0) == 0)
		{
			return RegisterAnimation(assetPath);
		}
		if (filename.rfind(u8"SK_", 0) == 0)
		{
			return RegisterMesh(assetPath);
		}
		if (filename.rfind(u8"SM_", 0) == 0)
		{
			return RegisterMesh(assetPath);
		}
		//if(filename.rfind(u8"NavMesh") != std::string::npos)
		//{
		//	return RegisterNavMesh(assetPath);
		//}
	}
	if (ext.ends_with("rock") || ext.ends_with("ROCK"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"A_", 0) == 0)
		{
			return RegisterAnimation(assetPath);
		}
		if (filename.rfind(u8"SK_", 0) == 0)
		{
			return RegisterMesh(assetPath);
		}
		if (filename.rfind(u8"SM_", 0) == 0)
		{
			return RegisterMesh(assetPath);
		}
		if(filename.rfind(u8"NavMesh") != std::string::npos)
		{
			return RegisterNavMesh(assetPath);
		}
	}
	if (ext.ends_with("json"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"M_", 0) == 0)
		{
			return RegisterMaterial(assetPath);
		}
		if (filename.rfind(u8"F_", 0) == 0)
		{
			return RegisterFont(assetPath);
		}
	}
	if (ext.ends_with("dirt"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"MAT_", 0) == 0)
		{
			return RegisterMaterialBin(assetPath);
		}
	}
	if (ext.ends_with("dds"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"sprite_", 0) == 0)
		{
			return RegisterSprite(assetPath);
		}
	}
	return false;
}

bool AssetManager::RegisterMesh(const std::filesystem::path& aPath)
{
	/*TGA::FBX::Mesh tgaMesh;
	const TGA::FBX::FbxImportStatus importStatus = TGA::FBX::Importer::LoadMesh(myContentRoot / aPath, tgaMesh);

	auto mesh = GenerateMesh(tgaMesh);

	auto asset = std::make_shared<MeshAsset>();
	asset->myMesh = mesh;
	asset->myPath = aPath;
	asset->myName = aPath.stem();*/
	AssetRegister newAsset;
	//newAsset.myAsset = LoadMesh(aPath);
	newAsset.myPath = aPath;
	newAsset.myType = aPath.filename().string().find("SK") == std::string::npos ? AssetType::Mesh : AssetType::AnimatedMesh;
	myAssets.emplace(toLower(aPath.stem().string()), newAsset);
	return true;
}

std::shared_ptr<Asset> AssetManager::LoadMesh(const std::filesystem::path& aPath, const bool& aCreateBuffers)
{
	auto mesh = std::make_shared<Mesh>();
	if(aPath.extension() == Serialization::format)
	{
		Serializer loader;
		loader.Open(myContentRoot / aPath, false);
		mesh->Serialize(loader);
		mesh->GenerateBuffers(aPath.stem().string());
		loader.Flush();
	}
	else
	{
		TGA::FBX::Mesh tgaMesh;
		const TGA::FBX::FbxImportStatus importStatus = TGA::FBX::Importer::LoadMesh(myContentRoot / aPath, tgaMesh);
		mesh = GenerateMesh(tgaMesh, aCreateBuffers).second;
	}
	auto asset = std::make_shared<MeshAsset>();
	asset->myMesh = mesh;
	asset->myName = aPath.stem();
	return asset;
}

bool AssetManager::RegisterAnimation(const std::filesystem::path& aPath)
{
	/*TGA::FBX::Animation tgaAnimation;
	const TGA::FBX::FbxImportStatus importStatus = TGA::FBX::Importer::LoadAnimation(myContentRoot / aPath, tgaAnimation);

	auto animation = std::make_shared<Animation>();

	animation->Duration = static_cast<float>(tgaAnimation.Duration);
	animation->FramesPerSecond = tgaAnimation.FramesPerSecond;

	for (const auto& frameData : tgaAnimation.Frames)
	{
		Animation::Frame frame;
		for (auto i : frameData.LocalTransforms)
		{
			frame.Transforms.emplace(std::pair(i.first, CU::Matrix4x4f(i.second.Data)));
		}
		animation->Frames.push_back(frame);
	}

	auto asset = std::make_shared<AnimationAsset>();
	asset->myAnimation = animation;
	asset->myPath = aPath;
	asset->myName = aPath.stem();*/
	AssetRegister newAsset;
	newAsset.myAssetLoaded = false;
	newAsset.myPath = aPath;
	newAsset.myType = AssetType::Animation;
	myAssets.emplace(toLower(aPath.stem().string()), newAsset);
	return true;
}

std::string AssetManager::toLower(std::string aInput)
{
	std::transform(aInput.begin(), aInput.end(), aInput.begin(), [](unsigned char c){return std::tolower(c);});
	return  aInput;
}

std::shared_ptr<Asset> AssetManager::LoadAnimation(const std::filesystem::path& aPath) const
{
	auto animation = std::make_shared<Animation>();
	if(aPath.extension() == Serialization::format)
	{
		Serializer loader;
		loader.Open(myContentRoot / aPath, false);
		animation->Serialize(loader);
		loader.Flush();
	}
	else 
	{
			
		TGA::FBX::Animation tgaAnimation;
		const TGA::FBX::FbxImportStatus importStatus = TGA::FBX::Importer::LoadAnimation(myContentRoot / aPath, tgaAnimation);

		
		animation->Duration = static_cast<float>(tgaAnimation.Duration);
		animation->FramesPerSecond = tgaAnimation.FramesPerSecond;

		for (const auto& frameData : tgaAnimation.Frames)
		{
			Animation::Frame frame;
			for (auto i : frameData.LocalTransforms)
			{
				frame.Transforms.emplace(std::pair(i.first, CU::Matrix4x4f(i.second.Data)));
			}
			animation->Frames.push_back(frame);
		}
	}

	auto asset = std::make_shared<AnimationAsset>();
	asset->myAnimation = animation;
	asset->myName = aPath.stem();
	return asset;
}

bool AssetManager::CreateMaterial(std::string_view aMaterialName, std::string_view anAlbedoTexture, std::string_view aNormalTexture, std::string_view aMaterialTexture, std::string_view anFXTexture, const CU::Vector4f& anAlbedoTint, const float& anEmissiveStrength)
{
	if (myAssets.contains(aMaterialName))
	{
		return true;
	}
	
	/*nlohmann::json data;
	data["AlbedoTint"]["R"] = anAlbedoTint.x;
	data["AlbedoTint"]["G"] = anAlbedoTint.y;
	data["AlbedoTint"]["B"] = anAlbedoTint.z;
	data["AlbedoTint"]["A"] = anAlbedoTint.w;
	data["EmissionStrength"] = anEmissiveStrength;
	if (anAlbedoTexture.empty())
	{
		data["AlbedoTexturePath"] = "\\Textures\\Default\\T_Default_C.dds";
	}
	else
	{
		std::filesystem::path albedo = "\\Textures\\" + (std::filesystem::path(anAlbedoTexture).string()) + ".dds";
		data["AlbedoTexturePath"] = albedo.string();
	}
	if (aNormalTexture.empty())
	{
		data["NormalTexturePath"] = "\\Textures\\Default\\T_Default_N.dds";
	}
	else
	{
		std::filesystem::path normal = "\\Textures\\" + (std::filesystem::path(aNormalTexture).string()) + ".dds";
		data["NormalTexturePath"] = normal.string();
	}
	if (aMaterialTexture.empty())
	{
		data["MaterialTexturePath"] = "\\Textures\\Default\\T_Default_M.dds";
	}
	else
	{
		std::filesystem::path material = "\\Textures\\" + (std::filesystem::path(aMaterialTexture).string()) + ".dds";
		data["MaterialTexturePath"] = material.string();
	}
	if (anFXTexture.empty())
	{
		data["EffectsTexturePath"] = "\\Textures\\Default\\T_Default_FX.dds";
	}
	else
	{
		std::filesystem::path fx = "\\Textures\\" + (std::filesystem::path(anFXTexture).string()) + ".dds";
		data["EffectsTexturePath"] = fx.string();
	}


	std::filesystem::path outputPath = (myContentRoot.string() + "\\Materials\\");
	std::filesystem::path file = aMaterialName;
	std::filesystem::path extension = ".json";
	std::filesystem::path outputFile = (outputPath.string() + file.string() + extension.string());
	std::ofstream output_file(outputFile.string().c_str());
	if (!output_file.is_open())
	{
		LOG(LogAssetManager, Error, "Failed to open up material-JSON file");
		return false;
	}
	else
	{
		output_file << data;
		output_file.close();
	}*/
	if (!RegisterMaterial(aMaterialName, anAlbedoTexture, aNormalTexture, aMaterialTexture, anFXTexture, anAlbedoTint, anEmissiveStrength))
	{
		LOG(LogAssetManager, Error, "Failed to register material");
		return false;
	}

	return true;
}

bool AssetManager::RegisterMaterial(const std::filesystem::path& aPath)
{
	AssetRegister newAsset;
	//newAsset.myAsset = LoadMaterial(aPath);
	newAsset.myPath = aPath;
	newAsset.myType = AssetType::Material;
	myAssets.emplace(toLower(aPath.stem().string()), newAsset);
	return true;
}

std::shared_ptr<Asset> AssetManager::LoadMaterial(const std::filesystem::path& aPath)
{
	auto asset = std::make_shared<MaterialAsset>();
	asset->myMaterial = std::make_shared<Material>();
	asset->myName = aPath.stem();
	std::ifstream input(myContentRoot/aPath);
	if (!input.is_open())
	{
		printf("Error Could not open material file");
		return asset;
	}
	nlohmann::json data;
	data = nlohmann::json::parse(input);
	input.close();
	if (data.contains("AlbedoTint"))
	{
		auto& albedo = asset->myMaterial->GetMaterialBuffer().AlbedoTint;
		albedo.x = data["AlbedoTint"]["R"];
		albedo.y = data["AlbedoTint"]["G"];
		albedo.z = data["AlbedoTint"]["B"];
		albedo.w = data["AlbedoTint"]["A"];
	}
	asset->myMaterial->GetAlbedoTexture() = std::make_shared<Texture>();
	if (data.contains("AlbedoTexturePath"))
	{
		std::filesystem::path anoy = myContentRoot;
		std::filesystem::path temp = std::filesystem::path(std::string(data["AlbedoTexturePath"]));
		anoy += temp;
		if (MakeRelative(anoy).extension() == ".dds")
		{
			if(GraphicsEngine::Get().LoadTexture(anoy, *asset->myMaterial->GetAlbedoTexture()))
			{
				asset->myMaterial->GetAlbedoTexture() = std::dynamic_pointer_cast<MaterialAsset>(myAssets.at("DefaultMaterial").myAsset)->myMaterial->GetAlbedoTexture();
			}
		}
		else
		{
			LOG(LogAssetManager, Error, "Texture not dds format!");
		}
	}
	asset->myMaterial->GetNormalTexture() = std::make_shared<Texture>();
	if (data.contains("NormalTexturePath"))
	{
		std::filesystem::path contentRoot = myContentRoot;
		std::filesystem::path temp = std::filesystem::path(std::string(data["NormalTexturePath"]));
		contentRoot += temp;
		if (MakeRelative(contentRoot).extension() == ".dds")
		{
			if(GraphicsEngine::Get().LoadTexture(contentRoot, *asset->myMaterial->GetNormalTexture()))
			{
				asset->myMaterial->GetNormalTexture() = std::dynamic_pointer_cast<MaterialAsset>(myAssets.at("DefaultMaterial").myAsset)->myMaterial->GetNormalTexture();
			}
		}
		else if (MakeRelative(contentRoot).extension() == ".h")
		{
			//GraphicsEngine::Get().LoadTexture(anoy, *newMaterial->myTexture.GetNormalMap());
		}
		else
		{
			std::cout << "Texture not dds format!\n";
		}
	}
	asset->myMaterial->GetMaterialTexture() = std::make_shared<Texture>();
	if (data.contains("MaterialTexturePath"))
	{
		std::filesystem::path anoy = myContentRoot;
		std::filesystem::path temp = std::filesystem::path(std::string(data["MaterialTexturePath"]));
		anoy += temp;
		if (MakeRelative(anoy).extension() == ".dds")
		{
			if(!GraphicsEngine::Get().LoadTexture(anoy, *asset->myMaterial->GetMaterialTexture()))
			{
				asset->myMaterial->GetMaterialTexture() = std::dynamic_pointer_cast<MaterialAsset>(myAssets.at("DefaultMaterial").myAsset)->myMaterial->GetMaterialTexture();
			}
		}
		else if (MakeRelative(anoy).extension() == ".h")
		{
			//GraphicsEngine::Get().LoadTexture(anoy, *newMaterial->myTexture.GetNormalMap());
		}
		else
		{
			std::cout << "Texture not dds format!\n";
		}
	}
	return asset;
}

bool AssetManager::RegisterMaterial(std::string_view aMaterialName, std::string_view anAlbedoTexture,
	std::string_view aNormalTexture, std::string_view aMaterialTexture, std::string_view anFXTexture,
	const CU::Vector4f& anAlbedoTint, const float& anEmissiveStrength)
{
	anAlbedoTexture;aNormalTexture;aMaterialTexture;anFXTexture;anAlbedoTint;anEmissiveStrength;
	AssetRegister newAsset;
	//newAsset.myAsset = LoadMaterial(aMaterialName, anAlbedoTexture, aNormalTexture, aMaterialTexture, anFXTexture, anAlbedoTint, anEmissiveStrength);
	newAsset.myPath = aMaterialName;
	newAsset.myType = AssetType::Material;
	myAssets.emplace(aMaterialName, newAsset);
	return true;
}

std::shared_ptr<Asset> AssetManager::LoadMaterial(std::string_view aMaterialName, std::string_view anAlbedoTexture,
	std::string_view aNormalTexture, std::string_view aMaterialTexture, std::string_view /*anFXTexture*/,
	const CU::Vector4f& anAlbedoTint, const float& anEmissiveStrength)
{
	anEmissiveStrength;
	auto asset = std::make_shared<MaterialAsset>();
	asset->myMaterial = std::make_shared<Material>();
	asset->myName = aMaterialName;

	asset->myMaterial->GetMaterialBuffer().AlbedoTint = anAlbedoTint;

	asset->myMaterial->GetAlbedoTexture() = std::make_shared<Texture>();
	std::filesystem::path contentRoot = myContentRoot;
	std::filesystem::path temp = anAlbedoTexture;
	contentRoot += temp;
	if (MakeRelative(contentRoot).extension() == ".dds")
	{
		if(GraphicsEngine::Get().LoadTexture(contentRoot, *asset->myMaterial->GetAlbedoTexture()))
		{
			asset->myMaterial->GetAlbedoTexture() = std::dynamic_pointer_cast<MaterialAsset>(myAssets.at("DefaultMaterial").myAsset)->myMaterial->GetAlbedoTexture();
		}
	}
	else
	{
		LOG(LogAssetManager, Error, "Texture not dds format!");
	}

	asset->myMaterial->GetNormalTexture() = std::make_shared<Texture>();
	contentRoot = myContentRoot;
	temp = aNormalTexture;
	contentRoot += temp;
	if (MakeRelative(contentRoot).extension() == ".dds")
	{
		if(GraphicsEngine::Get().LoadTexture(contentRoot, *asset->myMaterial->GetNormalTexture()))
		{
			asset->myMaterial->GetNormalTexture() = std::dynamic_pointer_cast<MaterialAsset>(myAssets.at("DefaultMaterial").myAsset)->myMaterial->GetNormalTexture();
		}
	}
	else
	{
		std::cout << "Texture not dds format!\n";
	}
	asset->myMaterial->GetMaterialTexture() = std::make_shared<Texture>();
	contentRoot = myContentRoot;
	temp = aMaterialTexture;
	contentRoot += temp;
	if (MakeRelative(contentRoot).extension() == ".dds")
	{
		if(!GraphicsEngine::Get().LoadTexture(contentRoot, *asset->myMaterial->GetMaterialTexture()))
		{
			asset->myMaterial->GetMaterialTexture() = std::dynamic_pointer_cast<MaterialAsset>(myAssets.at("DefaultMaterial").myAsset)->myMaterial->GetMaterialTexture();
		}
	}
	else
	{
		std::cout << "Texture not dds format!\n";
	}
	return asset;
}

bool AssetManager::RegisterSprite(const std::filesystem::path& aPath)
{
	AssetRegister newAsset;
	//newAsset.myAsset = LoadSprite(aPath);
	newAsset.myPath = aPath;
	newAsset.myType = AssetType::Sprite;
	myAssets.emplace(toLower(aPath.stem().string()), newAsset);
	return true;
}

std::shared_ptr<Asset> AssetManager::LoadSprite(const std::filesystem::path& aPath)
{
	auto asset = std::make_shared<SpriteAsset>();
	asset->myTexture = std::make_shared<Texture>();
	asset->myName = aPath.stem();

	std::ifstream input(myContentRoot / aPath);
	if (!input.is_open())
	{
		printf("Error Could not open sprite file");
		return asset;
	}

	std::filesystem::path anoy = myContentRoot;
	anoy += "\\";
	std::filesystem::path temp = aPath;
	anoy += temp;
	{
		if (GraphicsEngine::Get().LoadTexture(anoy, *asset->myTexture))
		{
			
		}
	}
	
	return asset;
}

bool AssetManager::RegisterFont(const std::filesystem::path& aPath)
{
	AssetRegister newAsset;
	//newAsset.myAsset = LoadFont(aPath);
	newAsset.myPath = aPath;
	newAsset.myType = AssetType::Font;
	myAssets.emplace(toLower(aPath.stem().string()), newAsset);
	return true;
}

std::shared_ptr<Asset> AssetManager::LoadFont(const std::filesystem::path& aPath)
{
	auto asset = std::make_shared<FontAsset>();
	asset->myFont = std::make_shared<Font>();
	asset->myFont->textTexture = std::make_shared<Texture>();
	asset->myName = aPath.stem();

	std::ifstream input(myContentRoot / aPath);
	if (!input.is_open())
	{
		printf("Error Could not open sprite file");
		return asset;
	}

	nlohmann::json data;
	data = nlohmann::json::parse(input);
	input.close();

	if (data.contains("atlasPath"))
	{
		std::filesystem::path anoy = myContentRoot;
		std::filesystem::path temp = std::filesystem::path(std::string(data["atlasPath"]));
		anoy += temp;
		if (MakeRelative(anoy).extension() == ".dds")
		{
			if (GraphicsEngine::Get().LoadTexture(anoy, *asset->myFont->textTexture))
			{
				
			}
		}
		else
		{
			LOG(LogAssetManager, Error, "Font atlas not dds format!");
		}
	}
	std::filesystem::path fontDefinitionPath = myContentRoot;
	fontDefinitionPath += "\\";
	std::filesystem::path temp = aPath;
	fontDefinitionPath += aPath;

	TextFactory::GetInstance()->LoadFont(fontDefinitionPath, asset->myFont);
	
	return asset;
}

bool AssetManager::RegisterMaterialBin(std::filesystem::path& aPath)
{
	struct paths
	{
		std::filesystem::path albedo;
		std::filesystem::path normal;
		std::filesystem::path material;
	};
	std::ifstream input(myContentRoot / aPath, std::ios::in | std::ios::binary);
	if (!input)
	{
		std::cerr << "Could not open file " << aPath << "\n";
		return false;
	}

	paths material;
	std::streamsize size;

	// Read albedo path
	input.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	if (size > 0)
	{
		std::string albedoPath(size, '\0');
		input.read(&albedoPath[0], size);
		material.albedo = albedoPath;
	}

	// Read normal path
	input.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	if (size > 0)
	{
		std::string normalPath(size, '\0');
		input.read(&normalPath[0], size);
		material.normal = normalPath;
	}

	// Read material path
	input.read(reinterpret_cast<char*>(&size), sizeof(size_t));
	if (size > 0)
	{
		std::string materialPath(size, '\0');
		input.read(&materialPath[0], size);
		material.material = materialPath;
	}

	input.close();
	AssetRegister newAsset;
	auto asset = std::make_shared<MaterialAsset>();
	asset->myMaterial = std::make_shared<Material>();
	asset->myMaterial->GetMaterialBuffer().AlbedoTint = { 1,1,1,1 };
	GraphicsEngine::Get().LoadTexture(material.albedo, *asset->myMaterial->GetAlbedoTexture());
	GraphicsEngine::Get().LoadTexture(material.normal, *asset->myMaterial->GetNormalTexture());
	GraphicsEngine::Get().LoadTexture(material.material, *asset->myMaterial->GetMaterialTexture());
	asset->myName = aPath.stem();
	newAsset.myAsset = asset;
	newAsset.myType = AssetType::Material;
	newAsset.myPath = aPath;
	myAssets.emplace(toLower(aPath.stem().string()), newAsset);
	return true;
}



void AssetManager::CreateDefaultMaterial()
{
	AssetRegister newAsset;
	newAsset.myType = AssetType::Material;
	std::shared_ptr<MaterialAsset> DefaultMaterial = std::make_shared<MaterialAsset>();
	DefaultMaterial->myName = "Default";
	DefaultMaterial->myMaterial = std::make_shared<Material>();
	DefaultMaterial->myMaterial->SetAlbedoTint({1,1,1,1});
	std::filesystem::path defaultAlbdeo("/Textures/Default/T_Default_C.dds");
	std::filesystem::path path = myContentRoot;
	path+=defaultAlbdeo;
	GraphicsEngine::Get().LoadTexture(path, *DefaultMaterial->myMaterial->GetAlbedoTexture());
	path = myContentRoot;
	path += L"/Textures/Default/T_Default_N.dds";
	GraphicsEngine::Get().LoadTexture(path, *DefaultMaterial->myMaterial->GetNormalTexture());

	path = myContentRoot;
	path += L"/Textures/Default/T_Default_M.dds";
	GraphicsEngine::Get().LoadTexture(path, *DefaultMaterial->myMaterial->GetMaterialTexture());

	newAsset.myAsset = DefaultMaterial;
	newAsset.myPath = L"DefaultMaterial";
	newAsset.myAssetLoaded = true;
	myAssets.emplace(newAsset.myPath, newAsset);
}

void AssetManager::CreateDefaultCube()
{
	AssetRegister newAsset;
	std::shared_ptr<MeshAsset> DefaultCube = std::make_shared<MeshAsset>();
	DefaultCube->myMesh = std::make_shared<Mesh>();
	DefaultCube->myMesh->GenerateCube(100.0f);
	DefaultCube->myName = "DefaultCube";
	newAsset.myPath = "DefaultCube";
	newAsset.myType = AssetType::Mesh;
	newAsset.myAsset = DefaultCube;
	newAsset.myAssetLoaded = true;

	myAssets.emplace(toLower(newAsset.myPath.string()), newAsset);
}

void AssetManager::CreateDefaultShameTriangle()
{
	AssetRegister newAsset;
	std::shared_ptr<MeshAsset> DefaultTriangle = std::make_shared<MeshAsset>();
	DefaultTriangle->myMesh = std::make_shared<Mesh>();
	DefaultTriangle->myMesh->GenerateHorizontalTriangle(100.f);
	DefaultTriangle->myName = "TriangleOfShame";
	newAsset.myPath = "TriangleOfShame";
	newAsset.myType = AssetType::Mesh;
	newAsset.myAsset = DefaultTriangle;
	newAsset.myAssetLoaded = true;

	myAssets.emplace(toLower(newAsset.myPath.string()), newAsset);
}

void AssetManager::CreateDefaultPlane()
{
	AssetRegister newAsset;
	std::shared_ptr<MeshAsset> plane = std::make_shared<MeshAsset>();
	plane->myMesh = std::make_shared<Mesh>();
	plane->myMesh->GenerateHorizontalPlane(100.0f);
	plane->myName = "Plane";
	newAsset.myPath = "Plane";
	newAsset.myType = AssetType::Mesh;
	newAsset.myAsset = plane;
	newAsset.myAssetLoaded = true;

	myAssets.emplace(toLower(newAsset.myPath.string()), newAsset);
}

void AssetManager::Reset()
{
	myAssets.clear();
	CreateDefaultMaterial();
}

AssetManager::AssetManager() = default;
AssetManager::~AssetManager() = default;

std::pair<bool, std::shared_ptr<Mesh>> AssetManager::GenerateMesh(TGA::FBX::Mesh& aTgaMesh, const bool& aCreateBuffers)
{
	Skeleton skeleton;
	GenerateSkeleton(aTgaMesh, skeleton);
	std::vector<Vertex> vertices{};
	std::vector<unsigned> indices{};
	std::vector<Mesh::Element> elements{};

	for (auto& tgaElement : aTgaMesh.Elements)
	{
		Mesh::Element element;
		element.VertexOffset = static_cast<unsigned>(vertices.size());
		element.IndexOffset = static_cast<unsigned>(indices.size());
		element.NumIndices = static_cast<unsigned>(tgaElement.Indices.size());
		element.NumVertices = static_cast<unsigned>(tgaElement.Vertices.size());
		element.MaterialIndex = tgaElement.MaterialIndex;

		for (const auto& vertex : tgaElement.Vertices)
		{
			CU::Vector4f position = { vertex.Position[0], vertex.Position[1], vertex.Position[2], vertex.Position[3] };
			CU::Vector4<unsigned> boneIDs = { vertex.BoneIDs[0], vertex.BoneIDs[1], vertex.BoneIDs[2], vertex.BoneIDs[3] };
			CU::Vector4f skinWeights = { vertex.BoneWeights[0], vertex.BoneWeights[1], vertex.BoneWeights[2], vertex.BoneWeights[3] };
			CU::Vector2f UV = { vertex.UVs[0][0], vertex.UVs[0][1] };
			CU::Vector3f normal = { vertex.Normal[0], vertex.Normal[1], vertex.Normal[2] };
			CU::Vector3f tangent = { vertex.Tangent[0], vertex.Tangent[1], vertex.Tangent[2] };
			CU::Vector4f color = { vertex.VertexColors[0][0], vertex.VertexColors[0][1], vertex.VertexColors[0][2], vertex.VertexColors[0][3] };

			if (abs(normal.x) <= 0.001f && abs(normal.y) <= 0.001f && abs(normal.z) <= 0.001f)
			{
				//std::cout << "gotcha normal";
				normal.x = 1.0f;
				normal.y = 1.0f;
				normal.z = 1.0f;
				normal.Normalize();

				CU::Vector3f arbittangent = {0, 0, 0};

				// Pick an arbitrary vector to calculate the tangent
				CU::Vector3f arbitraryVec = { 0,0,0 };
				if (std::fabs(normal.x) > std::fabs(normal.z)) {
					arbitraryVec = { 0.0f, 0.0f, 1.0f };  // Use Z-axis as arbitrary vector
				}
				else {
					arbitraryVec = { 1.0f, 0.0f, 0.0f };  // Use X-axis as arbitrary vector
				}

				tangent.x = normal.y * arbitraryVec.z - normal.z * arbitraryVec.y;
				tangent.y = normal.z * arbitraryVec.x - normal.x * arbitraryVec.z;
				tangent.z = normal.x * arbitraryVec.y - normal.y * arbitraryVec.x;

				tangent.Normalize();
			}
			else if (abs(tangent.x) <= 0.001f && abs(tangent.y) <= 0.001f && abs(tangent.z) <= 0.001f)
			{
				//std::cout << "gotcha Tangent";
				CU::Vector3f arbitraryVec = { 0,0,0 };
				tangent.x = normal.y * arbitraryVec.z - normal.z * arbitraryVec.y;
				tangent.y = normal.z * arbitraryVec.x - normal.x * arbitraryVec.z;
				tangent.z = normal.x * arbitraryVec.y - normal.y * arbitraryVec.x;

				tangent.Normalize();
			}

			vertices.emplace_back(Vertex(position, color, boneIDs, skinWeights, UV , normal, tangent));
		}

		for (unsigned index : tgaElement.Indices)
		{
			indices.push_back(index + element.VertexOffset);
		}

		elements.push_back(element);
	}
	auto mesh = std::make_shared<Mesh>();
	if(aCreateBuffers)
	{
		mesh->Initialize(std::move(vertices), std::move(indices), std::move(elements), std::move(skeleton), aTgaMesh.Name);
	}
	else
	{
		mesh->InitializeNoBuffer(std::move(vertices), std::move(indices), std::move(elements), std::move(skeleton), aTgaMesh.Name);
	}
	
	return std::pair<bool, std::shared_ptr<Mesh>>(false,mesh);
}

void AssetManager::GenerateSkeleton(TGA::FBX::Mesh& aTgaMesh, Skeleton& outSkeleton)
{
	if (aTgaMesh.Skeleton.GetRoot())
	{
		outSkeleton.Joints.resize(aTgaMesh.Skeleton.Bones.size());
		outSkeleton.JointNameToIndex.reserve(outSkeleton.Joints.size());
		for (size_t j = 0; j < aTgaMesh.Skeleton.Bones.size(); j++)
		{
			Skeleton::Joint& joint = outSkeleton.Joints[j];
			TGA::FBX::Skeleton::Bone& tgaJoint = aTgaMesh.Skeleton.Bones[j];

			CU::Matrix4x4f bindPoseInverseTranspose;
			memcpy(&bindPoseInverseTranspose, &tgaJoint.BindPoseInverse, sizeof(float) * 16);

			joint.BindPoseInverse = bindPoseInverseTranspose.GetTranspose();
			joint.Name = tgaJoint.Name;
			joint.Parent = tgaJoint.ParentIdx;
			joint.Children = tgaJoint.Children;

			outSkeleton.JointNameToIndex.emplace(joint.Name, static_cast<unsigned>(j));
			outSkeleton.IndexToJointName.emplace(static_cast<unsigned>(j), joint.Name);
			outSkeleton.Joints[j].Name = joint.Name;
		}
	}
}

std::shared_ptr<Asset> AssetManager::LoadAsset(const std::filesystem::path& aPath)
{
	std::shared_ptr<Asset> retVal;
	if(aPath.stem().string().find("SM_") == 0 || aPath.stem().string().find("SK_") == 0)
	{
		retVal = LoadMesh(aPath);
	}
	else if(aPath.stem().string().find("A_") == 0)
	{
		//auto path = "Animations/"/aPath/".fbx";
		retVal = LoadAnimation(aPath);
	}
	else if(aPath.stem().string().find("M_") == 0)
	{
		auto path = "Materials/"/aPath/".json";
		retVal = LoadMaterial(aPath);
	}
	else
	{
		LOG(LogAssetManager, Warning, "Asset of invalid type");
	}
	return retVal;
}

std::shared_ptr<Asset> AssetManager::LoadAsset(const AssetRegister& aAssetRegister)
{
	if (!aAssetRegister.myPath.has_extension() || !exists(myContentRoot / aAssetRegister.myPath))
	{
		LOG(LogAssetManager, Warning, "File doesn't exist {}", aAssetRegister.myPath.string());
		return nullptr;
	}

	std::filesystem::path assetPath = MakeRelative(aAssetRegister.myPath);

	const std::string ext = assetPath.extension().string();
	if ((ext.ends_with("rock") || ext.ends_with("ROCK")) || ext.ends_with("fbx") || ext.ends_with("FBX"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"A_", 0) == 0)
		{
			return LoadAnimation(assetPath);
		}
		if (filename.rfind(u8"SK_", 0) == 0 ||filename.rfind(u8"SM_", 0) == 0)
		{
			return LoadMesh(assetPath);
		}

	}
	if (ext.ends_with("json"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"M_", 0) == 0)
		{
			return LoadMaterial(assetPath);
		}
		if (filename.rfind(u8"F_", 0) == 0)
		{
			return LoadFont(assetPath);
		}
	}
	if (ext.ends_with("dirt"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"MAT_", 0) == 0)
		{
			//return LoadMaterial(assetPath);
		}
	}
	if (ext.ends_with("dds"))
	{
		auto filename = assetPath.filename().u8string();
		if (filename.rfind(u8"sprite_", 0) == 0)
		{
			return LoadSprite(assetPath);
		}
	}
	return nullptr;
}

std::filesystem::path AssetManager::MakeRelative(const std::filesystem::path& aPath)
{
	std::filesystem::path targetPath = relative(aPath, myContentRoot);
	targetPath = myContentRoot / targetPath;
	targetPath = weakly_canonical(targetPath);

	auto [rootEnd, nothing] = std::mismatch(
		myContentRoot.begin(),
		myContentRoot.end(),
		targetPath.begin());

	if (rootEnd != myContentRoot.end())
	{
		throw std::invalid_argument("Path is not bellow the root");
	}

	if (aPath.is_absolute())
	{
		return relative(aPath, myContentRoot);
	}

	return aPath;
}

//bool AssetManager::RegisterDecal(const std::filesystem::path& aPath)
//{
//	DecalAsset asset;
//	Texture texture = {};
//	std::filesystem::path path = myContentRoot.string() + "\\" + aPath.string();
//	if(!GraphicsEngine::Get().LoadTexture(path, texture))
//	{
//		LOG(LogAssetManager, Error, "Couldn't load decal {} from memory", aPath.string());
//		return false;
//	}
//	asset.myTexture = std::make_shared<Texture>(texture);
//	asset.myPath = path;
//	AssetRegister newAsset;
//	newAsset.myAsset = std::make_shared<DecalAsset>(asset);
//	newAsset.myPath = aPath;
//	myAssets.emplace(aPath.stem(), newAsset);
//	return true;
//}
