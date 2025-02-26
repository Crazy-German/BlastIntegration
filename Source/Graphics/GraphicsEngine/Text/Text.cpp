#include "Text.h"
#include "TextFactory.h"

Text::Text()
{
}

Text::~Text()
{
	myTextData.VertexBuffer.Reset();
	myTextData.IndexBuffer.Reset();
	myFont = nullptr;
}

void Text::Init(TextData aTextData, std::wstring aText, Font* aFont, CU::Vector4f aColor)
{
	myTextData = aTextData;
	myText = aText;
	myFont = aFont;
	myColor = aColor;

	myPosition = { 0.0f,0.0f,0 };
	myRotation = { 0,0,0 };
	myScale = { 1,1,1 };

	UpdateTransform();
}

CU::Matrix4x4f Text::GetTransform()
{
	return myTransform;
}

void Text::SetTransform(const CU::Matrix4x4f& aTransform)
{
	myTransform = aTransform;
}

void Text::SetRotation(CU::Vector3f aRotation)
{
	myRotation = aRotation;
	UpdateTransform();
}

void Text::SetPosition(CU::Vector3f aPosition)
{
	myPosition = aPosition;
	UpdateTransform();
}

void Text::SetScale(CU::Vector3f aScale)
{
	myScale = aScale;
	UpdateTransform();
}

float Text::GetWidth()
{
	return TextFactory::GetInstance()->GetWidthOfText(this) * myScale.x;
}

const CU::Vector4f& Text::GetColor() const
{
	return myColor;
}

void Text::SetColor(const CU::Vector4f& aColor)
{
	myColor = aColor;
}

void Text::UpdateTransform()
{
	CU::Matrix4x4<float> translation;
	CU::Matrix4x4<float> rotation;
	CU::Matrix4x4<float> scale;

	translation(4, 1) = myPosition.x; translation(4, 2) = myPosition.y; translation(4, 3) = myPosition.z; translation(4, 4) = 1;

	rotation = rotation.CreateRotationAroundZ(myRotation.z) * rotation.CreateRotationAroundX(myRotation.x) * rotation.CreateRotationAroundY(myRotation.y);

	scale(1, 1) = myScale.x; scale(2, 2) = myScale.y; scale(3, 3) = myScale.z;

	myTransform = scale * rotation * translation;
}
