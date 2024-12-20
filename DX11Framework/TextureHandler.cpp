#include "TextureHandler.h"

TextureHandler::TextureHandler(ID3D11Device* Device) :
    m_Device(Device)
{
}

TextureHandler::~TextureHandler()
{
    m_Device = nullptr;

    m_Textures.clear();
}

TextureData* TextureHandler::TextureLookUp(std::string TexturePath)
{
	if (m_Textures.count(TexturePath) == 0) // No Texture exists for this key, attempt to load it
	{
		// Load Texture @ Path Specified.
		LoadTextureFromPath(TexturePath);

		// Return the new Texture
		m_Textures[TexturePath]->Count++;
		return m_Textures[TexturePath].get();
	}

	else if (m_Textures.count(TexturePath) > 0) // Texture Must Exist at this key, Lets return it.
	{
		m_Textures[TexturePath]->Count++; // Increment the current users for this texture.
		return m_Textures[TexturePath].get();
	}
}

void TextureHandler::DecrementTextureUser(std::string TexturePath)
{
	if (m_Textures.count(TexturePath) == 0) return; // No Texture exists at this path

	m_Textures[TexturePath]->Count--; // Dwecrement the current users on this meshpath.

	if (m_Textures[TexturePath]->Count == 0) // Check if the number of current users is equal to 0.
		UnloadTextureFromPath(TexturePath); // Texture users = 0, Unload the texture from memory.
}

void TextureHandler::LoadTextureFromPath(std::string TexturePath)
{
	std::wstring WTexPath(TexturePath.begin(), TexturePath.end()); // Convert texture path to a wide string.
	ID3D11ShaderResourceView* TempTexture; // Create a temporay texture.
	HRESULT hr = CreateDDSTextureFromFile(m_Device, WTexPath.c_str(), nullptr, &TempTexture); // Create a texture using wide string texture path.
	if (FAILED(hr)) return; // check it created succesfully.

	std::unique_ptr<TextureData> TempTextureData = std::make_unique<TextureData>(TexturePath, TempTexture); // Create new texture data with all data created earlier.
	m_Textures[TexturePath] = std::move(TempTextureData); // Move the Unique owner to the map, this now owns the texture data.
}

void TextureHandler::UnloadTextureFromPath(std::string TexturePath)
{
	std::map<std::string, std::unique_ptr<TextureData>>::iterator it; // Create a new iterator for the texture map
	it = m_Textures.find(TexturePath); // find the iterator for the provided texture key.
	m_Textures.find(TexturePath)->second->Texture->Release(); // Release the texture from the data.
	m_Textures.erase(it); // erase this texture data from the map, this deletes it from memory.
}
