#include "ModelNode.hpp"

#include "Engine/3D/VisualDatabase.hpp"
#include "Engine/Renderer/Camera.hpp"


//------------------------------------------------------------------------------------------------
ModelNode::ModelNode()
{

}


//------------------------------------------------------------------------------------------------
ModelNode::~ModelNode()
{

}


//------------------------------------------------------------------------------------------------
GeometryNode* ModelNode::AsGeometryNode()
{
	GeometryNode* geoNode = dynamic_cast< GeometryNode* >( this );
	return geoNode;
}


//------------------------------------------------------------------------------------------------
ModelNode* ModelNode::GetParent()
{
	return m_parent;
}


//------------------------------------------------------------------------------------------------
std::vector<ModelNode*> const& ModelNode::GetChildren()
{
	return m_children;
}


//------------------------------------------------------------------------------------------------
Mat44 ModelNode::GetLocalToParentTransform() const
{
	return m_localToParent;
}


//------------------------------------------------------------------------------------------------
ModelNode* ModelNode::GetInstance()
{
	ModelNode* node = new ModelNode();
	node->m_localToWorld = m_localToWorld;
	node->m_localToParent = m_localToParent;
	node->m_nodeName = m_nodeName;
	node->m_nodeType = m_nodeType;

	for ( uint nodeNum = 0; nodeNum < m_children.size(); nodeNum++ )
	{
		ModelNode* child = m_children[ nodeNum ]->GetInstance();
		child->m_parent = node;
		node->m_children.push_back( child );
	}

	return node;
}


//------------------------------------------------------------------------------------------------
Mat44 ModelNode::GetLocalToWorldTransform() const
{
	return m_localToWorld;
}


//------------------------------------------------------------------------------------------------
void ModelNode::SetLocalToWorldTransform( Mat44 transformMat )
{
	m_localToWorld = transformMat;
}


////------------------------------------------------------------------------------------------------
//Material* GeometryNode::GetMaterial() const
//{
//	return m_material;
//}


//------------------------------------------------------------------------------------------------
VertexBuffer* GeometryNode::GetVertexBuffer() const
{
	return g_theVisualDatabase->m_meshData[ m_vertDataID ].m_vbo;
}


//------------------------------------------------------------------------------------------------
Vertex_PCUTBN* GeometryNode::GetVertexArray() const
{
	return g_theVisualDatabase->m_meshData[ m_vertDataID ].m_mesh.data();
}


//------------------------------------------------------------------------------------------------
uint GeometryNode::GetVertexCount() const
{
	return g_theVisualDatabase->m_meshData[ m_vertDataID ].m_vertexCount;
}


//------------------------------------------------------------------------------------------------
ModelNode* GeometryNode::GetInstance()
{
	GeometryNode* node     = new GeometryNode();
	node->m_localToWorld   = m_localToWorld;
	node->m_localToParent  = m_localToParent;
	node->m_vertDataID     = m_vertDataID;
	//node->m_material       = g_theVisualDatabase->GetMaterialInstance( m_material );
	node->m_nodeName       = m_nodeName;
	node->m_nodeType       = m_nodeType;
	
	for ( uint nodeNum = 0; nodeNum < m_children.size(); nodeNum++ )
	{
		ModelNode* child = m_children[ nodeNum ]->GetInstance();
		child->m_parent = node;
		node->m_children.push_back( child );
	}

	return node;
}
