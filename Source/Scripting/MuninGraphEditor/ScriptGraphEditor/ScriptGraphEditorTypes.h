#pragma once
#include "imgui.h"
#include "ScriptGraphEditorTypeRegistry.h"
#include "../../../Application/FeatherEngine/GameObjectSystem/GameObject.h"
#include "../../Utilities/CommonUtilities/Vector3.hpp"
#include "../../Utilities/CommonUtilities/Input.h"

#pragma comment(linker, "/include:__MuninGraph_AutoRegEditorTypes")

BEGIN_DECLARE_EDITOR_TYPE(float, Float, GraphColor(156, 246, 60, 255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(std::string, String, GraphColor(250, 0, 208, 255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(Commonutilities::Vector3<float>, Vec3, GraphColor(255, 204, 0, 255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();


BEGIN_DECLARE_EDITOR_TYPE(bool, Boolean, GraphColor(115, 0, 153, 255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(int, Integer, GraphColor(0, 210, 80, 255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(unsigned, Unsignedint, GraphColor(0, 80, 210, 255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(Commonutilities::Keys, Key, GraphColor(0,0,200,255), true);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(void*, Internal_VoidPtr, GraphColor(0, 168, 241, 255), false);
	FORCEINLINE bool IsInternal() const override { return true; }
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(Gameobject*, Actor, GraphColor(173, 216, 230, 255), false);
	bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const override;
	std::string ToString(const TypedDataContainer& aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();