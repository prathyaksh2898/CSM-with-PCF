//------------------------------------------------------------------------------------------------
static const uint MAXLIGHTS        = 8;
static const uint NUM_SAMPLES      = 32;
static const uint NUM_CASCADES     = 8;
static const uint INVALIDLIGHT     = 0;
static const uint DIRECTIONALLIGHT = 1;
static const uint SPOTLIGHT        = 2;
static const uint POINTLIGHT       = 3;

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4   cameraPosition;
};


//------------------------------------------------------------------------------------------------
cbuffer ObjectConstants : register(b3)
{
	float4x4 modelMatrix;
    float4   tint;
};


//------------------------------------------------------------------------------------------------
struct lightDataG // Light Data for GPU
{
	float3          worldPosition;
	uint            lightType;                               

	float3          color;
	float           intensity;                               

	float3          direction;                               
	float           directionFactor;                                 

	float3          attenuation;                             
	float           dotInnerAngle;                           

	float3          specularAttenuation;                     
	float           dotOuterAngle;                    
	
    float4x4        viewMatrix;
    float4x4        projMatrix[ NUM_CASCADES ];
	
    uint            isShadowCasting;
    float           shadowBias;

    float           padding0;
    float           padding1;
};


//------------------------------------------------------------------------------------------------
cbuffer LightConstantsG : register( b4 )
{
	float4		ambientLight;						
	lightDataG	lights[ MAXLIGHTS ];
	
	float		diffuseFactor;					
	float		specularFactor;				
	float		specularPower;				
	float		padding00;
};


//------------------------------------------------------------------------------------------------
struct DepthForCascade
{
    float  depthVal;
    float3 padding;
};

//------------------------------------------------------------------------------------------------
cbuffer CascadeConstantsG : register( b5 )
{
    DepthForCascade depthForCascades[ NUM_CASCADES ];
	float           dimensionOfDepthTexture;
	uint            numSamples;
    uint            numCascades;
	uint            enablePCF ;
};


//------------------------------------------------------------------------------------------------
cbuffer CameraConstantsForCam1 : register( b6 )
{
    float4x4 viewMatrixCam1;
    float4x4 projectionMatrixCam1;
};