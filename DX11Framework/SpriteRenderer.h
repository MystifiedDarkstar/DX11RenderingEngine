#pragma once

#include "Component.h"

class SpriteRenderer :  public Component
{
public:
	SpriteRenderer() = default;
	SpriteRenderer(GameObject* OwningGO, TextureData* SpriteData, XMFLOAT2 Size, RenderingSystem* RenderSys);
	~SpriteRenderer();

	void Render() override;
	void Update() override;

	void SetTextureData(TextureData* SpriteData);
	void SetSpriteSize(XMFLOAT2 Size);

private:
	RenderingSystem* m_Rendering;
	std::unique_ptr<SpriteDrawDesc> m_SpriteDrawDesc;
	TextureData* m_SpriteData;
	XMFLOAT2 m_SpriteSize;
	RECT m_DrawRect;
};

