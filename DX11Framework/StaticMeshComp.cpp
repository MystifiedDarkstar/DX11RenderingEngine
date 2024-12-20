#include "StaticMeshComp.h"
#include "Scene.h"

StaticMeshComp::StaticMeshComp(GameObject* OwningGO, MeshData* InMesh, RenderingSystem* RenderSys) :
	Component(OwningGO, 120, COMPTYPE_StaticMesh),
	m_Rendering(RenderSys)
{
	SetMeshData(InMesh);
}

StaticMeshComp::~StaticMeshComp()
{

}

void StaticMeshComp::Update()
{

}

void StaticMeshComp::Render()
{
	ObjectDrawDESC Object;
	Object.Mesh = m_MeshData;
	Object.ColourMap = m_ColourTexture;
	Object.HasColourMap = m_HasColourMap;
	Object.SpecularMap = m_SpecularTexture;
	Object.HasSpecularMap = m_HasSpecularMap;
	Object.Transparency = m_TransparencyFactor;
	Object.ObjectPosition = GetParentObject()->GetPosition();
	Object.WorldPos = *m_ParentObject->GetWorld();
	m_ObjectDESC = std::make_unique<ObjectDrawDESC>(Object);

	// transparency call
	if (m_IsTransparencyActive)
		m_Rendering->AddTransparentObjectToDrawQueue(m_ObjectDESC.get());
	else
		m_Rendering->AddOpaqueObjectToDrawQueue(m_ObjectDESC.get());
}

void StaticMeshComp::SetColourTexture(TextureData* ColourMapIn)
{
	// Decrement Current Texture Users
	if (m_ColourTexture != nullptr)
		m_Rendering->GetTextureHandler()->DecrementTextureUser(m_ColourTexture->ID);

	// Set new Texture
	m_ColourTexture = ColourMapIn;
	m_HasColourMap = 1;
}

void StaticMeshComp::SetSpecularTexture(TextureData* SpecularMapIn)
{
	// Decrement Current Texture Users
	if (m_SpecularTexture != nullptr)
		m_Rendering->GetTextureHandler()->DecrementTextureUser(m_SpecularTexture->ID);

	// Set new Texture	
	m_SpecularTexture = SpecularMapIn;
	m_HasSpecularMap = 1;
}

void StaticMeshComp::SetTransparencyActive(XMVECTOR TransparencyVal)
{
	m_IsTransparencyActive = true; 
	m_TransparencyFactor = TransparencyVal;
}

void StaticMeshComp::DisableTransparency()
{
	m_IsTransparencyActive = false;
}

void StaticMeshComp::SetMeshData(MeshData* MeshIn)
{
	// Decrement the amount of mesh users, when this reaches 0 the mesh will be unloaded.
	if (m_MeshData != nullptr)
		m_Rendering->GetMeshHandler()->DecrementMeshUser(m_MeshData->ID);

	// Set the new mesh.
	m_MeshData = MeshIn;
}
