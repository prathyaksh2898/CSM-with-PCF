#include "BlinnPhongFunctionLib.hlsl"


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
struct v2p_t 
{
	float4 position                                             : SV_Position; 
	float4 color                                                : VertexColor;    
    float2 uv                                                   : TexCoord;
	float4 worldPosition                                        : WorldPosition;
	float3 worldNormal                                          : WorldNormal;
	float3 worldTangent                                         : WorldTangent;
    float4 clipPosCam1                                          : ClipPosition;
};


//Project v onto u
//------------------------------------------------------------------------------------------------
float3 Projection(float3 vecU, float3 vecV)
{
    return dot( vecU, vecV ) * vecU / dot( vecU, vecU );
}


//------------------------------------------------------------------------------------------------
v2p_t VertexMain( vs_input_t input )
{
	v2p_t v2p;

	float4 localPosition = float4( input.position, 1 );
	float4 worldPosition = mul(modelMatrix, localPosition);
    float4 viewPosition  = mul(viewMatrix, worldPosition);
	float4 clipPosition  = mul(projectionMatrix, viewPosition);

	float4 localNormal   = float4( normalize( input.normal ), 0 );
	float4 worldNormal   = mul( modelMatrix, localNormal );
						 
	float4 localTangent  = float4( normalize( input.tangent ), 0 );
	float4 worldTangent  = mul( modelMatrix, localTangent );
	
    float4 cam1ViewPos = mul( viewMatrixCam1, worldPosition );
    float4 cam1ClipPos = mul( projectionMatrixCam1, cam1ViewPos );
    
	v2p.position      = clipPosition; // we want to output the clip position to raster (a perspective point)
    v2p.color         = input.color * tint;
    v2p.uv            = input.uv;;
	v2p.worldPosition = worldPosition;
    v2p.worldNormal   = worldNormal.xyz;
    v2p.worldTangent  = worldTangent.xyz;
    v2p.clipPosCam1   = cam1ClipPos;
	
	return v2p;
}


//------------------------------------------------------------------------------------------------
Texture2D<float4> SurfaceColorTexture : register( t0 );
Texture2D<float4> NormalColorTexture : register( t1 );

SamplerState SurfaceSampler : register( s0 );


//------------------------------------------------------------------------------------------------
float4 PixelMain( v2p_t input ) : SV_Target0
{
	float lightingFactor       = 1.0f;

    float3 clipPos             = input.clipPosCam1.xyz;
	
	float2   texCoord          = input.uv;
	float4   diffuseColor      = SurfaceColorTexture.Sample( SurfaceSampler, texCoord );
	float4   normalColor       = NormalColorTexture.Sample( SurfaceSampler, texCoord );
	float3   surfaceColor      = diffuseColor.xyz * input.color.xyz;
	float    alpha             = diffuseColor.w * input.color.w;
						       
	float3   normal            = normalize( input.worldNormal );
	float3   tangent           = normalize( input.worldTangent );
	
    tangent = normalize( tangent - Projection( normal, tangent ) );
	
	float3   bitangent         = normalize( cross( normal, tangent ) );
	float3x3 TBN               = float3x3( tangent, bitangent, normal );
	float3   surfaceNormal     = ColorToFloat3( normalColor.xyz );
	float3   worldNormal       = mul( surfaceNormal, TBN );
	
    //float4 lightProjectedPositions[ MAX_CASCADES * MAXLIGHTS ];
    //float4 lightSpacePos;
    //for ( uint lightNum = 0; lightNum < MAXLIGHTS; lightNum++ )
    //{
    //    lightSpacePos = mul( lights[ lightNum ].viewMatrix, input.worldPosition );
        
    //    for ( uint cascadeNum = 0; cascadeNum < NUM_CASCADES; cascadeNum++ )
    //    {
    //        lightSpacePos = mul( lights[ lightNum ].projMatrix[ cascadeNum ], lightSpacePos );
    //        lightProjectedPositions[ ( lightNum * MAX_CASCADES ) + cascadeNum ] = lightSpacePos;
    //    }
    //}
	
    float3 finalColor = ComputeLighting( input.worldPosition.xyz, worldNormal, surfaceColor, float3( 0.0f.xxx ), specularFactor, clipPos, input.worldPosition );
	return float4( finalColor, alpha );
}

