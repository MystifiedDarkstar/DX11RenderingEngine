#pragma once

#include "Component.h"

class TextRenderer : public Component
{
public:
	TextRenderer() = default;
	TextRenderer(GameObject* OwningGO, RenderingSystem* RenderSys, std::wstring Text, XMVECTOR Colour, XMFLOAT2 Scale);
	~TextRenderer();

	void Update() override;
	void Render() override;

	void SetText(std::wstring Text);
	void SetTextColour(XMVECTOR Colour);
	void SetTextScale(XMFLOAT2 Scale);

private:
	RenderingSystem* m_Rendering;
	std::wstring m_Text;
	XMVECTOR m_TextColour;
	XMFLOAT2 m_TextScale;
	std::unique_ptr<FontDrawDesc> m_FontDrawDesc;

};

