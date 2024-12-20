#include "TextRenderer.h"

TextRenderer::TextRenderer(GameObject* OwningGO, RenderingSystem* RenderSys, std::wstring Text, XMVECTOR Colour, XMFLOAT2 Scale) : 
	Component(OwningGO, 120, COMPTYPE_GUIText),
	m_Rendering(RenderSys),
	m_Text(Text),
	m_TextColour(Colour),
	m_TextScale(Scale)
{
	m_FontDrawDesc = std::make_unique<FontDrawDesc>();
	m_FontDrawDesc->DrawColour = m_TextColour;
	m_FontDrawDesc->DrawString = m_Text;
	m_FontDrawDesc->DrawScale = m_TextScale;
}

TextRenderer::~TextRenderer()
{

}

void TextRenderer::Update()
{
	m_FontDrawDesc->DrawPosition = XMFLOAT2(GetParentObject()->GetPosition().x, GetParentObject()->GetPosition().y);
	m_FontDrawDesc->DrawRotation = GetParentObject()->GetRotation().z;
}

void TextRenderer::Render()
{
	m_FontDrawDesc->DrawString = m_Text;
	m_Rendering->AddTextToDrawQueue(m_FontDrawDesc.get());
}

void TextRenderer::SetText(std::wstring Text)
{
	m_Text = Text;
	m_FontDrawDesc->DrawString = m_Text;
}

void TextRenderer::SetTextColour(XMVECTOR Colour)
{
	m_TextColour = Colour;
	m_FontDrawDesc->DrawColour = m_TextColour;
}

void TextRenderer::SetTextScale(XMFLOAT2 Scale)
{
	m_TextScale = Scale;
	m_FontDrawDesc->DrawScale = m_TextScale;
}
