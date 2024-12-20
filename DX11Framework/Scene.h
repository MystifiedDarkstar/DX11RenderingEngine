#pragma once

// Include all component Types here.
#include "GameObject.h"
#include "StaticCamera.h"
#include "DebugCamera.h"
#include "Renderer.h"
#include "StaticMeshComp.h"
#include "TextRenderer.h"
#include "SpriteRenderer.h"

#include "EngineStructures.h"

// Misc Includes
#include <map>
#include "JSON\json.hpp"

using json = nlohmann::json;


class Scene
{
public:
	Scene() = default;
	Scene(class SceneManager* SceneMan, RenderingSystem* RenderSys, std::string JsonDataPath);
	~Scene();

	void Render();
	void RenderSkyBox();
	void Update(float DeltaTime);

	GameObject* GetGameObject(UINT ID);
	void RemoveGameObject(UINT GameObjectID);

private:
	virtual void SetUpScene() {}
	virtual void EndScene() {}
	virtual void UpdateScene(float DeltaTime) {}

	void ReadCameraData(std::string SceneInfoPath);
	void ReadLightingData(std::string SceneInfoPath);
	void ReadSceneData(std::string SceneInfoPath);

protected:
	GameObject* CreateGameObject();
	GameObject* CreateGameObject(UINT ID);
	StaticMeshComp* CreateStaticMeshComponent(GameObject* OwningGO, MeshData* InMesh);
	SpriteRenderer* CreateSpriteRendererComponent(GameObject* OwningGO, TextureData* Sprite, XMFLOAT2 Size);
	TextRenderer* CreateTextRendererComponent(GameObject* OwningGO, std::wstring Text, XMVECTOR Colour);

protected:
	SceneManager* m_SceneManager;
	RenderingSystem* m_Rendering;

private:
	std::map<UINT, std::unique_ptr<GameObject>> m_GameObjects;
	GameObject* m_StaffsUniLogo = nullptr;
	GameObject* m_ActiveCameraTextGO = nullptr;
	TextRenderer* m_ActiveCameraText = nullptr;

	int m_activeCameraID = 0; // this is just a position in the vector of cameras
	std::vector<std::unique_ptr<BaseCamera>> m_Cameras;
};

