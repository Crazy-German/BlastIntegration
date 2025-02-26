#pragma once
#include "GraphicsCommandList.h"

struct AnimationBuffer;


class GfxCmd_SetAnimationBuffer : public GraphicsCommandBase
{
public:
	GfxCmd_SetAnimationBuffer(std::shared_ptr<AnimationBuffer> aAnimationBuffer);
	~GfxCmd_SetAnimationBuffer() override;

	void Execute() override;
	void Destroy() override;

private:
	std::shared_ptr<AnimationBuffer> myAnimationBuffer;
};