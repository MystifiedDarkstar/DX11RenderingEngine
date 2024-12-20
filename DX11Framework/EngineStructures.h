#pragma once

#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;

struct SimpleVertex
{
	XMFLOAT3 Position;
	XMFLOAT3 Normal;
	XMFLOAT2 Texcoord;

	bool operator< (const SimpleVertex other) const
	{
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0;
	};
}; 

struct TextureData
{
	std::string ID = "";
	int Count = 0;
	ID3D11ShaderResourceView* Texture = nullptr;

	TextureData() = default;

	TextureData(std::string pID, ID3D11ShaderResourceView* pTexture)
	{
		Count = 0;
		ID = pID;
		Texture = pTexture;
	}

	~TextureData()
	{
		Texture->Release();
	}
};

struct MeshData
{
	std::string ID; // the mesh path, used for accessing the mesh data to decrement/increment users.
	int Count; // keep track of how many times this is being used, key to unloading
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

struct DirectionalLight
{
	XMFLOAT4 AmbientLight;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 SpecularLight;
	XMFLOAT3 LightDir;
	float SpecularPower;
};

struct PointLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT3 Attenuation; // Point Lights
	float LightRange; // Used for Point Lights
	XMFLOAT3 Position;
	float Padding;
};

struct ConstantBuffer
{
	XMMATRIX Projection;
	XMMATRIX View;
	XMMATRIX World;
	DirectionalLight DLights[4];
	PointLight PLights[4];
	XMFLOAT3 CameraPosition;// 1
	int DLightCount;
	int PLightCount;
};

struct ScreenInformation
{
	ScreenInformation() = default;
	ScreenInformation(float WndWidth, float WndHeight)
	{
		WindowHeight = WndHeight;
		WindowWidth = WndWidth;
	}
	float WindowWidth;
	float WindowHeight;
};

struct ObjectDrawDESC
{
	MeshData* Mesh = nullptr;
	TextureData* ColourMap = nullptr;
	UINT HasColourMap;
	TextureData* SpecularMap = nullptr;
	UINT HasSpecularMap;
	XMFLOAT3 ObjectPosition;
	XMFLOAT4X4 WorldPos;
	XMVECTOR Transparency;


};

struct SpriteDrawDesc
{
	RECT DrawPosition; // where to draw the sprite
	TextureData* Sprite; // the sprite to draw (just a texture)
};

struct FontDrawDesc
{
	XMFLOAT2 DrawPosition;
	XMFLOAT2 DrawScale;
	FLOAT DrawRotation;
	XMVECTOR DrawColour;
	std::wstring DrawString;
};