// �T���v���`��

//! �R���X�^���g�o�b�t�@
/*!
�R���X�^���g�o�b�t�@���g�p���邱�Ƃł������̃R���X�^���g���W�X�^��1�܂Ƃ߂ɂ��邱�Ƃ��ł���
���W�X�^�̕ω��̃^�C�~���O�ɂ���ĕ����Ă��������悢
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

//! ���_����
/*!
��{��D3D9�ƕς��Ȃ�
�������A�������̃Z�}���e�B�N�X���ς���Ă���
�V�X�e�����ɓn���Z�}���e�B�N�X��SV_�̐ړ��������Ă���
*/
struct InputVS
{
	float3 pos			: IN_POSITION;
	float2 uv			: IN_UV;
	float4 color		: IN_COLOR;
};
struct OutputVS
{
	float4 pos_wvp		: SV_POSITION; // �V�X�e���ɓn�����_�̈ʒu
	float2 uv			: IN_UV;
	float4 color		: IN_COLOR;
};

//! ���_�V�F�[�_
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


//! �s�N�Z���V�F�[�_
float4 RenderPS(OutputVS inPixel) : SV_TARGET
{
	return texDiffuse.Sample(samDiffuse, inPixel.uv);
	//return inPixel.color;
}