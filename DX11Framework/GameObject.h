#pragma once

#include "ComponentTypes.h"
#include "EngineStructures.h"
#include "GameObjectTags.h"
#include "Transform.h"
#include "Renderer.h"

#include <iostream>
#include <vector>

using namespace DirectX;

class GameObject
{
public:
	GameObject(class Scene* OwningScene, GOTags GOTAG, RenderingSystem* RenderSys);
	~GameObject();

	void Update(); // Update called from Scene. Not overridable.
	void Render();

	UINT GetID() { return m_ID; }
	void SetID(UINT ID) { m_ID = ID; }

	Scene* GetParentScene();

	void AddComponent(std::unique_ptr<class Component> component);
	Component* GetComponent(ComponentTypes Type);
	void RemoveComponent(ComponentTypes Type);

	DirectX::XMFLOAT4X4* GetWorld() { return &m_WorldMatrix; }
	XMFLOAT3 GetPosition() { return m_Transform->Position; }
	XMFLOAT3 GetRotation() { return m_Transform->Rotation; }
	XMFLOAT3 GetScale() { return m_Transform->Scale; }
	void SetPosition(DirectX::XMFLOAT3 NewPosition);
	void SetRotation(DirectX::XMFLOAT3 NewRotation);
	void SetScale(DirectX::XMFLOAT3 NewScale);

	void SetTag(GOTags Tag);
	bool CompareTag(GOTags Tag);

private:
	void UpdateComponents(); // Updates all components. 

private:
	UINT m_ID;
	GOTags m_Tag;

	Scene* m_OwningScene;

	RenderingSystem* m_Rendering;

	std::shared_ptr<Transform> m_Transform;
	std::vector<std::unique_ptr<class Component>> m_Components;

	DirectX::XMFLOAT4X4 m_WorldMatrix = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

protected:
	static std::atomic<UINT> s_ID;
};

