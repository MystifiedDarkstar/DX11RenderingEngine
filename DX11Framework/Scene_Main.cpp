#include "Scene_Main.h"
#include "OBJLoader.h"

Scene_Main::Scene_Main(SceneManager* SceneMan, RenderingSystem* RenderSys, std::string JsonDataPath) :
	Scene(SceneMan, RenderSys, JsonDataPath)
{
	SetUpScene();
}

void Scene_Main::EndScene()
{

}

void Scene_Main::UpdateScene(float DeltaTime)
{
	for (int i = 3; i < 7; i++)
	{
		m_Crate = GetGameObject(i);
		m_RotCount += 0.25 * DeltaTime;
		m_Crate->SetRotation(XMFLOAT3(m_RotCount / 2, m_RotCount / 2, 0));
	}
}

void Scene_Main::SetUpScene()
{

}
