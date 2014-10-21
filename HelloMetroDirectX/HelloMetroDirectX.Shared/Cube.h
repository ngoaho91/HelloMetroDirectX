#pragma once
#include "pch.h"
#include "GameHelper.h"

using namespace Microsoft::WRL;
using namespace DirectX;

struct VERTEX
{
	float X, Y, Z;
	float NX, NY, NZ;
	float U, V;
};
struct RENDER_CONSTANTS
{
	XMMATRIX transformation;
	XMMATRIX rotation;
	XMVECTOR lightDirection;
	XMVECTOR diffuseColor;
	XMVECTOR ambientColor;
	float opacity;
};
class Cube
{
private:
	ID3D11Device1*						device;
	ID3D11DeviceContext1*				context;
private:
	ComPtr<ID3D11Buffer>				vertexBuffer;
	ComPtr<ID3D11Buffer>				indexBuffer;
	ComPtr<ID3D11Buffer>				constantBuffer;
	ComPtr<ID3D11VertexShader>			vertexShader;
	ComPtr<ID3D11PixelShader>			pixelShader;
	ComPtr<ID3D11ShaderResourceView>	texture;
private:
	static VERTEX		vertices[24];
	static short		indices[36];
	XMMATRIX			matTranslate;
	XMMATRIX			matRotateX;
	XMMATRIX			matRotateY;
	XMMATRIX			matRotateZ;
	XMMATRIX			matRotation;
	XMMATRIX			matScale;
	XMMATRIX			matModel;
	RENDER_CONSTANTS	constants;
private:
	bool				isBlinking;
	float				opacityAccel;
public:
	Cube();
	~Cube();
	void SetDevice(ID3D11Device1* device);
	void SetContext(ID3D11DeviceContext1* context);
	void BuildDirectXData();
	void SetTranslation(float x, float y, float z);
	void SetRotationX(float angle_rad);
	void SetRotationY(float angle_rad);
	void SetRotationZ(float angle_rad);
	void SetScale(float scale);
	void SetTexture(std::wstring file_path);
	void Draw(XMMATRIX& view_projection_matrix);
public:
	void Update();
	void SetBlinking(bool value);
	bool RayIntersects(XMVECTOR& ray_begin, XMVECTOR& ray_end);
	bool TriangleRayIntersects(XMVECTOR& ray_begin, XMVECTOR& ray_end,
		XMVECTOR& triangle_a, XMVECTOR& triangle_b, XMVECTOR& triangle_c);
};