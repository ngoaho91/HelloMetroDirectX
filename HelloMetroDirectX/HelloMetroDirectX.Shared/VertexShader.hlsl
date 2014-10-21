cbuffer ConstantBuffer
{
	float4x4 transformation;
	float4x4 rotation;
	float4 light_direction;
	float4 diffuse_color;
	float4 ambient_color;
	float opacity;
}
struct VS_INPUT
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float2 uv : UV;
};
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float opacity : OPACITY;
};
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(transformation, input.position);
	float4 normal = normalize(mul(rotation, input.normal));
	float diffusal = saturate(dot(normal, light_direction));
	output.color = ambient_color + (diffuse_color*diffusal);
	output.uv = input.uv;
	output.opacity = opacity;
	return output;
}