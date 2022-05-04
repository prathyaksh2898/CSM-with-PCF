// a struct containing what inputs our vertex shader is expecting
struct vs_input_t
{
	// variables being passes between stages have the following declartion style
	// [qualifier] type localName [: semanticName]; 
	uint vertexID : SV_VertexID;
	
	// localName -> what the shader will reference it as
	// semanticName -> how D3D links up variables between different stages (including initial GPU to CPU)
	// 				   Semenatics with `SV_` are special, and stand for `System Variable`, and have special meaning/function within the pipeline
	// 				   SV_VertexID will cause this variable to be filled with the index of the vertex we're drawing when this is used as an input to a vertex shader; 
};

// struct of data that I'm passing from the vertex stage for the fragment stage.  `v2f_t` is the name used in Unity for this, 
// but you can call it whatever you want, such as `VertexToPixel` or `VertexToFragment` if you prefer a different naming scheme
struct v2p_t // vertex to fragment
{
	float4 position : SV_Position; // SV_Position is always requried for a vertex shader, and denotes the renering location of this vertex.  As an input to the pixel shader, it gives the pixel location of the fragment
	float3 color : VertexColor;    // This semantic is NOT `SV_`, and is just whatever name we want to call it.  If a pixel stage has an input marked as `VertexColor`, it will link the two up.
};

// static globals - these are baked into the shader and can not be changed
// we'll be using this to pick out triangles positions
static float3 TRIANGLE_CORNERS[3] = 

{
	float3( -0.50f, -0.50f, 0.0f ),
	float3(  0.00f,  0.50f, 0.0f ),
	float3(  0.50f, -0.50f, 0.0f ),
};


cbuffer CameraConstants: register(b2)
{
	float OrthoMinX;
	float OrthoMinY;
	float OrthoMinZ;
	float OrthoMaxX;
	float OrthoMaxY;
	float OrthoMaxZ;
};


float3 RangeMapToClipSpace(float3 toBeRangeMapped)
{
	float3 rangeMappedValues;
	
	rangeMappedValues.x = ((toBeRangeMapped.x - OrthoMinX) / (OrthoMaxX - OrthoMinX)) * 2 - 1;
	rangeMappedValues.y = ((toBeRangeMapped.y - OrthoMinY) / (OrthoMaxY - OrthoMinY)) * 2 - 1;
	rangeMappedValues.z = toBeRangeMapped.z;

	return rangeMappedValues;
}

//------------------------------------------------------------------------------------------------
// Main Entry Point for the vertex stage
// which for graphical shaders is usually a main entry point
// and will get information from the game
v2p_t VertexMain( vs_input_t input )
{
	v2p_t v2f;

	float3 localPos = TRIANGLE_CORNERS[input.vertexID];

	float3 clipPos = RangeMapToClipSpace(localPos);
	[]
	// we defined the position as a 3 dimensional coordinate, but SV_Position expects a clip/perspective space coordinate (4D).  More on this later.  For now, just pass 1 for w; 
	v2f.position = float4( clipPos, 1 );
	v2f.color = float3(0.2f, 0.2f, 0.8f); 

	return v2f; // pass it on to the raster stage
}



//------------------------------------------------------------------------------------------------
// Main Entry Point for the Pixel Stage
// This returns only one value (the output color)
// so instead of using a struct, we'll just label the return value
//
// This determines the color of a single pixel or fragment in the output
// the input may vary certain variables in the raster stage, and we'll get those varied 
// inputs passed to us. 

// Note, system variables such as `SV_Position` have special rules, and the one output may
// have no or very little relation to the one you got into the pixel shader; 
float4 PixelMain( v2p_t input ) : SV_Target0 
{
	// output is RGBA, and input is RGB, so again.  (for now, this will have no visible change no matter what value you pass for alpha)
	return float4( input.color, 1 ); 
}

