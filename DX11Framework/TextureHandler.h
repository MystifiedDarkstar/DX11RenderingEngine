#pragma once

#include "EngineStructures.h"
#include "DDSTextureLoader.h"
#include <iostream>
#include <map>

class TextureHandler
{
public:
	TextureHandler() = default;
	TextureHandler(ID3D11Device* Device);
	~TextureHandler();

	TextureData* TextureLookUp(std::string TexturePath);
	void DecrementTextureUser(std::string TexturePath);

private:
	void LoadTextureFromPath(std::string TexturePath);
	void UnloadTextureFromPath(std::string TexturePath);

private:
	std::map<std::string, std::unique_ptr<TextureData>> m_Textures;
	ID3D11Device* m_Device;

};

