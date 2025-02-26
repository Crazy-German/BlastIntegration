#pragma once
#include "Font.h"
#include "CommonUtilities/Matrix.hpp"
class Text 
{
	
public:
	struct TextData
	{
		unsigned VertexAmount;
		unsigned IndexAmount;
		unsigned Stride;
		unsigned Offset;
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;
		D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology;
	};

	Text();
	~Text();

	void Init(TextData aTextData, std::wstring aText, Font* aFont, CU::Vector4f aColor = {1,1,1,1});

	FORCEINLINE const TextData& GetTextData() const { return myTextData; }
	FORCEINLINE const CU::Matrix4x4f& GetTransform() const { return myTransform; }

	CU::Matrix4x4f GetTransform();

	void SetTransform(const CU::Matrix4x4f& aTransform);

	void SetRotation(CU::Vector3f aRotation);
	void SetPosition(CU::Vector3f aPosition);
	void SetScale(CU::Vector3f aScale);

	float GetWidth();
	const CU::Vector4f& GetColor() const;
	void SetColor(const CU::Vector4f& aColor);
	

	FORCEINLINE const Font& GetFont() const { return *myFont; }
	FORCEINLINE const std::wstring& GetText() const { return myText; }

private:

	TextData myTextData = {};
	std::wstring myText;
	Font* myFont = nullptr;

	CU::Vector3f myPosition;
	CU::Vector3f myRotation;
	CU::Vector3f myScale;

	CU::Matrix4x4f myTransform;

	CU::Vector4f myColor;


	void UpdateTransform();
};