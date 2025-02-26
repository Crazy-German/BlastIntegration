#pragma once
#include "Text.h"
#include "../GraphicsEngine.h"

class TextFactory
{
public:
	TextFactory();

	static TextFactory* GetInstance() { if (!myInstance) { myInstance = new TextFactory(); myInstance->Init(); } return myInstance; }

	void LoadFont(const std::wstring aFontPath, std::shared_ptr<Font> aOutFont);

	Text* CreateText(const std::wstring aText, const int aWorldSize);
	std::shared_ptr<Text> CreateText(const std::wstring aText, const int aWorldSize, std::shared_ptr<Font> aFont);

	float GetWidthOfText(Text* aText);

	float GetWidthOfText(const std::wstring& aText, std::shared_ptr<Font> aFont);

private:

	static TextFactory* myInstance;
	//std::vector<Text*> myTextObjects;
	std::unordered_map<std::wstring, Font> myFonts;

	void Init();

	friend class GraphicsEngine;
};