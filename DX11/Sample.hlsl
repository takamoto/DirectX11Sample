// サンプル描画

//! コンスタントバッファ
/*!
コンスタントバッファを使用することでいくつかのコンスタントレジスタを1まとめにすることができる
レジスタの変化のタイミングによって分けておく方がよい
*/
cbuffer cbSceneParam : register( b0 )
{
	matrix	mtxView			: packoffset( c0 );
	matrix	mtxProj			: packoffset( c4 );
};

cbuffer cbMeshParam : register( b1 )
{
	matrix	mtxWorld		: packoffset( c0 );
};

Texture2D  texDiffuse  : register(t0);
SamplerState samDiffuse  : register(s0);

//! 頂点属性
/*!
基本はD3D9と変わらない
ただし、いくつかのセマンティクスが変わっている
システム側に渡すセマンティクスはSV_の接頭辞がついている
*/
struct InputVS
{
	float3 pos			: IN_POSITION;
	float2 uv			: IN_UV;
	float4 color		: IN_COLOR;
};
struct OutputVS
{
	float4 pos_wvp		: SV_POSITION; // システムに渡す頂点の位置
	float2 uv			: IN_UV;
	float4 color		: IN_COLOR;
};

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	matrix	mtxWV = mul(mtxWorld, mtxView);
	matrix	mtxWVP = mul(mtxWV, mtxProj);

	OutputVS	outVert;
	outVert.pos_wvp = mul(float4(inVert.pos, 1), mtxWVP);
	outVert.color = inVert.color;
	outVert.uv = inVert.uv;

	return outVert;
}


//! ピクセルシェーダ
float4 RenderPS(OutputVS inPixel) : SV_TARGET
{
	return texDiffuse.Sample(samDiffuse, inPixel.uv);
	//return inPixel.color;
}