#include "pch.h"
#include "Game.h"
#include <fstream>


void Game::Initialize()
{
	CoreWindow^ window = CoreWindow::GetForCurrentThread();    // get the window pointer
	{// init device & context
		ComPtr<ID3D11Device> _device;
		ComPtr<ID3D11DeviceContext> _context;
		D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
			D3D11_SDK_VERSION, &_device, NULL, &_context);
		_device.As(&device);
		_context.As(&context);
	}
	{// swap buffer
		ComPtr<IDXGIDevice1> dxgi_device;
		device.As(&dxgi_device);
		ComPtr<IDXGIAdapter> dxgi_adapter;
		ComPtr<IDXGIFactory2> dxgi_factory;
		dxgi_device->GetAdapter(&dxgi_adapter);
		dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), &dxgi_factory);

		DXGI_SWAP_CHAIN_DESC1 scd = { 0 };
		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 2;
		scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scd.SampleDesc.Count = 1;

		dxgi_factory->CreateSwapChainForCoreWindow(device.Get(), reinterpret_cast<IUnknown*>(window),
			&scd, NULL,	&swapChain);
	}
	{// render target
		ComPtr<ID3D11Texture2D> buffer;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &buffer);
		device->CreateRenderTargetView(buffer.Get(), NULL, &renderTarget);
	}
	{// create texture as z buffer
		D3D11_TEXTURE2D_DESC texd = { 0 };

		texd.Width = window->Bounds.Width;
		texd.Height = window->Bounds.Height;
		texd.ArraySize = 1;
		texd.MipLevels = 1;
		texd.SampleDesc.Count = 1;
		texd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ComPtr<ID3D11Texture2D> zbuffertexture;
		device->CreateTexture2D(&texd, nullptr, &zbuffertexture);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		
		device->CreateDepthStencilView(zbuffertexture.Get(), &dsvd, &zBuffer);

		D3D11_DEPTH_STENCIL_DESC dsd = { 0 };
		dsd.DepthEnable = true;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsd.DepthFunc = D3D11_COMPARISON_LESS;
		device->CreateDepthStencilState(&dsd, defaultDepthState.GetAddressOf());
		dsd.DepthEnable = false;
		device->CreateDepthStencilState(&dsd, offDepthState.GetAddressOf());

		D3D11_VIEWPORT viewport = { 0 };
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = window->Bounds.Width;
		viewport.Height = window->Bounds.Height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		context->RSSetViewports(1, &viewport);

		context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), zBuffer.Get());
		context->ClearDepthStencilView(zBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		
	}
	{
		D3D11_RASTERIZER_DESC rd;
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.FrontCounterClockwise = FALSE;
		rd.DepthClipEnable = TRUE;
		rd.ScissorEnable = FALSE;
		rd.AntialiasedLineEnable = FALSE;
		rd.MultisampleEnable = FALSE;
		rd.DepthBias = 0;
		rd.DepthBiasClamp = 0.0f;
		rd.SlopeScaledDepthBias = 0.0f;

		device->CreateRasterizerState(&rd, &defaultRasterizerState);
		
		rd.FillMode = D3D11_FILL_WIREFRAME;
		rd.AntialiasedLineEnable = TRUE;

		device->CreateRasterizerState(&rd, &wireRasterizerState);
	}
	{
		D3D11_BLEND_DESC bd;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		bd.IndependentBlendEnable = FALSE;
		bd.AlphaToCoverageEnable = FALSE;

		device->CreateBlendState(&bd, &blendState);
	}
	{
		D3D11_SAMPLER_DESC sd;
		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		sd.MaxAnisotropy = 16;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0.0f;
		sd.MaxLOD = FLT_MAX;
		sd.MipLODBias = 0.0f;

		device->CreateSamplerState(&sd, &samplerState);    // create the anisotropic sampler
	}
	InitGraphics();
}
void Game::InitGraphics()
{
	color[0] = 0.0; color[1] = 0.0; color[2] = 0.0; color[3] = 1.0;
	colorAccel[0] = random(-MAX_ACCEL, MAX_ACCEL);
	colorAccel[1] = random(-MAX_ACCEL, MAX_ACCEL);
	colorAccel[2] = random(-MAX_ACCEL, MAX_ACCEL);

	enableWireFrame = false;
	enableDepth = true;
	time = 0.0f;
	wood_cube.SetDevice(device.Get());
	wood_cube.SetContext(context.Get());
	wood_cube.BuildDirectXData();
	brick_cube.SetDevice(device.Get());
	brick_cube.SetContext(context.Get());
	brick_cube.BuildDirectXData();
	solid_cube.SetDevice(device.Get());
	solid_cube.SetContext(context.Get());
	solid_cube.BuildDirectXData();

	for (int i = 0; i < 20; i++)
	{
		ray_cube[i].SetDevice(device.Get());
		ray_cube[i].SetContext(context.Get());
		ray_cube[i].BuildDirectXData();
	}

	wood_cube.SetTexture(L"Wood.png");
	wood_cube.SetTranslation(0.0, 0.0, 0.0);
	brick_cube.SetTranslation(-3.0, -3.0, 0);
	brick_cube.SetTexture(L"bricks.png");
	solid_cube.SetTranslation(3.0, -3.0, 0);
	solid_cube.SetTexture(L"Wood.png");

	for (int i = 0; i < 20; i++)
	{
		ray_cube[i].SetTexture(L"Wood.png");
		ray_cube[i].SetTranslation(0.0, 0.0, 0.0);
		ray_cube[i].SetScale(0.01);
	}
	isPaused = false;
}
void Game::Update()
{
	if (isPaused)
	{
		return;
	}
	color[0] += colorAccel[0];
	color[1] += colorAccel[1];
	color[2] += colorAccel[2];
	if (color[0] <= 0.0f || color[0] >= 1.0f || abs(colorAccel[0]) < MIN_ACCEL)
	{
		colorAccel[0] = random(-MAX_ACCEL, MAX_ACCEL);
	}
	if (color[1] <= 0.0f || color[1] >= 1.0f || abs(colorAccel[1]) < MIN_ACCEL)
	{
		colorAccel[1] = random(-MAX_ACCEL, MAX_ACCEL);
	}
	if (color[2] <= 0.0f || color[2] >= 1.0f || abs(colorAccel[2]) < MIN_ACCEL)
	{
		colorAccel[2] = random(-MAX_ACCEL, MAX_ACCEL);
	}

	
	XMVECTOR cam_position = XMVectorSet(1.5f, 3.5f, 15.5f, 0);
	XMVECTOR look_at = XMVectorSet(0, 0, 0, 0);
	XMVECTOR cam_up = XMVectorSet(0, 1, 0, 0);
	matView = XMMatrixLookAtLH(cam_position, look_at, cam_up);
	
	CoreWindow^ window = CoreWindow::GetForCurrentThread();
	matProjection = XMMatrixPerspectiveFovLH(
		PI*0.25,
		(FLOAT)window->Bounds.Width / (FLOAT)window->Bounds.Height,
		1,
		100);
	time += 0.05f;
	wood_cube.SetRotationY(time);
	solid_cube.SetRotationY(time*1.5);
	brick_cube.SetRotationY(time*2);
	wood_cube.Update();
	solid_cube.Update();
	brick_cube.Update();
}
void Game::Pause()
{
	isPaused = true;
}
void Game::Resume()
{
	isPaused = false;
}
void Game::Render()
{
	
	context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), zBuffer.Get());
	context->ClearRenderTargetView(renderTarget.Get(), color);
	context->ClearDepthStencilView(zBuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (enableWireFrame)
	{
		context->RSSetState(wireRasterizerState.Get());
	}
	else
	{
		context->RSSetState(defaultRasterizerState.Get());
	}
	if (enableDepth)
		context->OMSetDepthStencilState(defaultDepthState.Get(), 0);
	else
		context->OMSetDepthStencilState(offDepthState.Get(), 0);
	context->OMSetBlendState(blendState.Get(), 0, 0xffffffff);
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	{
		XMMATRIX camera_transform = matView * matProjection;
		wood_cube.Draw(camera_transform);
		brick_cube.Draw(camera_transform);
		solid_cube.Draw(camera_transform);

		for (int i = 0; i < 20; i++)
		{
			ray_cube[i].Draw(camera_transform);
		}
	}
	swapChain->Present(1, 0);
}
void Game::SwitchWireFrame()
{
	enableWireFrame = !enableWireFrame;
}
void Game::SwitchDepth()
{
	enableDepth = !enableDepth;
}
int Game::ScreenPositionToObject(float x, float y)
{
	XMVECTOR transformed_ray_begin = XMVectorSet(x, -y, 0.0f, 1.0f);
	XMVECTOR transformed_ray_end = XMVectorSet(x, -y, 1.0f, 1.0f);
	XMVECTOR ray_begin;
	XMVECTOR ray_end;
	XMMATRIX camera_transform = matView * matProjection;
	XMVECTOR det = XMMatrixDeterminant(camera_transform);
	XMMATRIX inverse_transform = XMMatrixInverse(&det, camera_transform);

	ray_begin = XMVector4Transform(transformed_ray_begin, inverse_transform);
	if (ray_begin.m128_f32[3] != 0) ray_begin = XMVectorScale(ray_begin, 1.0 / ray_begin.m128_f32[3]);
	ray_end = XMVector4Transform(transformed_ray_end, inverse_transform);
	if (ray_end.m128_f32[3] != 0) ray_end = XMVectorScale(ray_end, 1.0 / ray_end.m128_f32[3]);

	XMVECTOR ray = XMVectorSubtract(ray_end, ray_begin);
	// visually show the ray, draw 20 small cube along it
	for (int i = 0; i < 20; i++)
	{
		XMVECTOR position = XMVectorLerp(ray_begin, ray_end, (float)i / 20);
		ray_cube[i].SetTranslation(position.m128_f32[0], position.m128_f32[1], position.m128_f32[2]);
	}
	if (wood_cube.RayIntersects(ray_begin, ray_end))
	{
		wood_cube.SetBlinking(true);
		brick_cube.SetBlinking(false);
		solid_cube.SetBlinking(false);
		return 1;
	}
	if (brick_cube.RayIntersects(ray_begin, ray_end))
	{
		wood_cube.SetBlinking(false);
		brick_cube.SetBlinking(true);
		solid_cube.SetBlinking(false);
		return 2;
	}
	if (solid_cube.RayIntersects(ray_begin, ray_end))
	{
		wood_cube.SetBlinking(false);
		brick_cube.SetBlinking(false);
		solid_cube.SetBlinking(true);
		return 3;
	}
	return 0;
}