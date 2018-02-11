#define MAX_BONE_MATRICES 1024

/* ������Ϣ */
cbuffer MatrixBuffer
{
	float4 camera_pos;
	matrix mvp_matrix;
};

/* �����任���� */
cbuffer AnimMatrices
{
	matrix const_bone_world[MAX_BONE_MATRICES];
};

struct VertexInput  
{  
	float4 pos : POSITION;  
	float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;	
	float4 weights : WEIGHTS;	// ��ͷȨ��
	uint4 bones : BONES;		// ��ͷ����
};  
 
struct PixelInput  
{  
	float4 pos : SV_POSITION;  
	float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;  
	float3 light_dir : TEXCOORD1;
	float3 view_dir : TEXCOORD2;
};  

struct SkinnedInfo
{
	float4 pos;
	float3 normal;
};

// ��ȡ�����Ӧ�ı任����
matrix FetchBoneTransform(uint b)
{
	matrix mret;
	if( b == 65535 )
	{
		mret = const_bone_world[0];
	}
	else
	{
		mret = const_bone_world[b];
	}
	return mret;
}

// ��������任
SkinnedInfo SkinVert(VertexInput i)
{
	SkinnedInfo o = (SkinnedInfo)0;
	
	// Bone0
	uint bone = i.bones.x;
	float weight = i.weights.x;
	matrix m = FetchBoneTransform(bone);
	o.pos += weight * mul(i.pos, m);
	o.normal += weight * mul(i.normal, (float3x3)m);
	
	// Bone1
	bone = i.bones.y;
	weight = i.weights.y;
	m = FetchBoneTransform(bone);
	o.pos += weight * mul(i.pos, m);
	o.normal += weight * mul(i.normal, (float3x3)m);
	
	// Bone2
	bone = i.bones.z;
	weight = i.weights.z;
	m = FetchBoneTransform(bone);
	o.pos += weight * mul(i.pos, m);
	o.normal += weight * mul(i.normal, (float3x3)m);
	
	// Bone3
	bone = i.bones.w;
	weight = i.weights.w;
	m = FetchBoneTransform(bone);
	o.pos += weight * mul(i.pos, m);
	o.normal += weight * mul(i.normal, (float3x3)m);
	
	return o;
}

PixelInput main(VertexInput i)
{
	PixelInput o;
	
	SkinnedInfo skinned = SkinVert(i);

	o.pos = mul(skinned.pos, mvp_matrix);
	o.tex0 = i.tex0;
	
	o.normal = skinned.normal;
	o.light_dir = normalize(float3(0, 17, -5) - i.pos.xyz);
	o.view_dir = camera_pos - i.pos.xyz;

	return o;
}