struct VS_OUTPUT
{
	float4 postion : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;
	float opacity : OPACITY;
};
struct FS_OUTPUT
{
	float4 color : SV_TARGET;
};
Texture2D wood_texture;
Texture2D bricks_texture;
SamplerState samper_config;

FS_OUTPUT main(VS_OUTPUT input)
{
	FS_OUTPUT output;
	float4 wood_color = wood_texture.Sample(samper_config, input.uv);
	float4 bricks_color = bricks_texture.Sample(samper_config, input.uv);
	output.color = input.color*(wood_color*0.8f +bricks_color*0.2f);
	output.color.a = input.opacity;
	return output;
}