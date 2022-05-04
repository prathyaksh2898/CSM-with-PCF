#include "FBXLoader.hpp"

#include "Engine/3D/Model.hpp"
#include "Engine/3D/ModelNode.hpp"
#include "Engine/3D/VisualDatabase.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/VertexData/VertexUtils.hpp"


//-----------------------------------------------------------------------------------------------
FBXLoader* g_theFBXLoader = nullptr;


//-----------------------------------------------------------------------------------------------
FbxString GetAttributeTypeName( FbxNodeAttribute::EType type )
{
	switch ( type )
	{
		case FbxNodeAttribute::eUnknown:          return "Unidentified";
		case FbxNodeAttribute::eNull:             return "Null";
		case FbxNodeAttribute::eMarker:           return "Marker";
		case FbxNodeAttribute::eSkeleton:         return "Skeleton";
		case FbxNodeAttribute::eMesh:             return "Mesh";
		case FbxNodeAttribute::eNurbs:            return "Nurbs";
		case FbxNodeAttribute::ePatch:            return "Patch";
		case FbxNodeAttribute::eCamera:           return "Camera";
		case FbxNodeAttribute::eCameraStereo:     return "Stereo";
		case FbxNodeAttribute::eCameraSwitcher:   return "Camera Switcher";
		case FbxNodeAttribute::eLight:            return "Light";
		case FbxNodeAttribute::eOpticalReference: return "Optical reference";
		case FbxNodeAttribute::eOpticalMarker:    return "Marker";
		case FbxNodeAttribute::eNurbsCurve:       return "Nurbs Curve";
		case FbxNodeAttribute::eTrimNurbsSurface: return "Trim Nurbs Surface";
		case FbxNodeAttribute::eBoundary:         return "Boundary";
		case FbxNodeAttribute::eNurbsSurface:     return "Nurbs Surface";
		case FbxNodeAttribute::eShape:            return "Shape";
		case FbxNodeAttribute::eLODGroup:         return "LODGroup";
		case FbxNodeAttribute::eSubDiv:           return "Subdiv";
		default:                                  return "Unknown";
	}
}


//-----------------------------------------------------------------------------------------------
FBXLoader::FBXLoader( FBXConfig config ) : m_config( config )
{

}


//-----------------------------------------------------------------------------------------------
FBXLoader::~FBXLoader()
{

}


//-----------------------------------------------------------------------------------------------
void FBXLoader::Startup()
{
	m_manager = FbxManager::Create();
}


//-----------------------------------------------------------------------------------------------
void FBXLoader::Shutdown()
{
	if ( m_manager == nullptr )
		return;

	m_manager->Destroy();
}


//------------------------------------------------------------------------------------------------
Mat44 FBXLoader::ConvertFBXMatrixToMat44( FbxMatrix matrix )
{
	Mat44 mat;

	mat.m_values[ 0 ]  = static_cast< float >( matrix.mData[ 0 ].mData[ 0 ] );
	mat.m_values[ 1 ]  = static_cast< float >( matrix.mData[ 0 ].mData[ 1 ] );
	mat.m_values[ 2 ]  = static_cast< float >( matrix.mData[ 0 ].mData[ 2 ] );
	mat.m_values[ 3 ]  = static_cast< float >( matrix.mData[ 0 ].mData[ 3 ] );
					   
	mat.m_values[ 4 ]  = static_cast< float >( matrix.mData[ 1 ].mData[ 0 ] );
	mat.m_values[ 5 ]  = static_cast< float >( matrix.mData[ 1 ].mData[ 1 ] );
	mat.m_values[ 6 ]  = static_cast< float >( matrix.mData[ 1 ].mData[ 2 ] );
	mat.m_values[ 7 ]  = static_cast< float >( matrix.mData[ 1 ].mData[ 3 ] );

	mat.m_values[ 8 ]  = static_cast< float >( matrix.mData[ 2 ].mData[ 0 ] );
	mat.m_values[ 9 ]  = static_cast< float >( matrix.mData[ 2 ].mData[ 1 ] );
	mat.m_values[ 10 ] = static_cast< float >( matrix.mData[ 2 ].mData[ 2 ] );
	mat.m_values[ 11 ] = static_cast< float >( matrix.mData[ 2 ].mData[ 3 ] );

	mat.m_values[ 12 ] = static_cast< float >( matrix.mData[ 3 ].mData[ 0 ] );
	mat.m_values[ 13 ] = static_cast< float >( matrix.mData[ 3 ].mData[ 1 ] );
	mat.m_values[ 14 ] = static_cast< float >( matrix.mData[ 3 ].mData[ 2 ] );
	mat.m_values[ 15 ] = static_cast< float >( matrix.mData[ 3 ].mData[ 3 ] );

	return mat;
}


//------------------------------------------------------------------------------------------------
ModelNode* FBXLoader::LoadModelNodeFromFBXNode( FbxNode* fbxnode, char const* filepath )
{
	if ( fbxnode == nullptr )
	{
		ERROR_AND_DIE( "FBX Node is nullptr" );
	}

	ModelNode* node = nullptr;
	FbxNodeAttribute* attribute = nullptr;
	
	bool isMesh = false;
	for ( int attributeNum = 0; attributeNum < fbxnode->GetNodeAttributeCount(); ++attributeNum )
	{
		attribute = fbxnode->GetNodeAttributeByIndex( attributeNum );
		if ( attribute->GetAttributeType() == FbxNodeAttribute::eMesh )
		{
			GeometryNode* geoNode = new GeometryNode();
			geoNode->m_vertDataID = g_theVisualDatabase->CreateOrGetVertsFromFBX( fbxnode, filepath );
			geoNode->m_nodeType = NodeType::Geometry;
	
			/*int materialCount = fbxnode->GetMaterialCount();
			if ( materialCount > 0 )
			{
				FbxSurfaceMaterial* mat = fbxnode->GetMaterial( 0 );
				geoNode->m_material = g_theVisualDatabase->CreateOrGetMaterial( mat );
			}*/

			node = geoNode;
			isMesh = true;
			break;
		}
	}

	if ( node == nullptr )
	{
		node = new ModelNode();
	}

	FbxMatrix localToParent = fbxnode->EvaluateLocalTransform();
	node->m_localToParent = ConvertFBXMatrixToMat44( localToParent );
	node->m_nodeName = std::string( fbxnode->GetName() );

	for ( int fbxnodeNum = 0; fbxnodeNum < fbxnode->GetChildCount(); fbxnodeNum++ )
	{
		ModelNode* child = LoadModelNodeFromFBXNode( fbxnode->GetChild( fbxnodeNum ), filepath );
		child->m_parent = node;
		node->m_children.push_back( child );
	}

	return node;
}


//------------------------------------------------------------------------------------------------
Model* FBXLoader::LoadModelFromFBX( char const* filePath )
{
	ASSERT_OR_DIE( g_theVisualDatabase != nullptr, "Visual Database is nullptr" );

	FbxIOSettings* fbxIOS = FbxIOSettings::Create( m_manager, IOSROOT );
	m_manager->SetIOSettings( fbxIOS );
	FbxImporter* fbxImporter = FbxImporter::Create( m_manager, "" );

	if ( !fbxImporter->Initialize( filePath, -1, m_manager->GetIOSettings() ) )
	{
		ERROR_AND_DIE( Stringf( "Error returned: %s\n\n", fbxImporter->GetStatus().GetErrorString() ).c_str() );
	}

	FbxScene* fbxScene = FbxScene::Create( m_manager, "myScene" );
	fbxImporter->Import( fbxScene );
	fbxImporter->Destroy();

	FbxNode* root = fbxScene->GetRootNode();

	Model*   model = new Model( filePath );

	model->m_node = LoadModelNodeFromFBXNode( root, filePath );

	root->Destroy( true );
	fbxScene->Destroy( true );
	fbxIOS->Destroy( true );

	return model;
}


