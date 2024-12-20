#pragma once

#include "Scene.h"

class Scene_Main : public Scene
{
public:
	Scene_Main(class SceneManager* SceneMan, RenderingSystem* RenderSys, std::string JsonDataPath);
	void EndScene() override; // this acts as a destructor, its called from base_scene

	void UpdateScene(float DeltaTime) override;

private:
	void SetUpScene() override;

private:
	GameObject* m_Crate;
	float m_RotCount;
};

