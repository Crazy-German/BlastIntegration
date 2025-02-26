#include "pch.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ScriptGraphEditorTypes.h"

#include <array>

#include "imgui_internal.h"
#include "../../../Utilities/CommonUtilities/Input.h"

extern "C" void __MuninGraph_AutoRegEditorTypes() {  }



IMPLEMENT_EDITOR_TYPE(float, Float)

bool ScriptGraphEditorType_Float::TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const
{
	const float y = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(y - 2);
	const ImVec2 inputSize = ImGui::CalcTextSize("0.0000");
	ImGui::SetNextItemWidth(inputSize.x);
	ImGui::InputFloat(aUniqueName.data(), static_cast<float*>(*aDataContainer), 0, 0, "%.1f");
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		return true;
	}

	return false;
}

std::string ScriptGraphEditorType_Float::ToString(const TypedDataContainer& aDataContainer) const
{
	float f = 0;
	aDataContainer.TryGet(f);
	return std::to_string(f);
};

IMPLEMENT_EDITOR_TYPE(std::string, String)

bool ScriptGraphEditorType_String::TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const
{
	ImGui::NewLine();
	ImGui::PushItemWidth(150);
	ImGui::InputText(aUniqueName.data(), static_cast<std::string*>(*aDataContainer), ImGuiInputTextFlags_EnterReturnsTrue);
	bool result = false;
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		result = true;
	}
	ImGui::PopItemWidth();
	return result;
}


std::string ScriptGraphEditorType_String::ToString(const TypedDataContainer& aDataContainer) const
{
	return *static_cast<const std::string*>(*aDataContainer);
}

IMPLEMENT_EDITOR_TYPE(CommonUtilities::Vector3<float>, Vec3)

bool ScriptGraphEditorType_Vec3::TypeEditWidget(std::string_view aUniqueName,
	const TypedDataContainer& aDataContainer) const
{
	const float y = ImGui::GetCursorPosY();
	CommonUtilities::Vector3<float>* data = static_cast<CommonUtilities::Vector3<float>*>(*aDataContainer);
	ImGui::SetCursorPosY(y - 2);
	const ImVec2 inputSize = ImGui::CalcTextSize("0.0000");
	ImGui::SetNextItemWidth(inputSize.x*3.f);
	std::string name = aUniqueName.data();
	ImGui::InputFloat3(aUniqueName.data(), data->GetArray(), "%.3f");
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		return true;
	}

	return false;
}

std::string ScriptGraphEditorType_Vec3::ToString(const TypedDataContainer& aDataContainer) const
{
	CommonUtilities::Vector3<float> vect;
	aDataContainer.TryGet(vect);
	return "X: " + std::to_string(vect.x) + ", Y: "+ std::to_string(vect.y)+ ", Z: "+ std::to_string(vect.z);
}

IMPLEMENT_EDITOR_TYPE(bool, Boolean)

bool ScriptGraphEditorType_Boolean::TypeEditWidget(std::string_view aUniqueName,
	const TypedDataContainer& aDataContainer) const
{
	bool* data = static_cast<bool*>(*aDataContainer);
	ImGui::Checkbox(aUniqueName.data(), data);
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		return true;
	}

	return false;
}

std::string ScriptGraphEditorType_Boolean::ToString(const TypedDataContainer& aDataContainer) const
{
	bool val;
	aDataContainer.TryGet(val);
	return val ? "True" : "False";
};

IMPLEMENT_EDITOR_TYPE(int, Integer)

bool ScriptGraphEditorType_Integer::TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const
{
	int* value = static_cast<int*>(*aDataContainer);
	ImGui::PushItemWidth(50.f);
	ImGui::InputScalar(aUniqueName.data(), ImGuiDataType_S32,value, nullptr, nullptr, "%d", ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopItemWidth();
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		return true;
	}

	return false;
}
std::string ScriptGraphEditorType_Integer::ToString(const TypedDataContainer& aDataContainer) const
{
	int value;
	aDataContainer.TryGet(value);
	return std::to_string(value);
}
IMPLEMENT_EDITOR_TYPE(unsigned, Unsignedint)

bool ScriptGraphEditorType_Unsignedint::TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const
{
	unsigned* value = static_cast<unsigned*>(*aDataContainer);
	ImGui::PushItemWidth(50.f);
	ImGui::InputScalar(aUniqueName.data(), ImGuiDataType_U32,value, nullptr, nullptr, "%d", ImGuiInputTextFlags_CharsDecimal);
	ImGui::PopItemWidth();
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		return true;
	}

	return false;
}
std::string ScriptGraphEditorType_Unsignedint::ToString(const TypedDataContainer& aDataContainer) const
{
	unsigned value;
	aDataContainer.TryGet(value);
	return std::to_string(value);
}


IMPLEMENT_EDITOR_TYPE(CommonUtilities::Keys, Key)

bool ScriptGraphEditorType_Key::TypeEditWidget(std::string_view aUniqueName,
                                               const TypedDataContainer& aDataContainer) const
{
	aUniqueName;
	int* value = static_cast<int*>(*aDataContainer);

    // Create a list of the keys (based on the keys in the map)
    std::vector<std::string> keyNameList;
    std::vector<int> keyCodeList;
	float xPos = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(ImGui::GetCursorPosX()-60);
	/*std::string input = keyNameList[selectedIndex];
	ImGui::InputText("Key", input.data(), 10);*/
	ImGui::PushItemWidth(50);
	ImGui::Text(CommonUtilities::Input::KeyCodeToString(static_cast<CommonUtilities::Keys>(*value)));
	ImGui::PopItemWidth();
	ImGui::SetCursorPosX(xPos);
	if(ImGui::IsItemDeactivatedAfterEdit())
	{
		return true;
	}

	return false;
}

std::string ScriptGraphEditorType_Key::ToString(const TypedDataContainer& aDataContainer) const
{
	int value;
	aDataContainer.TryGet(value);
	return ImGui::GetKeyName((ImGuiKey)value);
};

IMPLEMENT_EDITOR_TYPE(GameObject*, Actor)

bool ScriptGraphEditorType_Actor::TypeEditWidget(std::string_view aUniqueName,
                                               const TypedDataContainer& aDataContainer) const
{
	aUniqueName;
	aDataContainer;

	return false;
}

std::string ScriptGraphEditorType_Actor::ToString(const TypedDataContainer& aDataContainer) const
{
	GameObject* value;
	aDataContainer.TryGet(value);
	return value->GetName();
};

IMPLEMENT_EDITOR_TYPE(void*, Internal_VoidPtr);
