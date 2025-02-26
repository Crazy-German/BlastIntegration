#pragma once
#include "SpriteVertex.h"
#include "Material.h"
#include "wrl.h"
//#include "SpriteAnchor.h"

class Texture;

struct ID3D11Buffer;


enum class AnchorPoint
{
	Center,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left
};



class Sprite2D
{
public:

	Sprite2D() = default;
	~Sprite2D();

	//remove material once sprite sstuff is verifiable

	void InitRelative(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<Texture> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);
	void InitRelative(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<Texture> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);

	void InitAbsolute(CU::Vector2<float> aPosition, CU::Vector2<float> aSize, std::shared_ptr<Texture> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);
	void InitAbsolute(float aX, float aY, float aWidth, float aHeight, std::shared_ptr<Texture> aTexture, AnchorPoint aAnchor = AnchorPoint::Center);

	void SetPositionRelative(CU::Vector2<float> aPosition);
	void SetPositionRelative(float aX, float aY);

	void SetPosition(CU::Vector2<float> aPosition);
	void SetPosition(float aX, float aY);

	void SetColor(CU::Vector4f aColor);
	void SetColor(CU::Vector3f aColor, float aAlpha);
	void SetColor(float aR, float aG, float aB, float aA);

	void SetColor(CU::Vector3f aColor);
	void SetColor(float aR, float aG, float aB);
	void SetAlpha(float aAlpha);

	CU::Vector2f GetPosition();
	CU::Vector2f GetSize();
	float GetLeft();
	float GetRight();
	float GetTop();
	float GetBottom();

	void SetAnchor(AnchorPoint aAnchor);

	void SetSourceRectangle(CU::Vector2f aStart, CU::Vector2f aSize);

	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const;
	std::shared_ptr<Texture> GetTexture() const;

	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	AnchorPoint myAnchor;
	CU::Vector2f myPosition;
	CU::Vector2f mySize;

	CU::Vector4f myColor;
	
	//std::shared_ptr<Material> myMaterial;

	//TODO: make sprite use singular texture instead of a whole ass material
	std::shared_ptr<Texture> myTexture;
	std::vector<SpriteVertex> myVertices;
};

