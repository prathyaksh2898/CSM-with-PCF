#pragma once
#include "Engine/3D/FBXLoader.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"
#include "Engine/Math/Mat44.hpp"

#include <string>
#include <vector>


//------------------------------------------------------------------------------------------------
class GeometryNode;
class Model;
class VertexBuffer;

struct Camera;
struct Material;


//------------------------------------------------------------------------------------------------
enum class NodeType
{
	Base,
	Geometry,
	Light,
	Cam
};


//------------------------------------------------------------------------------------------------
class ModelNode
{
	friend class FBXLoader;
	friend class GeometryNode;
	friend class Model;

protected:
	virtual ModelNode* GetInstance();

	ModelNode* m_parent        = nullptr;
	
	Mat44 m_localToWorld  = Mat44();
	Mat44 m_localToParent = Mat44();
	
	std::vector<ModelNode*> m_children;

public:
	ModelNode();
	~ModelNode();

	GeometryNode*                  AsGeometryNode();
					               
	ModelNode*                     GetParent();

	std::vector<ModelNode*> const& GetChildren();

	Mat44                          GetLocalToParentTransform() const;
					               
	Mat44                          GetLocalToWorldTransform() const;
	void                           SetLocalToWorldTransform( Mat44 transformMat );

public:
	NodeType    m_nodeType = NodeType::Base;
	std::string m_nodeName;
};


//------------------------------------------------------------------------------------------------
class GeometryNode : public ModelNode
{
	friend class FBXLoader;
	friend class Model;

public:
	//Material*       GetMaterial() const;
	VertexBuffer*   GetVertexBuffer() const;
	Vertex_PCUTBN*  GetVertexArray() const;
	uint            GetVertexCount() const;

protected:
	ModelNode*      GetInstance() override;

	//Material*       m_material = nullptr;
	uint            m_vertDataID = 777;

};
