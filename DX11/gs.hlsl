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
	float4 color  : COLOR0;          // ���_�J���[�̓W�I���g���V�F�[�_�[���Œǉ�����
	float3 eye	  : COLOR1;
	float3 normal : NORMAL;
	float2 texel  : TEXCOORD0;
	uint   viewportIndex : SV_ViewportArrayIndex;
};

[maxvertexcount(12)] // �W�I���g���V�F�[�_�[�ŏo�͂���ő咸�_��
void RenderGS(
	triangle InputGS input[3],                // �g���C�A���O�� ���X�g���\�����钸�_�z��̓��͏��
	inout TriangleStream<OutputGS> TriStream   // ���_����ǉ����邽�߂̃X�g���[���I�u�W�F�N�g
) {
	int drawable = c_viewportDrawable; //�r�b�g���Z�̂���int�^�ɃL���X�g
	for (int i_viewport = 0; i_viewport < c_viewportNum; i_viewport++) {
		if (drawable & 0x01) {
			matrix matVP = mul(c_cameras[i_viewport].matView, c_cameras[i_viewport].matProjection);
			matrix matWVP = mul(c_matWorld, matVP);

			// ���Ƃ̒��_���o��
			for (int i_vertex = 0; i_vertex < 3; i_vertex++) {
				OutputGS output;
				output.pos    = mul(input[i_vertex].pos,    matWVP);
				output.normal = mul(input[i_vertex].normal, c_matWorld);
				output.texel  = input[i_vertex].texel;
				output.color  = input[i_vertex].color;
				output.eye    = normalize(c_cameras[i_viewport].eyePosition.xyz - input[i_vertex].pos.xyz);
				output.viewportIndex = i_viewport;

				// ���_��ǉ�����
				TriStream.Append(output);
			}
			// ���݂̃X�g���b�v���I�����A�V�����X�g���b�v���J�n����B
			TriStream.RestartStrip();
		}
		drawable >>= 1;
	}
}