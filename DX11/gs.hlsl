struct Camera {
	matrix matView;
	matrix matProjection;
	float4 eyePosition;
};

cbuffer WorldBuffer : register(b0) {
	matrix	c_matWorld;
};

cbuffer CameraBuffer : register(b1) {
	Camera c_cameras[4];
};

cbuffer ViewportBuffer : register(b2) {
	float c_viewportDrawable;
	float c_viewportNum;
};

struct InputGS {
	float4 pos			: SV_POSITION;
	float4 color		: COLOR0;
	float3 eye			: COLOR1;
	float3 normal       : NORMAL;
	float2 texel        : TEXCOORD0;
};

struct OutputGS {
	float4 pos    : SV_POSITION;
	float4 color  : COLOR0;          // 頂点カラーはジオメトリシェーダー内で追加する
	float3 eye	  : COLOR1;
	float3 normal : NORMAL;
	float2 texel  : TEXCOORD0;
	uint   viewportIndex : SV_ViewportArrayIndex;
};

[maxvertexcount(12)] // ジオメトリシェーダーで出力する最大頂点数
void RenderGS(
	triangle InputGS input[3],                // トライアングル リストを構成する頂点配列の入力情報
	inout TriangleStream<OutputGS> TriStream   // 頂点情報を追加するためのストリームオブジェクト
) {
	int drawable = c_viewportDrawable; //ビット演算のためint型にキャスト
	for (int i_viewport = 0; i_viewport < c_viewportNum; i_viewport++) {
		if (drawable & 0x01) {
			matrix matVP = mul(c_cameras[i_viewport].matView, c_cameras[i_viewport].matProjection);
			matrix matWVP = mul(c_matWorld, matVP);

			// もとの頂点を出力
			for (int i_vertex = 0; i_vertex < 3; i_vertex++) {
				OutputGS output;
				output.pos    = mul(input[i_vertex].pos,    matWVP);
				output.normal = mul(input[i_vertex].normal, c_matWorld);
				output.texel  = input[i_vertex].texel;
				output.color  = input[i_vertex].color;
				output.eye    = normalize(c_cameras[i_viewport].eyePosition.xyz - input[i_vertex].pos.xyz);
				output.viewportIndex = i_viewport;

				// 頂点を追加する
				TriStream.Append(output);
			}
			// 現在のストリップを終了し、新しいストリップを開始する。
			TriStream.RestartStrip();
		}
		drawable >>= 1;
	}
}