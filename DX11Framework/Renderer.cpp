#include "Renderer.h"

void RenderingSystem::AddOpaqueObjectToDrawQueue(ObjectDrawDESC* Object)
{
	m_OpaqueObjects.push_back(Object);
}

void RenderingSystem::AddSkyboxObjectToDrawQueue(ObjectDrawDESC* Object)
{
	m_SkyboxObject = Object;
}

void RenderingSystem::AddTransparentObjectToDrawQueue(ObjectDrawDESC* Object)
{
	m_TransparentObjects.push_back(Object);
}

void RenderingSystem::AddSpriteToDrawQueue(SpriteDrawDesc* Sprite)
{
	m_2DSprites.push_back(Sprite);
}

void RenderingSystem::AddTextToDrawQueue(FontDrawDesc* Text)
{
	m_2DTexts.push_back(Text);
}

void RenderingSystem::RenderAllOpaqueObjects()
{
	SortObjects(m_OpaqueObjects, 0, m_OpaqueObjects.size() - 1);
	// Do Draw Stuff here
	for (int i = 0; i < m_OpaqueObjects.size(); i++)
	{
		//Store this Objects data in constant buffer struct
		m_CbData.World = XMMatrixTranspose(XMLoadFloat4x4(&m_OpaqueObjects[i]->WorldPos));

		//Write constant buffer data onto GPU
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		m_DevContext->Map(m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		memcpy(mappedSubresource.pData, &m_CbData, sizeof(m_CbData));
		m_DevContext->Unmap(m_ConstantBuffer, 0);

		m_DevContext->OMSetBlendState(0, 0, 0xffffffff); // regular draw call

		// Set Vertex and Index Buffers
		m_DevContext->IASetVertexBuffers(0, 1, &m_OpaqueObjects[i]->Mesh->VertexBuffer,
											   &m_OpaqueObjects[i]->Mesh->VBStride,
											   &m_OpaqueObjects[i]->Mesh->VBOffset);

		m_DevContext->IASetIndexBuffer(m_OpaqueObjects[i]->Mesh->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// Set Shader Resources (Textures) on the GPU
		m_DevContext->PSSetShaderResources(0, 1, &m_OpaqueObjects[i]->ColourMap->Texture);

		if (m_OpaqueObjects[i]->SpecularMap) // Check if specular texture exists
			m_DevContext->PSSetShaderResources(1, 1, &m_OpaqueObjects[i]->SpecularMap->Texture);

		// Draw the Mesh
		m_DevContext->DrawIndexed(m_OpaqueObjects[i]->Mesh->IndexCount, 0, 0);
	}

	m_OpaqueObjects.clear();
}

void RenderingSystem::RenderAllTransparentObjects()
{
	SortObjects(m_TransparentObjects, 0, m_TransparentObjects.size() - 1);
	// Do draw Stuff here

	for (int i = 0; i < m_TransparentObjects.size(); i++)
	{

		//Store this Objects data in constant buffer struct
		m_CbData.World = XMMatrixTranspose(XMLoadFloat4x4(&m_TransparentObjects[i]->WorldPos));

		//Write constant buffer data onto GPU
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		m_DevContext->Map(m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		memcpy(mappedSubresource.pData, &m_CbData, sizeof(m_CbData));
		m_DevContext->Unmap(m_ConstantBuffer, 0);


		XMFLOAT4 TempTransVal;
		XMStoreFloat4(&TempTransVal, m_TransparentObjects[i]->Transparency);
		m_DevContext->OMSetBlendState(m_TransparencyBS, &TempTransVal.x, 0xffffffff);

		// Set Vertex and Index Buffers
		m_DevContext->IASetVertexBuffers(0, 1, &m_TransparentObjects[i]->Mesh->VertexBuffer, 
												&m_TransparentObjects[i]->Mesh->VBStride,
												&m_TransparentObjects[i]->Mesh->VBOffset);

		m_DevContext->IASetIndexBuffer(m_TransparentObjects[i]->Mesh->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// Set Shader Resources (Textures) on the GPU
		m_DevContext->PSSetShaderResources(0, 1, &m_TransparentObjects[i]->ColourMap->Texture);
		m_DevContext->PSSetShaderResources(1, 1, &m_TransparentObjects[i]->SpecularMap->Texture);

		// Draw the Mesh
		m_DevContext->DrawIndexed(m_TransparentObjects[i]->Mesh->IndexCount, 0, 0);
	}
	m_TransparentObjects.clear();
}

void RenderingSystem::RenderAllSkybox()
{
	m_DevContext->OMSetBlendState(0, 0, 0xffffffff); // regular draw call

	// Set Vertex and Index Buffers
	m_DevContext->IASetVertexBuffers(0, 1, &m_SkyboxObject->Mesh->VertexBuffer, &m_SkyboxObject->Mesh->VBStride, &m_SkyboxObject->Mesh->VBOffset);
	m_DevContext->IASetIndexBuffer(m_SkyboxObject->Mesh->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set Shader Resources (Textures) on the GPU
	m_DevContext->PSSetShaderResources(0, 1, &m_SkyboxObject->ColourMap->Texture);

	// Draw the Mesh
	m_DevContext->DrawIndexed(m_SkyboxObject->Mesh->IndexCount, 0, 0);
}

void RenderingSystem::RenderAllSprites()
{
	for (int i = 0; i < m_2DSprites.size(); i++)
	{
		m_SpriteBatch->Draw(m_2DSprites[i]->Sprite->Texture, m_2DSprites[i]->DrawPosition);
	}
	m_2DSprites.clear();
}

void RenderingSystem::RenderAllText()
{
	for (int i = 0; i < m_2DTexts.size(); i++)
	{
		std::wstring Temp = m_2DTexts[i]->DrawString;

		m_SpriteFont->DrawString(m_SpriteBatch.get(), 
								Temp.c_str(),
								m_2DTexts[i]->DrawPosition, 
								m_2DTexts[i]->DrawColour, 
								m_2DTexts[i]->DrawRotation, 
								XMFLOAT2(0.0f,0.0f), 
								m_2DTexts[i]->DrawScale);
	}
	m_2DSprites.clear();
}

void RenderingSystem::SetCameraPosition(XMFLOAT3 camPos)
{
	m_CameraPos = camPos;
}

void RenderingSystem::SortObjects(std::vector<ObjectDrawDESC*>& array, int start, int end)
{
	if (start < end)
	{
		int pivotIndex = Partition(array, start, end);
		SortObjects(array, start, pivotIndex - 1);
		SortObjects(array, pivotIndex + 1, end);
	}
}

int RenderingSystem::Partition(std::vector<ObjectDrawDESC*>& array, int start, int end)
{
	int pivotIndex = end;

	int low = start - 1;

	for (int i = start; i <= end; i++)
	{
		// Check which distance is greater, Place swap it closer to front.
		if ((Distance(array[i]->ObjectPosition, m_CameraPos)) >
			(Distance(array[pivotIndex]->ObjectPosition, m_CameraPos)))
		{
				
			low++;
			std::swap(array[low], array[i]);
		}
	}
	std::swap(array[low + 1], array[end]);
	return low + 1;
}

void RenderingSystem::Release()
{
	// This is effectively a destructor for me to set pointers null
	if (m_DevContext) m_DevContext->Release();
	if (m_Device) m_Device->Release();
	if (m_DXGIDevice) m_DXGIDevice->Release();
	if (m_DXGIFactory) m_DXGIFactory->Release();
	if (m_FrameBufferView) m_FrameBufferView->Release();
	if (m_SwapChain) m_SwapChain->Release();

	if (m_FillRSState)m_FillRSState->Release();
	if (m_WireFrameRSState)m_WireFrameRSState->Release();
	if (m_SkyboxRSState)m_SkyboxRSState->Release();
	if (m_VertexShader)m_VertexShader->Release();
	if (m_SkyboxVertexShader)m_SkyboxVertexShader->Release();
	if (m_InputLayout)m_InputLayout->Release();
	if (m_PixelShader)m_PixelShader->Release();
	if (m_SkyboxPixelShader)m_SkyboxPixelShader->Release();
	if (m_ConstantBuffer)m_ConstantBuffer->Release();

	if (m_DepthStencilBuffer) m_DepthStencilBuffer->Release();
	if (m_DepthStencilView) m_DepthStencilView->Release();
	if (m_SkyBoxDepthStencilState) m_SkyBoxDepthStencilState->Release();
	if (m_SpriteBatch) m_SpriteBatch.release();
	if (m_SpriteFont) m_SpriteFont.release();

	delete m_TextureHandler;
	m_TextureHandler = nullptr;
	delete m_MeshHandler;
	m_MeshHandler = nullptr;
	m_OpaqueObjects.clear();
	m_TransparentObjects.clear();
}

void RenderingSystem::Update(float DeltaTime)
{
	//Sets Wireframe State on F1 Press
	if (GetAsyncKeyState(VK_F1) & 0x0001)
	{
		m_IsWireframeActive = true;
		m_IsFillActive = false;
	}
	// Sets Fill State on F2 Press
	else if (GetAsyncKeyState(VK_F2) & 0x0001)
	{
		m_IsFillActive = true;
		m_IsWireframeActive = false;
	}
}

void RenderingSystem::Render()
{
	m_DevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DevContext->IASetInputLayout(m_InputLayout);

	m_DevContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_DevContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);

	m_DevContext->PSSetSamplers(0, 1, &m_BilinearSamplerState);

	//Present unbinds render target, so rebind and clear at start of each frame
	float backgroundColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
	m_DevContext->OMSetRenderTargets(1, &m_FrameBufferView, m_DepthStencilView);
	m_DevContext->ClearRenderTargetView(m_FrameBufferView, backgroundColor);
	m_DevContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	// set variables for skybox draw
	m_DevContext->RSSetState(m_SkyboxRSState);
	m_DevContext->VSSetShader(m_SkyboxVertexShader, nullptr, 0);
	m_DevContext->PSSetShader(m_SkyboxPixelShader, nullptr, 0);
	m_DevContext->OMSetDepthStencilState(m_SkyBoxDepthStencilState, 0);

	// RENDER SKYBOX HERE
	RenderAllSkybox();

	// set shaders for drawing objects.
	if (m_IsFillActive)
		m_DevContext->RSSetState(m_FillRSState);
	else if (m_IsWireframeActive)
		m_DevContext->RSSetState(m_WireFrameRSState);

	m_DevContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_DevContext->PSSetShader(m_PixelShader, nullptr, 0);

	// RENDER OPAQUE 
	RenderAllOpaqueObjects();

	// RENDER TRANSPARENT
	RenderAllTransparentObjects();

		//2D SPRITE BATCHING
	m_SpriteBatch->Begin();

	RenderAllSprites();
	RenderAllText();

	m_SpriteBatch->End();

	//Present Backbuffer to screen
	m_SwapChain->Present(0, 0);
}

float RenderingSystem::Distance(const XMFLOAT3& v1, const XMFLOAT3& v2)
{
	XMVECTOR vector1 = XMLoadFloat3(&v1);
	XMVECTOR vector2 = XMLoadFloat3(&v2);
	XMVECTOR vectorSub = XMVectorSubtract(vector1, vector2);
	XMVECTOR length = XMVector3Length(vectorSub);

	float distance = 0.0f;
	XMStoreFloat(&distance, length);
	return distance;
}

RenderingSystem::RenderingSystem(HWND WindowHandle, ScreenInformation ScreenInfo) :
	m_WindowHandle(WindowHandle),
	m_ScreenInfo(ScreenInfo)
{
}

RenderingSystem::~RenderingSystem()
{
}

HRESULT RenderingSystem::Initialise()
{
	HRESULT hr = S_OK;

	hr = CreateD3DDevice();
	if (FAILED(hr)) return E_FAIL;

	hr = CreateSwapChain();
	if (FAILED(hr)) return E_FAIL;

	hr = InitFrameBuffer();
	if (FAILED(hr)) return E_FAIL;

	hr = InitShadersAndInputLayout();
	if (FAILED(hr)) return E_FAIL;

	hr = InitHandlers();
	if (FAILED(hr)) return E_FAIL;

	hr = InitPipelineStates();
	if (FAILED(hr)) return E_FAIL;

	return hr;
}
HRESULT RenderingSystem::CreateD3DDevice()
{
	HRESULT hr = S_OK;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
	};

	ID3D11Device* baseDevice;
	ID3D11DeviceContext* baseDeviceContext;

	DWORD createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT | createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &baseDevice, nullptr, &baseDeviceContext);
	if (FAILED(hr)) return hr;

	///////////////////////////////////////////////////////////////////////////////////////////////

	hr = baseDevice->QueryInterface(__uuidof(ID3D11Device), reinterpret_cast<void**>(&m_Device));
	hr = baseDeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext), reinterpret_cast<void**>(&m_DevContext));

	baseDevice->Release();
	baseDeviceContext->Release();

	///////////////////////////////////////////////////////////////////////////////////////////////

	hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&m_DXGIDevice));
	if (FAILED(hr)) return hr;

	IDXGIAdapter* dxgiAdapter;
	hr = m_DXGIDevice->GetAdapter(&dxgiAdapter);
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&m_DXGIFactory));
	dxgiAdapter->Release();

	//Viewport Values
	m_Viewport = { 0.0f, 0.0f, (float)m_ScreenInfo.WindowWidth, (float)m_ScreenInfo.WindowHeight, 0.0f, 1.0f };
	m_DevContext->RSSetViewports(1, &m_Viewport);

	return S_OK;
}
HRESULT RenderingSystem::CreateSwapChain()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = 0; // Defer to WindowWidth
	swapChainDesc.Height = 0; // Defer to WindowHeight
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //FLIP* modes don't support sRGB backbuffer
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	hr = m_DXGIFactory->CreateSwapChainForHwnd(m_Device, m_WindowHandle, &swapChainDesc, nullptr, nullptr, &m_SwapChain);
	if (FAILED(hr)) return hr;

	return hr;
}
HRESULT RenderingSystem::InitFrameBuffer()
{
	ID3D11Texture2D* frameBuffer = nullptr;

	HRESULT hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&frameBuffer));
	if (FAILED(hr)) return hr;

	D3D11_RENDER_TARGET_VIEW_DESC framebufferDesc = {};
	framebufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //sRGB render target enables hardware gamma correction
	framebufferDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = m_Device->CreateRenderTargetView(frameBuffer, &framebufferDesc, &m_FrameBufferView);

	D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
	frameBuffer->GetDesc(&DepthBufferDesc);

	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	m_Device->CreateTexture2D(&DepthBufferDesc, nullptr, &m_DepthStencilBuffer);
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer, nullptr, &m_DepthStencilView);

	frameBuffer->Release();

	return hr;
}
HRESULT RenderingSystem::InitShadersAndInputLayout()
{
	HRESULT hr = S_OK;
	ID3DBlob* errorBlob;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* vsBlob;

	hr = D3DCompileFromFile(L"SimpleShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBoxA(m_WindowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
		errorBlob->Release();
		return hr;
	}

	hr = m_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_VertexShader);

	if (FAILED(hr)) return hr;

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

	};

	hr = m_Device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout);
	if (FAILED(hr)) return hr;

	///////////////////////////////////////////////////////////////////////////////////////////////

	ID3DBlob* psBlob;

	hr = D3DCompileFromFile(L"SimpleShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBoxA(m_WindowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
		errorBlob->Release();
		return hr;
	}

	hr = m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PixelShader);
	if (FAILED(hr)) return hr;

	vsBlob->Release();
	psBlob->Release();


	// Compile Skybox Vertex Shader at runtime, throw error box if error occurs.
	hr = D3DCompileFromFile(L"SkyboxShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS_main", "vs_5_0", dwShaderFlags, 0, &vsBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBoxA(m_WindowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
		errorBlob->Release();
		return hr;
	}

	// Create skybox vertex shader using the compiled shader information, throw a error box if error occurs.
	hr = m_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_SkyboxVertexShader);
	if (FAILED(hr)) return hr;

	// Compile skybox pixel shader at runtime, throw error box if error occurs.
	hr = D3DCompileFromFile(L"SkyboxShaders.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS_main", "ps_5_0", dwShaderFlags, 0, &psBlob, &errorBlob);
	if (FAILED(hr))
	{
		MessageBoxA(m_WindowHandle, (char*)errorBlob->GetBufferPointer(), nullptr, ERROR);
		errorBlob->Release();
		return hr;
	}

	// Create skybox pixel shader using compiled shader information, throw a error box if error occurs.
	hr = m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_SkyboxPixelShader);
	if (FAILED(hr)) return hr;

	// Release Vertex and Pixel shader blobs again since we are finished with these.
	vsBlob->Release();
	psBlob->Release();

	return hr;
}
HRESULT RenderingSystem::InitPipelineStates()
{
	HRESULT hr = S_OK;

	//Input Assembler
	m_DevContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DevContext->IASetInputLayout(m_InputLayout);

	//_filLState Rasterizer
	D3D11_RASTERIZER_DESC FillStateDesc = {};
	FillStateDesc.FillMode = D3D11_FILL_SOLID;
	FillStateDesc.CullMode = D3D11_CULL_BACK;

	hr = m_Device->CreateRasterizerState(&FillStateDesc, &m_FillRSState);
	if (FAILED(hr)) return hr;

	//_wireframeState
	D3D11_RASTERIZER_DESC WireFrameDesc = {};
	WireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
	WireFrameDesc.CullMode = D3D11_CULL_NONE;

	hr = m_Device->CreateRasterizerState(&WireFrameDesc, &m_WireFrameRSState);
	if (FAILED(hr)) return hr;

	//_skyboxState
	D3D11_RASTERIZER_DESC SkyboxDesc = {};
	SkyboxDesc.FillMode = D3D11_FILL_SOLID;
	SkyboxDesc.CullMode = D3D11_CULL_NONE;

	hr = m_Device->CreateRasterizerState(&SkyboxDesc, &m_SkyboxRSState);
	if (FAILED(hr)) return hr;

	m_DevContext->RSSetState(m_FillRSState);

	//Constant Buffer
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_Device->CreateBuffer(&constantBufferDesc, nullptr, &m_ConstantBuffer);
	if (FAILED(hr)) { return hr; }

	m_DevContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_DevContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);

	// Texture Sampler
	D3D11_SAMPLER_DESC BilinearSamplerDesc = {};
	BilinearSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	BilinearSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	BilinearSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	BilinearSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	BilinearSamplerDesc.MaxLOD = 1;
	BilinearSamplerDesc.MinLOD = 0;

	hr = m_Device->CreateSamplerState(&BilinearSamplerDesc, &m_BilinearSamplerState);
	if (FAILED(hr)) return hr;

	m_DevContext->PSSetSamplers(0, 1, &m_BilinearSamplerState);

	// Skybox Depth Stencil State
	D3D11_DEPTH_STENCIL_DESC dsDescSkybox = {};
	dsDescSkybox.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDescSkybox.DepthEnable = true;
	dsDescSkybox.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_Device->CreateDepthStencilState(&dsDescSkybox, &m_SkyBoxDepthStencilState);

	// Transparency Blending
	D3D11_BLEND_DESC blendDesc = {};

	D3D11_RENDER_TARGET_BLEND_DESC rtbd = {};
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	m_Device->CreateBlendState(&blendDesc, &m_TransparencyBS);

	return S_OK;
}
HRESULT RenderingSystem::InitHandlers()
{
	m_MeshHandler = new MeshHandler(m_Device); // This handles loading and unloading meshes as required.
	m_TextureHandler = new TextureHandler(m_Device); // this handles loading and unloading textures as required.

	if (m_MeshHandler == nullptr)
		return E_FAIL;

	if (m_TextureHandler == nullptr)
		return E_FAIL;

	m_SpriteBatch = std::make_unique<SpriteBatch>(m_DevContext);
	m_SpriteFont = std::make_unique<SpriteFont>(m_Device, L"Fonts/ComicSansSpriteSheet16.spritefont");

	if (m_SpriteBatch == nullptr)
		return E_FAIL;

	if (m_SpriteFont == nullptr)
		return E_FAIL;

	return S_OK;
}
