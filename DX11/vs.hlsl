struct InputVS {
	float3 pos			: IN_POSITION;
	float4 color		: IN_COLOR;
	float3 normal		: IN_NORMAL;
	float2 texel        : IN_TEXCOORD;
};
struct OutputVS {
	float4 pos			: SV_POSITION;
	float4 color		: COLOR0;
	float3 eye			: COLOR1;
	float3 normal       : NORMAL;
	float2 texel        : TEXCOORD0;
};

OutputVS RenderVS(InputVS input) {
	OutputVS output = { float4(input.pos, 1.0f), input.color, float3(0.f, 0.f, 0.f), input.normal, input.texel };
	return output;
}