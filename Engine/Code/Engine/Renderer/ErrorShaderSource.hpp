static char const* g_errorShaderSource = R"(
struct vs_input_t
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};



struct v2p_t 
{
	float4 position : SV_Position; 
	float4 color : VertexColor;    
    float2 uv : TexCoord;
};


//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};


//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b3)
{
	float4x4 modelMatrix;
    float4   tint;
};


v2p_t VertexMain( vs_input_t input )
{
	v2p_t v2f;

	float4 localPosition  = float4( input.position, 1 ) ;
	float4 worldPosition  = mul(modelMatrix, localPosition);
    float4 viewPosition   = mul(viewMatrix, worldPosition);
	float4 clipPosition   = mul(projectionMatrix, viewPosition);

	v2f.position = clipPosition;
	v2f.color = input.color * tint;
    v2f.uv = input.uv;

	return v2f; 
}


Texture2D<float4> SurfaceColorTexture : register(t0);
SamplerState SurfaceSampler : register(s0);


float4 PixelMain( v2p_t input ) : SV_Target0
{
	float2 texCoord = input.uv;
	float4 surfaceColor = SurfaceColorTexture.Sample( SurfaceSampler, texCoord );

	float4 tint = input.color;
	float4 finalColor = tint * surfaceColor;

	return float4( 1.0f, 0.0f, 1.0f, 1.0f );
}

)";