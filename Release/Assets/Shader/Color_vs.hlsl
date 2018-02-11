cbuffer MatrixBuffer
{ 
	float4 camera_pos;
    matrix mvp_matrix;
};

struct VertexInput  
{  
	float4 pos : POSITION;  
	float4 color : COLOR;  
};  
  
struct PixelInput  
{  
	float4 pos : SV_POSITION;  
	float4 color : COLOR;  
};  

PixelInput main(VertexInput i)
{
	PixelInput o;

	o.pos = i.pos;
	o.pos = mul(o.pos, mvp_matrix);
	o.color = i.color;

	return o;
}

