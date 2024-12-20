#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(GameObject* OwningGO, TextureData* SpriteData, XMFLOAT2 Size, RenderingSystem* RenderSys) :
	Component(OwningGO, 120, COMPTYPE_SpriteRenderer),
	m_SpriteData(SpriteData),
	m_SpriteSize(Size),
	m_Rendering(RenderSys)
{
	m_SpriteDrawDesc = std::make_unique<SpriteDrawDesc>();
	m_DrawRect.left = GetParentObject()->GetPosition().x;
	m_DrawRect.top = GetParentObject()->GetPosition().y;
	m_DrawRect.bottom = GetParentObject()->GetPosition().y + m_SpriteSize.y;
	m_DrawRect.right = GetParentObject()->GetPosition().x + m_SpriteSize.x;
	m_SpriteDrawDesc->Sprite = m_SpriteData;
	m_SpriteDrawDesc->DrawPosition = m_DrawRect;
}

SpriteRenderer::~SpriteRenderer()
{
	m_SpriteData = nullptr;
	m_Rendering = nullptr;
}

void SpriteRenderer::Render()
{	
	m_Rendering->AddSpriteToDrawQueue(m_SpriteDrawDesc.get());
}

void SpriteRenderer::Update()
{
	m_DrawRect.left = GetParentObject()->GetPosition().x;
	m_DrawRect.top = GetParentObject()->GetPosition().y;
	m_DrawRect.bottom = GetParentObject()->GetPosition().y + m_SpriteSize.y;
	m_DrawRect.right = GetParentObject()->GetPosition().x + m_SpriteSize.x;
	m_SpriteDrawDesc->Sprite = m_SpriteData;
	m_SpriteDrawDesc->DrawPosition = m_DrawRect;
}

void SpriteRenderer::SetTextureData(TextureData* SpriteData)
{
	m_SpriteData = SpriteData;
}

void SpriteRenderer::SetSpriteSize(XMFLOAT2 Size)
{
	m_SpriteSize = Size;
}
