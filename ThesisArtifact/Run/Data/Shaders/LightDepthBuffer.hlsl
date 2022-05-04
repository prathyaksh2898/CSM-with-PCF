static const uint NUM_CASCADES = 3;

//------------------------------------------------------------------------------------------------
struct vs_input_t
{
	float3 position : POSITION;
	float4 color    : COLOR;
	float2 uv       : TEXCOORD;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal   : NORMAL;
};


//------------------------------------------------------------------------------------------------
struct v2f_t
{
    float4 position : SV_Position;
    float4 color    : COLOR;
    float2 uv       : TEXCOORD;
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

	float4 localPosition = float4( input.position, 1 );
	float4 worldPosition = mul( modelMatrix, localPosition );
	float4 viewPosition = mul( viewMatrix, worldPosition );
    float4 projPosition = mul( projectionMatrix, viewPosition );

    v2f.position = projPosition;
    v2f.color = input.color * tint;
    v2f.uv = input.uv;

	return v2f;
}

