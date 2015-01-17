
cbuffer LightBuffer : register(b0) {
	float4 c_lightDirection;
};

cbuffer MaterialBuffer : register(b1) {
	float4 c_ambient;
	float4 c_diffuse;
	float4 c_emissive;
	float4 c_speculer;
}

struct InputPS {
	float4 pos    : SV_POSITION;
	float4 color  : COLOR0;          // 頂点カラーはジオメトリシェーダー内で追加する
	float3 eye	  : COLOR1;
	float3 normal : NORMAL;
	float2 texel  : TEXCOORD0;
	uint   viewportIndex : SV_ViewportArrayIndex;
};

Texture2D  t_diffuse : register(t0);
SamplerState s_diffuse  : register(s0);

//! ピクセルシェーダ
float4 RenderPS(InputPS input) : SV_TARGET
{
	return t_diffuse.Sample(s_diffuse, input.texel);
/*	float p = max(dot(input.normal, normalize(c_lightDirection.xyz)), 0.f);
	p = p * 0.5f + 0.5f;
	p = p * p;
	
	float4 output = t_diffuse.Sample(s_diffuse, input.texel) + float4(c_diffuse);
	output.rgb *= p;
	return output;*/
}
