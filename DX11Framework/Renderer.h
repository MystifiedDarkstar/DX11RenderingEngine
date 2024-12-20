#pragma once

#include <vector>
#include <SpriteBatch.h> // DirectX Spritebatch
#include <SpriteFont.h> // DirectX Spritefont
#include "EngineStructures.h"
#include "MeshHandler.h"
#include "TextureHandler.h"

class RenderingSystem
{
public:
	RenderingSystem(HWND WindowHandle, ScreenInformation ScreenInfo);
	~RenderingSystem();

	HRESULT Initialise();
	void Release();

	void Update(float DeltaTime);
	void Render();

	void AddSkyboxObjectToDrawQueue(ObjectDrawDESC* Object);
	void AddOpaqueObjectToDrawQueue(ObjectDrawDESC* Object);
	void AddTransparentObjectToDrawQueue(ObjectDrawDESC* Object);
	void AddSpriteToDrawQueue(SpriteDrawDesc* Sprite);
	void AddTextToDrawQueue(FontDrawDesc* Text);

	void SetCameraPosition(XMFLOAT3 camPos);

	ID3D11Device* GetD3DDevice() { return m_Device; }
	ID3D11DeviceContext* GetD3DDevContext() { return m_DevContext; }

	ID3D11Buffer* GetConstantBuffer() { return m_ConstantBuffer; }
	ConstantBuffer GetCBData() { return m_CbData; }
	void SetCBData(ConstantBuffer CbData) {  m_CbData = CbData; }

	MeshHandler* GetMeshHandler() { return m_MeshHandler; }
	TextureHandler* GetTextureHandler() { return m_TextureHandler; }

	ScreenInformation GetScreenInfo() { return m_ScreenInfo; }

private:
	HRESULT CreateD3DDevice();
	HRESULT CreateSwapChain();
	HRESULT InitFrameBuffer();
	HRESULT InitShadersAndInputLayout();
	HRESULT InitPipelineStates();
	HRESULT InitHandlers();

	void SortObjects(std::vector<ObjectDrawDESC*>& array, int start, int end);
	int Partition(std::vector<ObjectDrawDESC*>& array, int start, int end);
	float Distance(const XMFLOAT3& v1, const XMFLOAT3& v2);

	void RenderAllOpaqueObjects();
	void RenderAllTransparentObjects();
	void RenderAllSkybox();
	void RenderAllSprites();
	void RenderAllText();


private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DevContext;
	IDXGISwapChain1* m_SwapChain;
	HWND m_WindowHandle;
	D3D11_VIEWPORT m_Viewport;
	IDXGIDevice* m_DXGIDevice = nullptr;
	IDXGIFactory2* m_DXGIFactory = nullptr;

	std::unique_ptr<SpriteBatch> m_SpriteBatch;
	std::unique_ptr<SpriteFont> m_SpriteFont;

	ScreenInformation m_ScreenInfo;

	MeshHandler* m_MeshHandler;
	TextureHandler* m_TextureHandler;

	ID3D11RasterizerState* m_FillRSState; 
	ID3D11RasterizerState* m_WireFrameRSState; 
	ID3D11RasterizerState* m_SkyboxRSState; 
	bool m_IsFillActive = true;
	bool m_IsWireframeActive = false;

	ID3D11VertexShader* m_VertexShader; 
	ID3D11PixelShader* m_PixelShader; 
	ID3D11InputLayout* m_InputLayout; 

	ID3D11VertexShader* m_SkyboxVertexShader; 
	ID3D11PixelShader* m_SkyboxPixelShader; 
	ID3D11DepthStencilState* m_SkyBoxDepthStencilState; 

	ID3D11RenderTargetView* m_FrameBufferView = nullptr;

	ID3D11Buffer* m_ConstantBuffer; 
	ConstantBuffer m_CbData;

	ID3D11Texture2D* m_DepthStencilBuffer; 
	ID3D11DepthStencilView* m_DepthStencilView; 

	ID3D11SamplerState* m_BilinearSamplerState; 

	ID3D11BlendState* m_TransparencyBS;

	// Rendering Queue Variables
	ObjectDrawDESC* m_SkyboxObject;
	std::vector<ObjectDrawDESC*> m_OpaqueObjects;
	std::vector<ObjectDrawDESC*> m_TransparentObjects;
	std::vector<SpriteDrawDesc*> m_2DSprites;
	std::vector<FontDrawDesc*> m_2DTexts;

	XMFLOAT3 m_CameraPos;
};