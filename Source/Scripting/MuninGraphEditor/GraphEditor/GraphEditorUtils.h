#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
#include "NodeGraph/NodeGraphCommon.h"

enum class GraphIcon : unsigned
{
	Exec,
	Circle,
	Square,
	Grid,
	RoundedSquare,
	Diamond
};

namespace GraphEditorUtils
{
	void DrawIcon(ImDrawList* aDrawList, const ImRect& aRect, GraphIcon aGraphIcon, bool aIsFilled, GraphColor aColor, GraphColor aFillColor);
};
