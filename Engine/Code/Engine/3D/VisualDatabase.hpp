#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"

#include "fbxsdk/include/fbxsdk.h"

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
class Model;
class Shader;
class Texture;
class VertexBuffer;

struct Material;


//-----------------------------------------------------------------------------------------------
struct VisualDatabaseConfig
{

};


//-----------------------------------------------------------------------------------------------
struct MeshData
{
	uint                       m_id = 0;
	std::string                m_meshName;
	VertexBuffer*              m_vbo = nullptr;
	std::vector<Vertex_PCUTBN> m_mesh;
	uint                       m_vertexCount = 0;
};


////-----------------------------------------------------------------------------------------------
//struct MaterialData
//{
//	std::vector<Material*>  m_materials;
//	std::vector<int>        m_materialInstances;
//};


//-----------------------------------------------------------------------------------------------
struct ModelData
{
	std::vector<Model*>  m_models;
	std::vector<uint>    m_modelInstanceNumbers;
};


//-----------------------------------------------------------------------------------------------
class VisualDatabase
{
public:
	VisualDatabase( VisualDatabaseConfig config );
	~VisualDatabase();
	void Startup();
	void Shutdown();

	Model*     CreateOrGetModelFromFBX( char const* m_filePath );
	
	//Material*  CreateOrGetMaterial( FbxSurfaceMaterial* material );
	//Material*  CreateOrGetMaterial( std::string matName, Texture* texture, Shader* shader );
	//Material*  GetMaterialInstance( Material* material );

	uint       CreateOrGetVertsFromFBX( FbxNode* node, char const* filepath );


public:
	VisualDatabaseConfig       m_config;
	ModelData                  m_modelData; 
	//MaterialData               m_materialData; 
	std::vector<MeshData>      m_meshData;

	//std::vector<Material*>     m_materialInstances;
	std::vector<Model*>        m_modelInstances;
};