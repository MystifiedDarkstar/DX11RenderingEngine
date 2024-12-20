#include "GameObject.h"
#include "Component.h"
//#include "Scene.h"

std::atomic<UINT> GameObject::s_ID;

GameObject::GameObject(class Scene* OwningScene, GOTags GOTAG, RenderingSystem* RenderSys) :
	m_OwningScene(OwningScene),
	m_ID(s_ID++),
	m_Tag(GOTAG),
	m_Rendering(RenderSys)
{
	m_Transform = std::make_shared<Transform>();
}

GameObject::~GameObject()
{
	m_Components.clear();
	m_Rendering = nullptr;	
	m_OwningScene = nullptr;
}

void GameObject::Update()
{
	// Create a world matrix for this object, this is used when rendering.
	XMStoreFloat4x4(&m_WorldMatrix, 
					XMMatrixScaling(m_Transform->Scale.x, m_Transform->Scale.y, m_Transform->Scale.z) *
					XMMatrixRotationX(m_Transform->Rotation.x) * XMMatrixRotationY(m_Transform->Rotation.y) * XMMatrixRotationZ(m_Transform->Rotation.z) *
					XMMatrixTranslation(m_Transform->Position.x, m_Transform->Position.y, m_Transform->Position.z));
	
	// Update all of the components on this object.
	UpdateComponents();
}

void GameObject::Render()
{
	// iterate through all the components on this object and call their render function.
	for (auto& comp : m_Components)
	{
		comp->Render();
	}
}

Scene* GameObject::GetParentScene()
{
	return m_OwningScene;
}


void GameObject::AddComponent(std::unique_ptr<Component> component)
{
	// Add new component to vector
	m_Components.push_back(std::move(component));

	// Sort components in order of UpdateOrder. Lowest order to 0 first.
	// This is because some components like a Camera/Collision might want to update after movement has taken place.
	Components::SortComponents(m_Components, 0, (int)m_Components.size() - 1);
}

Component* GameObject::GetComponent(ComponentTypes Type)
{
	// iterate through all components
	for (int i = 0; i < m_Components.size(); i++)
	{
		// Check if their type is equal to what we are trying to find. This will just return the first one if their is 2 of the same component.
		if (m_Components[i]->GetCompType() == Type)
		{
			// Return the component.
			return m_Components[i].get();
		}
	}
}

void GameObject::RemoveComponent(ComponentTypes Type)
{
	// Iterate through all components.
	for (int i = 0; i < m_Components.size(); i++)
	{
		// Check if the component we are looking at matches the type we are looking for.
		if (m_Components[i]->GetCompType() == Type)
		{
			// remove the component
			m_Components.erase(m_Components.begin() + i, m_Components.begin() + i + 1);
		}
	}
}

void GameObject::SetPosition(DirectX::XMFLOAT3 NewPosition)
{
	m_Transform->Position = NewPosition;
}

void GameObject::SetRotation(DirectX::XMFLOAT3 NewRotation)
{
	m_Transform->Rotation = NewRotation;
}

void GameObject::SetScale(DirectX::XMFLOAT3 NewScale)
{
	m_Transform->Scale = NewScale;
}

void GameObject::SetTag(GOTags Tag)
{
	m_Tag = Tag;
}

bool GameObject::CompareTag(GOTags Tag)
{
	if (m_Tag == Tag)
		return true;
	else
		return false;
}

void GameObject::UpdateComponents()
{
	// iterate through all components and update them 1 by 1.
	for (auto& comp : m_Components)
	{
		comp->Update();
	}
}