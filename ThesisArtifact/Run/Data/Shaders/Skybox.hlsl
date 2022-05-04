static const uint NUM_CASCADES = 3;

//------------------------------------------------------------------------------------------------
struct vs_input_t
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 uv       : TEXCOORD;
};


//------------------------------------------------------------------------------------------------
struct v2f_t
{
    float4 position : SV_Position;
    float3 worlPos  : Position;
};


//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register( b2 )
{
	float4x4 viewMatrix;
    float4x4 projectionMatrix;
	float4   cameraPosition;
};


//------------------------------------------------------------------------------------------------
cbuffer ObjectConstants : register( b3 )
{
	float4x4 modelMatrix;
	float4   tint;
};


//------------------------------------------------------------------------------------------------
v2f_t VertexMain( vs_input_t input )
{
    v2f_t v2f;

    float4 position = float4( input.position, 0.0f );
	float4 viewPosition = mul( viewMatrix, position );
    float4 projPosition = mul( projectionMatrix, viewPosition );

    v2f.position = projPosition;
    v2f.position.z = v2f.position.w;

    v2f.worlPos = input.position;

	return v2f;
}


//------------------------------------------------------------------------------------------------
TextureCube SurfaceColorTexture : register( t0 );
SamplerState SurfaceSampler : register( s0 );


//------------------------------------------------------------------------------------------------
float4 PixelMain( v2f_t input ) : SV_Target0
{
    return SurfaceColorTexture.Sample( SurfaceSampler, input.worlPos );
}