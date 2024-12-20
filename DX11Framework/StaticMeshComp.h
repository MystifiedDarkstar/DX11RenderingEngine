#pragma once

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "MeshHandler.h"
#include "TextureHandler.h"

#include "Component.h"
#include "EngineStructures.h"

class StaticMeshComp : public Component
{
public:
	StaticMeshComp(GameObject* OwningGO, MeshData* InMesh, RenderingSystem* RenderSys);
	~StaticMeshComp();

	void Update() override;
	void Render() override;

	void SetColourTexture(TextureData* ColourMapIn);
	TextureData* GetColourMap() { return m_ColourTexture; }

	void SetSpecularTexture(TextureData* SpecularMapIn);
	TextureData* GetSpecularMap() { return m_SpecularTexture; }

	void SetTransparencyActive(XMVECTOR TransparencyVal);
	XMVECTOR GetTransparency() { return m_TransparencyFactor; }

	void DisableTransparency();
	
	void SetMeshData(MeshData* MeshIn);
	MeshData* GetMeshData() { return m_MeshData; }

private:
	TextureData* m_ColourTexture = nullptr;
	TextureData* m_NormalTexture = nullptr;
	TextureData* m_SpecularTexture = nullptr;
	UINT m_HasColourMap = 0;
	UINT m_HasNormalMap = 0;
	UINT m_HasSpecularMap = 0;

	MeshData* m_MeshData = nullptr;

	XMVECTOR m_TransparencyFactor;
	bool m_IsTransparencyActive = false;

	RenderingSystem* m_Rendering;

	std::unique_ptr<ObjectDrawDESC> m_ObjectDESC;
};

