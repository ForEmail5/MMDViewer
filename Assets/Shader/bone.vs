#define MAX_BONE_MATRICES 255

cbuffer MatrixBuffer
{ 
	float4 camera_pos;
    matrix mvp_matrix;
};

cbuffer AnimMatrices
{
	matrix const_bone_world[MAX_BONE_MATRICES];
};

struct VertexInput  
{  
	float4 pos : POSITION;  
	float4 color : COLOR;
	float4 weights : WEIGHTS;	// 骨头权重
	uint4 bones : BONES;		// 骨头索引
};  
 
struct PixelInput  
{  
	float4 pos : SV_POSITION;  
	float4 color : COLOR;
};  

struct SkinnedInfo
{
	float4 pos;
	float3 normal;
};

// 获取顶点对应的变换矩阵
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

// 骨骼顶点变换
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

	o.pos = mul(i.pos, mvp_matrix);
	o.color = i.color;

	return o;
}