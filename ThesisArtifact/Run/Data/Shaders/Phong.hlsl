struct vs_input_t
{
	float3 position : POSITION;
	float4 color : TINT;
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
	float4x4 cameraOrtho;
};


//------------------------------------------------------------------------------------------------
float4 MultiplyWithOrthoMatrix( float4 position )
{
	return mul( cameraOrtho, position );
}


v2p_t VertexMain( vs_input_t input )
{
	v2p_t v2f;

	v2f.position = MultiplyWithOrthoMatrix( float4(input.position, 1) );
	v2f.color = input.color;
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

	return float4(finalColor);
}

