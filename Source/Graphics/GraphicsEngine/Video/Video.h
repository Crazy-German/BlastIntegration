#pragma once

class Texture;

class VideoTexture {
public:
	static bool CreateAPI();
	static void DestroyAPI();


	bool Create(const char* aFilename);
	void Destroy();
	bool Update(float aDeltaTime);

	void Pause();
	void Resume();
	bool HasFinished();
	std::shared_ptr<Texture> GetTexture();
	float GetAspectRatio() const;

private:

	struct InternalData;
	InternalData* myInternalData = nullptr;
};
