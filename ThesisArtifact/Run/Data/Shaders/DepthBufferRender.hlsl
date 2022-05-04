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
cbuffer CameraConstants : register( b2 )
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4 cameraPosition;
};




//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register( b3 )
{
    float4x4 modelMatrix;
    float4 tint;
};


//------------------------------------------------------------------------------------------------
cbuffer DebugCascadePrint : register( b4 )
{
    float  arraySlice;
    float3 padding;
};


//------------------------------------------------------------------------------------------------
v2p_t VertexMain( vs_input_t input )
{
    v2p_t v2f;

    float4 localPosition = float4( input.position, 1 );
    float4 worldPosition = mul( modelMatrix, localPosition );
    float4 viewPosition = mul( viewMatrix, worldPosition );
    float4 clipPosition = mul( projectionMatrix, viewPosition );

    v2f.position = clipPosition;
    v2f.color = input.color * tint;
    v2f.uv = input.uv;

    return v2f;
}


//------------------------------------------------------------------------------------------------
Texture2DArray<float4> SurfaceColorTexture : register( t0 );
SamplerState SurfaceSampler : register( s0 );


//------------------------------------------------------------------------------------------------
float4 PixelMain( v2p_t input ) : SV_Target0
{
    float2 texCoord = input.uv;
    float4 surfaceColor = SurfaceColorTexture.Sample( SurfaceSampler, float3( texCoord, arraySlice ) ); 
        
    float4 tint = input.color;
    float4 finalColor = tint * surfaceColor;
    return finalColor;
    float3 invertedColor = float3( 1.0f, 1.0f, 1.0f ) - float3( finalColor.xyz );
    return float4( invertedColor, finalColor.a );
}
