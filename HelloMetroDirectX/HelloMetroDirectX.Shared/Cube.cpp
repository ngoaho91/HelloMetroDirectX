#include "pch.h"
#include "Cube.h"
using namespace DirectX;
VERTEX Cube::vertices[24] =
{
	{ -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },    // side 1
	{ 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
	{ -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },

	{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },    // side 2
	{ -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f },
	{ 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f },

	{ -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },    // side 3
	{ -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
	{ 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },

	{ -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },    // side 4
	{ 1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f },
	{ -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f },
	{ 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f },

	{ 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },    // side 5
	{ 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
	{ 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },

	{ -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },    // side 6
	{ -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
	{ -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
	{ -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
};
short Cube::indices[36] =
{
	0, 1, 2,    // side 1
	2, 1, 3,
	4, 5, 6,    // side 2
	6, 5, 7,
	8, 9, 10,    // side 3
	10, 9, 11,
	12, 13, 14,    // side 4
	14, 13, 15,
	16, 17, 18,    // side 5
	18, 17, 19,
	20, 21, 22,    // side 6
	22, 21, 23,
};
Cube::Cube()
{
	matRotateX = XMMatrixIdentity();
	matRotateY = XMMatrixIdentity();
	matRotateZ = XMMatrixIdentity();
	matTranslate = XMMatrixIdentity();
	matScale = XMMatrixIdentity();
	isBlinking = false;
	constants.opacity = 1.0f;
}
Cube::~Cube()
{

}
void Cube::SetBlinking(bool value)
{
	isBlinking = value;
	constants.opacity = 1.0f;
	opacityAccel = 0.08f;
}
void Cube::SetDevice(ID3D11Device1* _device)
{
	device = _device;
}
void Cube::SetContext(ID3D11DeviceContext1* _context)
{
	context = _context;
}
void Cube::BuildDirectXData()
{
	{// constants buffer
		constants.lightDirection = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		constants.diffuseColor = XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f);
		constants.ambientColor = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		D3D11_BUFFER_DESC bd = { 0 };
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(constants);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		device->CreateBuffer(&bd, nullptr, &constantBuffer);
		
	}
	{// load shaders
		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(vertices);
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA srd = { vertices, 0, 0 };

		device->CreateBuffer(&bd, &srd, &vertexBuffer);

		Array<byte>^ vs_file = LoadShaderFile("VertexShader.cso");
		Array<byte>^ ps_file = LoadShaderFile("PixelShader.cso");
		device->CreateVertexShader(vs_file->Data, vs_file->Length, nullptr, &vertexShader);
		device->CreatePixelShader(ps_file->Data, ps_file->Length, nullptr, &pixelShader);
		context->VSSetShader(vertexShader.Get(), nullptr, 0);
		context->PSSetShader(pixelShader.Get(), nullptr, 0);

		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		ComPtr<ID3D11InputLayout> input_layout;
		device->CreateInputLayout(ied, ARRAYSIZE(ied), vs_file->Data, vs_file->Length, &input_layout);
		context->IASetInputLayout(input_layout.Get());
	}
	{
		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(short) * ARRAYSIZE(indices);
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA srd = { indices, 0, 0 };
		device->CreateBuffer(&bd, &srd, &indexBuffer);
	}
}
void Cube::SetTranslation(float x, float y, float z)
{
	matTranslate = XMMatrixTranslation(x, y, z);
}
void Cube::SetRotationX(float angle_rad)
{
	matRotateX = XMMatrixRotationX(angle_rad);
}
void Cube::SetRotationY(float angle_rad)
{
	matRotateY = XMMatrixRotationY(angle_rad);
}
void Cube::SetRotationZ(float angle_rad)
{
	matRotateZ = XMMatrixRotationZ(angle_rad);
}
void Cube::SetScale(float scale)
{
	matScale = XMMatrixScaling(scale, scale, scale);
}
void Cube::SetTexture(std::wstring file_path)
{
	CreateWICTextureFromFile(device, nullptr, file_path.c_str(), nullptr, &texture, 0);
}
void Cube::Update()
{
	if (isBlinking)
	{
		constants.opacity += opacityAccel;
		if (constants.opacity > 1.0f || constants.opacity < 0.0f)
		{
			opacityAccel = -opacityAccel;
		}
	}
}
void Cube::Draw(XMMATRIX& view_projection_matrix)
{
	matRotation = matRotateX * matRotateY * matRotateZ;
	matModel = matScale * matTranslate * matRotation;
	constants.transformation = matModel * view_projection_matrix;
	XMVECTOR cube_position = XMVector4Transform(XMVectorSet(0.0, 0.0, 0.0, 1.0), constants.transformation);
	constants.rotation = matRotation;
	context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());
	context->UpdateSubresource(constantBuffer.Get(), 0, 0, &constants, 0, 0);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->PSSetShaderResources(0, 1, texture.GetAddressOf());
	context->DrawIndexed(36, 0, 0);
}
bool Cube::RayIntersects(XMVECTOR& ray_begin, XMVECTOR& ray_end)
{
	matRotation = matRotateX * matRotateY * matRotateZ;
	matModel = matTranslate * matRotation * matScale;
	XMVECTOR A, B, C, D, E, F, G, H;
	A = XMVector4Transform(XMVectorSet(vertices[0].X, vertices[0].Y, vertices[0].Z, 1.0f), matModel);
	B = XMVector4Transform(XMVectorSet(vertices[1].X, vertices[1].Y, vertices[1].Z, 1.0f), matModel);
	C = XMVector4Transform(XMVectorSet(vertices[2].X, vertices[2].Y, vertices[2].Z, 1.0f), matModel);
	D = XMVector4Transform(XMVectorSet(vertices[3].X, vertices[3].Y, vertices[3].Z, 1.0f), matModel);
	E = XMVector4Transform(XMVectorSet(vertices[4].X, vertices[4].Y, vertices[4].Z, 1.0f), matModel);
	G = XMVector4Transform(XMVectorSet(vertices[5].X, vertices[5].Y, vertices[5].Z, 1.0f), matModel);
	F = XMVector4Transform(XMVectorSet(vertices[6].X, vertices[6].Y, vertices[6].Z, 1.0f), matModel);
	H = XMVector4Transform(XMVectorSet(vertices[7].X, vertices[7].Y, vertices[7].Z, 1.0f), matModel); 
	if (TriangleRayIntersects(ray_begin, ray_end, A, B, C))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, A, C, D))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, E, F, G))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, E, G, H))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, A, E, H))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, A, H, D))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, A, E, F))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, A, F, B))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, B, F, G))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, B, G, C))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, C, G, H))
	{
		return true;
	}
	if (TriangleRayIntersects(ray_begin, ray_end, C, H, D))
	{
		return true;
	}
	return false;
}
#define SMALL_NUM   0.00000001
#define DOT(VECT1, VECT2) XMVector3Dot(VECT1, VECT2).m128_f32[0]
bool Cube::TriangleRayIntersects(XMVECTOR& ray_begin, XMVECTOR& ray_end,
	XMVECTOR& triangle_a, XMVECTOR& triangle_b, XMVECTOR& triangle_c)
{
	XMVECTOR u, v, n;
	XMVECTOR ray_dir, w0, w;
	float r, a, b;
	u = XMVectorSubtract(triangle_b, triangle_a);
	v = XMVectorSubtract(triangle_c, triangle_a);
	n = XMVector3Cross(u, v);
	ray_dir = XMVectorSubtract(ray_end, ray_begin);
	w0 = ray_begin - triangle_a;
	a = -DOT(n, w0);
	b = DOT(n, ray_dir);
	if (abs(b) < SMALL_NUM)// parallel
	{
		if (a == 0)// ray lie on triangle
		{
			return true;
		}
		return false;
	}
	r = a / b;
	if (r < 0.0)// triangle is behind ray
	{
		return false;
	}
	XMVECTOR intersection;
	intersection = XMVectorAdd(ray_begin, XMVectorScale(ray_dir, r));
	float uu, uv, vv, wu, wv, d;
	uu = DOT(u, u);
	uv = DOT(u, v);
	vv = DOT(v, v);
	w = XMVectorSubtract(intersection, triangle_a);
	wu = DOT(w, u);
	wv = DOT(w, v);
	d = uv * uv - uu * vv;

	// get and test parametric coords
	float s, t;
	s = (uv * wv - vv * wu) / d;
	if (s < 0.0 || s > 1.0)
		return false;
	t = (uv * wu - uu * wv) / d;
	if (t < 0.0 || (s + t) > 1.0)
		return false;

	return true;
}