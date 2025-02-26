#pragma once

#include "GraphicsCommandList.h"
#include "DebugColor.h"
#include "Vertex.h"

class DebugLineObject;
class Mesh;

class GfxCmd_DrawDebugLines : public GraphicsCommandBase
{
public:
	GfxCmd_DrawDebugLines(std::shared_ptr<DebugLineObject> someDebugLines, const CU::Matrix4x4f& aTransform);
	GfxCmd_DrawDebugLines(std::shared_ptr<Mesh> aMesh, const CU::Matrix4x4f& aTransform, bool aHasBones = false, DebugColor aColor = DebugColor::Red, std::string_view aDebugName = "");
	~GfxCmd_DrawDebugLines() override;


	void Execute() override;
	void Destroy() override;
private:
	std::shared_ptr<DebugLineObject> myDebugObject;
	CU::Matrix4x4f myTransform;
	std::string myDebugName;
	bool myHasBones = false;
};
