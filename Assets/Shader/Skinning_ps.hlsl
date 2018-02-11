#define PI 3.1415926

SamplerState sample;	// 采样器

Texture2D main_tex;		// 主纹理
Texture2D sub_tex;		// 辅助纹理
Texture2D toon_tex;

// 外部颜色设置
cbuffer CustomColor
{
	float4 diffuse;		// 漫反射颜色
	float4 embient;		// 环境反射颜色
	float shininess;	// 反光度
	float edge_size;
	int use_sub_tex;
	int has_toon;
};

// 输出顶点结构
struct PixelInput  
{  
	float4 pos : SV_POSITION;  
	float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;  
	float3 light_dir : TEXCOORD1;
	float3 view_dir : TEXCOORD2;
};  

// 正态分布函数，通过粗糙度（roughness）用统计学估算中间向量（H）与微平面平均取向的概率
float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a2 	 = roughness * roughness;
	float NdotH  = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return a2 / denom;
}

float4 main(PixelInput i) : SV_TARGET
{
	float3 L = normalize(i.light_dir);
	float3 V = normalize(i.view_dir);
	float3 N = normalize(i.normal);
	float3 H = normalize(L + V);

	float4 spec_tex = float4(1, 1, 1, 1);
	float4 obj_color = main_tex.Sample(sample, i.tex0);

	return obj_color;

	float edge = max(dot(N, V), 0);
	if( has_toon != 0 && edge < edge_size )
	{
		obj_color *= toon_tex.Sample(sample, float2(0, edge / edge_size));
	}
	
	if( use_sub_tex != 0)
	{
		obj_color += obj_color * pow(max(dot(N, V), 0.0), 2) * sub_tex.Sample(sample, i.tex0);
	}

	float roughtness = 0.85;		// 粗糙度
	float diff = DistributionGGX(N, H, roughtness);

	// 环境光 + 漫反射光
	float4 result = (embient + diffuse * diff) * obj_color;

	// 镜面高光
	if(shininess != 0)
	{
		result += obj_color * pow(max(dot(N, V), 0.0), shininess);
	} 
	return result;
}