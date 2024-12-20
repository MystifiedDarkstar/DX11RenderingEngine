#pragma once

#include "Scene.h"
#include "Renderer.h"

// Include all Specfic Scenes here
#include "Scene_Main.h"

enum Screens
{
	SCREEN_Main
};

class SceneManager
{
public:
	SceneManager() = default;
	SceneManager(RenderingSystem* RenderSys);
	~SceneManager();

	void Render();
	void RenderSkybox();
	void Update(float DeltaTime);

	void ChangeScreen(Screens NewScreen);
	void SetSelfRef(SceneManager* Self) { m_Self = Self; }

private:
	RenderingSystem* m_Rendering;

	std::unique_ptr<Scene> m_CurrentScene;
	Screens m_CurrentSceneState;

	SceneManager* m_Self;
};

