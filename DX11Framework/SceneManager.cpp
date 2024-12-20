#include "SceneManager.h"

SceneManager::SceneManager(RenderingSystem* RenderSys) :
	m_CurrentScene(nullptr),
	m_Rendering(RenderSys)
{

}

SceneManager::~SceneManager()
{
	m_Rendering = nullptr;
	m_Self = nullptr;

}

void SceneManager::Render()
{
	m_CurrentScene->Render(); 
}

void SceneManager::RenderSkybox()
{
	m_CurrentScene->RenderSkyBox();
}

void SceneManager::Update(float DeltaTime)
{
	m_CurrentScene->Update(DeltaTime);
}

void SceneManager::ChangeScreen(Screens NewScreen)
{
	// Check which scene we want to change to.
	m_CurrentScene.reset();
	switch (NewScreen)
	{
	case SCREEN_Main:
	{
		m_CurrentScene = std::make_unique<Scene_Main>(m_Self, m_Rendering, "ConfigFiles/Scene_Main.json");
		m_CurrentSceneState = NewScreen;
		break;
	}
	default:
		return;
		break;
	}

}
