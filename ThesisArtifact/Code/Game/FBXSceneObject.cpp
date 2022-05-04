#include "FBXSceneObject.hpp"

#include "Game/Game.hpp"

#include "Engine/3D/Material.hpp"
#include "Engine/3D/Model.hpp"
#include "Engine/3D/ModelNode.hpp"
#include "Engine/3D/FBXLoader.hpp"
#include "Engine/3D/VisualDatabase.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "fbxsdk/include/fbxsdk.h"


//------------------------------------------------------------------------------------------------
FBXSceneObject::FBXSceneObject( Game* Owner, Vec3 const& startPosition, EulerAngles const& orientation, std::string objectPath ) :
	Object( Owner, startPosition, orientation )
{
	m_fbxModel = g_theVisualDatabase->CreateOrGetModelFromFBX( objectPath.c_str() );

}


//------------------------------------------------------------------------------------------------
FBXSceneObject::~FBXSceneObject()
{

}


//------------------------------------------------------------------------------------------------
void FBXSceneObject::Update( float deltaseconds )
{
	UNUSED( deltaseconds );
	m_modelMatrix = Mat44();
	m_modelMatrix.AppendTranslation3D( m_position );
	m_modelMatrix.AppendZRotation( m_orientation.m_yawDegrees );
	m_modelMatrix.AppendYRotation( m_orientation.m_pitchDegrees );
	m_modelMatrix.AppendXRotation( m_orientation.m_rollDegrees );

	UpdateTraverse( m_fbxModel->m_node );
}


//------------------------------------------------------------------------------------------------
void FBXSceneObject::Render() const
{
	g_theRenderer->BindTexture( m_texture );
	g_theRenderer->BindTexture( m_normalTexture, 1 );
	RenderTraverse( m_fbxModel->m_node );
}


//------------------------------------------------------------------------------------------------
void FBXSceneObject::DebugRender() const
{

}


//------------------------------------------------------------------------------------------------
void FBXSceneObject::UpdateTraverse( ModelNode* node )
{
	if ( node->GetParent() )
	{
		Mat44 mat( node->GetParent()->GetLocalToWorldTransform() );
		mat.Append( node->GetLocalToParentTransform() );
		node->SetLocalToWorldTransform( mat );
	}
	else
	{
		Mat44 toWorldMatrix( m_modelMatrix );
		toWorldMatrix.Append( node->GetLocalToParentTransform() );
		node->SetLocalToWorldTransform( toWorldMatrix );
	}

	for ( ModelNode* childNode : node->GetChildren() )
	{
		UpdateTraverse( childNode );
	}
}


//------------------------------------------------------------------------------------------------
void FBXSceneObject::RenderTraverse( ModelNode* node ) const
{
	if ( node->m_nodeType == NodeType::Geometry )
	{
		GeometryNode* geoNode = node->AsGeometryNode();
		//Material* material = geoNode->GetMaterial();
		//material->SetRendererState();

		m_game->AddVertsRendered( static_cast< uint32_t >( geoNode->GetVertexCount() ) );

		ModelTransformationData data;
		data.modelMatrix = geoNode->GetLocalToWorldTransform();
		g_theRenderer->SetModelBuffer( data );
		g_theRenderer->DrawVertexBuffer( geoNode->GetVertexBuffer(), geoNode->GetVertexCount() );
	}

	for ( ModelNode* childNode : node->GetChildren() )
	{
		RenderTraverse( childNode );
	}
}
