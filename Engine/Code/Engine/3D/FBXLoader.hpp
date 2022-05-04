#pragma once
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"

#include "fbxsdk/include/fbxsdk.h"

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class Model;
class ModelNode;


//-----------------------------------------------------------------------------------------------
struct FBXConfig
{

};


//-----------------------------------------------------------------------------------------------
class FBXLoader
{
public:
	FBXLoader( FBXConfig config );
	~FBXLoader();
	void Startup();
	void Shutdown();

	ModelNode* LoadModelNodeFromFBXNode( FbxNode* fbxnode, char const* filepath );
	Model*     LoadModelFromFBX( char const* filePath );

	static Mat44      ConvertFBXMatrixToMat44( FbxMatrix matrix );

public:
	FBXConfig   m_config;
	FbxManager* m_manager  = nullptr;
};



//------------------------------------------------------------------------------------------------
FbxString GetAttributeTypeName( FbxNodeAttribute::EType type );