#include "BlinnPhongConstantBuffers.hlsl"

//------------------------------------------------------------------------------------------------
static const float2 poissonDisk[ 16 ] =
                             {
                               float2( -0.94201624,  -0.39906216 ),
                               float2(  0.94558609,  -0.76890725 ),
                               float2( -0.094184101, -0.92938870 ),
                               float2(  0.34495938,   0.29387760 ),
                               float2( -0.91588581,   0.45771432 ),
                               float2( -0.81544232,  -0.87912464 ),
                               float2( -0.38277543,   0.27676845 ),
                               float2(  0.97484398,   0.75648379 ),
                               float2(  0.44323325,  -0.97511554 ),
                               float2(  0.53742981,  -0.47373420 ),
                               float2( -0.26496911,  -0.41893023 ),
                               float2(  0.79197514,   0.19090188 ),
                               float2( -0.24188840,   0.99706507 ),
                               float2( -0.81409955,   0.91437590 ),
                               float2(  0.19984126,   0.78641367 ),
                               float2(  0.14383161,  -0.14100790 )
                            };


//------------------------------------------------------------------------------------------------
Texture2DArray<float4> DepthTexture[ MAXLIGHTS ] : register( t8 );

SamplerComparisonState DepthSampler : register( s1 );


//------------------------------------------------------------------------------------------------
float random( float2 p )
{
    float2 K1 = float2(
        23.14069263277926, // e^pi (Gelfond's constant)
        2.665144142690225 // 2^sqrt(2) (Gelfond–Schneider constant)
    );
    return frac( cos( dot( p, K1 ) ) * 12345.6789 );
}


//------------------------------------------------------------------------------------------------
float random( float4 p )
{
    float dot_product = dot( p, float4( 12.9898, 78.233, 45.164, 94.673 ) );
    return frac( sin( dot_product ) * 43758.5453 );
}


//------------------------------------------------------------------------------------------------
float BiasMultiplier( float theta )
{
    float tanTheta = abs( tan( theta ) );
    tanTheta = clamp( 0.0f, 50.0f, tanTheta );

    float t = ( tanTheta ) / ( 50 );
    
    if ( tanTheta > 1.0f)
        return lerp( 2.f, 25.0f, t );
    
    else
        return 1.0f;
}


//------------------------------------------------------------------------------------------------
float CalculateDepthValue( float2 texCoord, uint lightNum, float pixelDepth, float cascadeNum = 0.0f )
{
    float val = 1.0f;
    float3 shadowTexCoords = float3( texCoord, cascadeNum );
    
    switch ( lightNum )
    {
        case 0:
            val = DepthTexture[ 0 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 1:
            val = DepthTexture[ 1 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 2:
            val = DepthTexture[ 2 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 3:
            val = DepthTexture[ 3 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 4:
            val = DepthTexture[ 4 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 5:
            val = DepthTexture[ 5 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 6:
            val = DepthTexture[ 6 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
        case 7:
            val = DepthTexture[ 7 ].SampleCmpLevelZero( DepthSampler, shadowTexCoords, pixelDepth );
            break;
    }
    
    return val;
}


//------------------------------------------------------------------------------------------------
float CalculateShadowFactor( float4 lightSpacePos, uint lightNum, float bias, float cascadeNum = 0.0f )
{
    float  visibility = 0.0f;
    float2 shadowUVTexCoords;
    float  pixelDepth;
    float  shadowDepthValue;
    float  factor;
    
    if ( lights[ lightNum ].intensity <= 0.0f ) 
        return visibility;
           
    shadowUVTexCoords.x = 0.5f + ( lightSpacePos.x / lightSpacePos.w * 0.5f );
    shadowUVTexCoords.y = 0.5f - ( lightSpacePos.y / lightSpacePos.w * 0.5f );
        
    pixelDepth = ( lightSpacePos.z / lightSpacePos.w ) - bias;
    
    float2 sampleCoords;

    if ( enablePCF == 0 )
    {
        sampleCoords = shadowUVTexCoords;
        if ( ( saturate( sampleCoords.x ) == sampleCoords.x ) &&
		        ( saturate( sampleCoords.y ) == sampleCoords.y ) )
        {
            visibility = CalculateDepthValue( sampleCoords, lightNum, pixelDepth, cascadeNum );
        }
    }
    else
    {
        uint numComparisons = 0;
    
        for ( uint sampleNum = 0; sampleNum < numSamples; sampleNum++ )
        {        
            float2 randomRotation = float2( 1.0f, 1.0f );
            uint index = uint( 16.0f * random( float4( shadowUVTexCoords.xyy, sampleNum ) ) ) % 16;
            sampleCoords = shadowUVTexCoords + ( poissonDisk[ index ] * randomRotation / ( 0.85f * dimensionOfDepthTexture ) );
       
            if ( ( saturate( sampleCoords.x ) == sampleCoords.x ) &&
		        ( saturate( sampleCoords.y ) == sampleCoords.y ) )
            {
                float depthValue = CalculateDepthValue( sampleCoords, lightNum, pixelDepth, cascadeNum );
                visibility += depthValue;
                numComparisons++;
            }
            else if (lights[lightNum].lightType == SPOTLIGHT )
            {
                numComparisons++;
            }
        }   
        visibility /= numComparisons;
    }

    return visibility;
}


//------------------------------------------------------------------------------------------------
float3 ColorToFloat3( float3 color )
{
    return normalize( color * float3( 2.0f, 2.0f, 1.0f ) - float3( 1.0f, 1.0f, 0.0f ) );
}


//------------------------------------------------------------------------------------------------
float2 ComputeLightFactor( lightDataG light, float3 worldPosition, float3 worldNormal, float3 directionToCam )
{
    float3 vectorToLight = light.worldPosition - worldPosition;
    float distanceToLight = length( vectorToLight );
    float3 directionToLight = normalize( vectorToLight );
    
    float3 lightDirection = lerp( -directionToLight, light.direction, light.directionFactor );
    float3 attenuationVector = float3( 1.0f, distanceToLight, distanceToLight * distanceToLight );

    float dotAngle = dot( -directionToLight, light.direction );
    float coneAttenuation = smoothstep( light.dotOuterAngle, light.dotInnerAngle, dotAngle );

    float diffuseAttenuation = light.intensity;
    float specularAttenuation = light.intensity;

    if ( light.lightType != DIRECTIONALLIGHT )
    {
        diffuseAttenuation = light.intensity / ( dot( attenuationVector, light.attenuation ) ) * coneAttenuation;
        specularAttenuation = light.intensity / dot( attenuationVector, light.specularAttenuation ) * coneAttenuation;
    }

    float dotIncident = dot( -lightDirection, worldNormal );
    float facingDirection = smoothstep( -0.4f, 0.1f, dotIncident );
    float diffuse = max( 0.0f, dotIncident );

    float3 hv = normalize( -lightDirection + directionToCam );
    float specular = max( 0.0f, dot( normalize( worldNormal ), hv ) );

    specular = facingDirection * specular;
    specular = specularFactor * pow( specular, specularPower );

    return float2( diffuseAttenuation * diffuse, specularAttenuation * specular );
}


//------------------------------------------------------------------------------------------------
float3 ComputeLighting( float3 worldPos, float3 worldNormal, float3 surfaceColor, float3 emmisiveColor, float specularFactor, float3 clipPos, float4 worldPosition )
{
    float3 directionToCam = normalize( cameraPosition.xyz - worldPos );
    float3 diffuse = ambientLight.xyz * ambientLight.w; // assumes ambient is set from a user - so sRGB space
    float3 specular = float3( 0.0f.xxx );
    
   // add up contribution of all lights
    for ( uint index = 0; index < MAXLIGHTS; index++ )
    {
        if ( lights[index].lightType == INVALIDLIGHT)
            continue;
        
        float3 lightColor = lights[ index ].color.xyz;
        float2 lightFactors = ComputeLightFactor( lights[ index ], worldPos, worldNormal, directionToCam );
        
        float visibility = 1.0f;

        if ( lights[index].isShadowCasting == 1)
        {
            float cosTheta = clamp( dot( worldNormal, -lights[ index ].direction ), 0, 1 );
            float theta = acos( cosTheta );
            //float biasMultiplier = lerp( 1.0f, 100.0f, ( 1 - SmoothStop4( tanTheta ) ) );
            float bias = BiasMultiplier( theta ) * lights[ index ].shadowBias;
            //float bias = lerp( lights[ index ].shadowBias, lights[ index ].shadowBias * 3.0f, tanTheta );
            //float bias = lights[ index ].shadowBias * tanTheta;
            //bias = clamp( bias, lights[ index ].shadowBias, lights[ index ].shadowBias * 10.0f );
            //float bias = lights[ index ].shadowBias;
            
            float4 lightViewSpacePos = mul( lights[ index ].viewMatrix, worldPosition );
            float4 lightProjSpacePos;
            
            if ( lights[ index ].lightType == DIRECTIONALLIGHT )
            {
                int cascade = -1;
                for ( uint cascadeNum = 0; cascadeNum < numCascades; cascadeNum++ )
                {
                    if ( clipPos.z <= depthForCascades[ cascadeNum ].depthVal )
                    {
                        cascade = cascadeNum;
                        break;
                    }
                }
        
                if ( cascade != -1 )
                {
                    lightProjSpacePos = mul( lights[ index ].projMatrix[ cascade ], lightViewSpacePos );
                    visibility *= CalculateShadowFactor( lightProjSpacePos, index, bias, ( float ) cascade );
                }
            }
            else if ( lights[ index ].lightType == SPOTLIGHT )
            {
                bias = lights[ index ].shadowBias;
                lightProjSpacePos = mul( lights[ index ].projMatrix[ 0 ], lightViewSpacePos );
                visibility *= CalculateShadowFactor( lightProjSpacePos, index, bias, 0 );
            }
        }
                
        if ( visibility > 0.0f )
        {
            diffuse += lightFactors.x * lightColor * visibility;
            if ( lights[index].lightType != DIRECTIONALLIGHT )
            {
                specular += lightFactors.y * lightColor * visibility;
            }
        }
    }

    diffuse = min( diffuseFactor * diffuse, float3( 1.f.xxx ) );
    specular *= specularFactor; // scale back specular based on spec factor

    float3 specularEmissive = specular + emmisiveColor;
    float3 resultDiffuse = diffuse * surfaceColor + specularEmissive;

    return resultDiffuse + specularEmissive;
}
