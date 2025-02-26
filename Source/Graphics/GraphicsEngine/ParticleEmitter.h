#pragma once
#include "ParticleVertex.h"
#include "ParticleEmitterSettings.h"
#include <random>

struct ID3D11Buffer;

class ParticleEmitter
{
	friend class GraphicsEngine;
	friend class GraphicsCommandDrawParticles;

public:
	ParticleEmitter(/*PipelineStateObject& aPSO*/);
	virtual ~ParticleEmitter();

	void Init(ParticleEmitterSettings* aSettings, CU::Vector3f& aPosition, float aLifeTime = -1.0f);
	void Init(ParticleEmitterSettings* aSettings, CU::Matrix4x4f& aTransform, float aLifeTime = -1.0f);
	void SetParent(CU::Matrix4x4f* aParentTransform);
	void SetPosition(CU::Vector3f aPosition);
	void SetRotation(CU::Vector3f aRotation);
	CU::Vector3f GetPosition();
	CU::Vector3f GetRotation();
	void Update(float aDeltaTime);
	void Burst();
	ParticleEmitterSettings* GetSettings();
	CU::Matrix4x4f* GetTransform();

	const bool GetIsDead() const;

	FORCEINLINE Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return myVertexBuffer; }
	const size_t GetParticleAmount() const;

protected:

	virtual void UpdateTransform();

	virtual void InitParticle(ParticleVertex& aVertex, size_t aIndex);
	virtual void UpdateParticle(ParticleVertex& aParticle, size_t aIndex, float aDeltaTime);
	virtual void InitInternal();
	std::vector<ParticleVertex> myParticles;

private:
	static std::mt19937 ourRandomEngine;
	static std::uniform_real_distribution<float> ourRandomDistribution;

	//PipelineStateObject& myPSO;
	Microsoft::WRL::ComPtr<ID3D11Buffer> myVertexBuffer;
	ParticleEmitterSettings* mySettings;
	CU::Vector4f myPosition;
	CU::Vector3f myRotation;
	float myLifeTime;
	bool myLocalTransformChanged = false;
	bool myAmDying = false;
	bool myAmDead = false;
	CU::Matrix4x4f myLocalTransform;
	CU::Matrix4x4f myTransform;
	CU::Matrix4x4f* myParent;
};

