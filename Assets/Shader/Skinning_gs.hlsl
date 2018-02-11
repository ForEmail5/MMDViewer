struct PixelInput  
{  
	float4 pos : SV_POSITION;  
	float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;  
	float3 light_dir : TEXCOORD1;
	float3 view_dir : TEXCOORD2;
};  

struct PixelOutput 
{  
	float4 pos : SV_POSITION;  
	float3 normal : NORMAL;
	float2 tex0 : TEXCOORD0;  
	float3 light_dir : TEXCOORD1;
	float3 view_dir : TEXCOORD2;
	uint idx : SV_VIEWPORTARRAYINDEX;
};  
   
[maxvertexcount(12)]  
void main(triangle PixelInput input[3], inout TriangleStream<PixelOutput> output)  
{  
	for(int j = 0; j < 4; j++)
	{
		PixelOutput element;  
		element.idx = j;

	    for (uint i = 0; i < 3;i++)  
		{  
		   element.pos = input[i].pos * (j + 1) * 0.1;  
		   element.normal = input[i].normal;  
		   element.tex0 = input[i].tex0;  
		   element.light_dir = input[i].light_dir;  
		   element.view_dir = input[i].view_dir;  
		   output.Append(element);  
		}  
		output.RestartStrip();
	}
}