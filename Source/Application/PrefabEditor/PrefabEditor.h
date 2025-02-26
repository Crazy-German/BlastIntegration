#pragma once
#include "GameObjectSystem/GameObject.h"

class SIZE;
class WNDPROC;
class LPCWSTR;

class PrefabEditor
{
public:
	PrefabEditor();
	~PrefabEditor();

	bool Initialize(SIZE aWindowSize, WNDPROC aWindowProcess, LPCWSTR aWindowTitle);
	int Run();
	static void RegisterAssets();
	void CreateScene();
	void UpdateScene(float aTimeDelta);
	void RenderScene();

private:
	GameObject myGameObject;
	
};
