#include "VisualDatabase.hpp"

#include "Engine/3D/Material.hpp"
#include "Engine/3D/Model.hpp"
#include "Engine/3D/FBXLoader.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"


//------------------------------------------------------------------------------------------------
VisualDatabase* g_theVisualDatabase = nullptr;


//------------------------------------------------------------------------------------------------
static void ReadNormal( FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vec3& outNormal )
{
	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal( 0 );

	switch ( vertexNormal->GetMappingMode() )
	{
		case FbxGeometryElement::eByControlPoint:
			switch ( vertexNormal->GetReferenceMode() )
			{
				case FbxGeometryElement::eDirect:
				{
					outNormal.x = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[0] );
					outNormal.y = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[1] );
					outNormal.z = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[2] );
					
					//float wVal = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[3] );
					//outNormal = wVal > 0 ? outNormal : -outNormal;
				}
				break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexNormal->GetIndexArray().GetAt( inCtrlPointIndex );
					
					outNormal.x = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[0] );
					outNormal.y = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[1] );
					outNormal.z = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[2] );
					
					//float wVal = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[3] );
					//outNormal = wVal > 0 ? outNormal : -outNormal;
				}
				break;
				default:
					throw std::exception( "Invalid Reference" );
			}
			break;
		case FbxGeometryElement::eByPolygonVertex:
			switch ( vertexNormal->GetReferenceMode() )
			{
				case FbxGeometryElement::eDirect:
				{
					outNormal.x = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inVertexCounter ).mData[0] );
					outNormal.y = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inVertexCounter ).mData[1] );
					outNormal.z = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inVertexCounter ).mData[2] );
					
					//float wVal = static_cast< float >( vertexNormal->GetDirectArray().GetAt( inVertexCounter ).mData[3] );
					//outNormal = wVal > 0 ? outNormal : -outNormal;
				}
				break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexNormal->GetIndexArray().GetAt( inVertexCounter );
					
					outNormal.x = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[0] );
					outNormal.y = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[1] );
					outNormal.z = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[2] );
					
					//float wVal = static_cast< float >( vertexNormal->GetDirectArray().GetAt( index ).mData[3] );
					//outNormal = wVal > 0 ? outNormal : -outNormal;
				}
				break;
				default: throw std::exception( "Invalid Reference" );
			}

			break;
	}
}


//------------------------------------------------------------------------------------------------
static void ReadTangent( FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vec3& outTangent )
{
	FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent( 0 );

	switch ( vertexTangent->GetMappingMode() )
	{
		case FbxGeometryElement::eByControlPoint:
			switch ( vertexTangent->GetReferenceMode() )
			{
				case FbxGeometryElement::eDirect:
				{					
					outTangent.x = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inCtrlPointIndex ).mData[0] );
					outTangent.y = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inCtrlPointIndex ).mData[1] );
					outTangent.z = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inCtrlPointIndex ).mData[2] );
					
					//float wVal = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inCtrlPointIndex ).mData[3] );
					//outTangent = wVal > 0 ? outTangent : -outTangent;
				}
				break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexTangent->GetIndexArray().GetAt( inCtrlPointIndex );
					
					outTangent.x = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[0] );
					outTangent.y = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[1] );
					outTangent.z = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[2] );

					//float wVal = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[3] );
					//outTangent = wVal > 0 ? outTangent : -outTangent;
				}
				break;
				default:
					throw std::exception( "Invalid Reference" );
			}
			break;
		case FbxGeometryElement::eByPolygonVertex:
			switch ( vertexTangent->GetReferenceMode() )
			{
				case FbxGeometryElement::eDirect:
				{
					outTangent.x = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inVertexCounter ).mData[0] );
					outTangent.y = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inVertexCounter ).mData[1] );
					outTangent.z = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inVertexCounter ).mData[2] );
					
					//float wVal = static_cast< float >( vertexTangent->GetDirectArray().GetAt( inVertexCounter ).mData[3] );
					//outTangent = wVal > 0 ? outTangent : -outTangent;
				}
				break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexTangent->GetIndexArray().GetAt( inVertexCounter );
					
					outTangent.x = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[0] );
					outTangent.y = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[1] );
					outTangent.z = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[2] );
					
					//float wVal = static_cast< float >( vertexTangent->GetDirectArray().GetAt( index ).mData[3] );
					//outTangent = wVal > 0 ? outTangent : -outTangent;
				}
				break;
				default: throw std::exception( "Invalid Reference" );
			}

			break;
	}
}


//------------------------------------------------------------------------------------------------
static void ReadBinormal( FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, Vec3& outBinormal )
{
	FbxGeometryElementBinormal* vertexBinormal = inMesh->GetElementBinormal( 0 );

	switch ( vertexBinormal->GetMappingMode() )
	{
		case FbxGeometryElement::eByControlPoint:
			switch ( vertexBinormal->GetReferenceMode() )
			{
				case FbxGeometryElement::eDirect:
				{
					outBinormal.x = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[0] );
					outBinormal.y = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[1] );
					outBinormal.z = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[2] );

					//float wVal = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inCtrlPointIndex ).mData[3] );
					//outBinormal = wVal > 0 ? outBinormal : -outBinormal;
				}
				break;

				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexBinormal->GetIndexArray().GetAt( inCtrlPointIndex );

					outBinormal.x = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( index ).mData[0] );
					outBinormal.y = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( index ).mData[1] );
					outBinormal.z = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( index ).mData[2] );

					//float wVal = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( index ).mData[3] );
					//outBinormal = wVal > 0 ? outBinormal : -outBinormal;
				}
				break;
				default:
					break;
			}
			break;
		case FbxGeometryElement::eByPolygonVertex:
			switch ( vertexBinormal->GetReferenceMode() )
			{
				case FbxGeometryElement::eDirect:
				{
					outBinormal.x = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inVertexCounter ).mData[0] );
					outBinormal.y = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inVertexCounter ).mData[1] );
					outBinormal.z = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inVertexCounter ).mData[2] );

					//float wVal = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( inVertexCounter ).mData[3] );
					//outBinormal = wVal > 0 ? outBinormal : -outBinormal;
				}
				break;
				case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexBinormal->GetIndexArray().GetAt( inVertexCounter );

					outBinormal.x = static_cast< float >(vertexBinormal->GetDirectArray().GetAt( index ).mData[ 0 ]);
					outBinormal.y = static_cast< float >(vertexBinormal->GetDirectArray().GetAt( index ).mData[ 1 ]);
					outBinormal.z = static_cast< float >(vertexBinormal->GetDirectArray().GetAt( index ).mData[ 2 ]);

					//float wVal = static_cast< float >( vertexBinormal->GetDirectArray().GetAt( index ).mData[3] );
					//outBinormal = wVal > 0 ? outBinormal : -outBinormal;
				}
				break;
				default: 
					break;
			}

			break;
	}
}


//------------------------------------------------------------------------------------------------
VisualDatabase::VisualDatabase( VisualDatabaseConfig config ):
	m_config( config )
{

}


//------------------------------------------------------------------------------------------------
VisualDatabase::~VisualDatabase()
{

}


//------------------------------------------------------------------------------------------------
void VisualDatabase::Startup()
{
	//m_materialData.m_materials.clear();
	//m_materialData.m_materialInstances.clear();
	
	m_modelData.m_models.clear();
	m_modelData.m_modelInstanceNumbers.clear();
	
	m_meshData.clear();
}


//------------------------------------------------------------------------------------------------
void VisualDatabase::Shutdown()
{
	//for ( int matNum = 0; matNum < m_materialData.m_materials.size(); matNum++ )
	//{
	//	delete m_materialData.m_materials[ matNum ];
	//	m_materialData.m_materials[ matNum ] = nullptr;
	//}

	//for ( int matInstanceNum = 0; matInstanceNum < m_materialData.m_materials.size(); matInstanceNum++ )
	//{
	//	delete m_materialData.m_materials[ matInstanceNum ];
	//	m_materialData.m_materials[ matInstanceNum ] = nullptr;
	//}

	for ( int meshNum = 0; meshNum < m_meshData.size(); meshNum++ )
	{
		g_theRenderer->DestroyVertexBuffer( m_meshData[ meshNum ].m_vbo );
		m_meshData[ meshNum ].m_vbo = nullptr;
	}

	for ( int modelNum = 0; modelNum < m_modelData.m_models.size(); modelNum++ )
	{
		delete m_modelData.m_models[ modelNum ];
		m_modelData.m_models[ modelNum ] = nullptr;
	}

	for ( int modelNum = 0; modelNum < m_modelInstances.size(); modelNum++ )
	{
		delete m_modelInstances[ modelNum ];
		m_modelInstances[ modelNum ] = nullptr;
	}
}


//------------------------------------------------------------------------------------------------
Model* VisualDatabase::CreateOrGetModelFromFBX( char const* m_filePath )
{
	for ( Model*& model : m_modelData.m_models )
	{
		if ( _strcmpi( m_filePath, model->m_filePath.c_str() ) == 0 )
		{
			Model* newMod = model->GetInstance();
			newMod->m_instanceId = ++m_modelData.m_modelInstanceNumbers[model->m_id];
			
			m_modelInstances.push_back( newMod );

			return newMod;
		}
	}

	if ( g_theFBXLoader == nullptr )
	{
		ERROR_AND_DIE( "No FBX Loader Found!!!!" );
	}

	Model* model = g_theFBXLoader->LoadModelFromFBX( m_filePath );
	model->m_instanceId = 0;
	
	m_modelData.m_modelInstanceNumbers.push_back( 0 );
	m_modelData.m_models.push_back( model );

	model->m_id = static_cast< uint > ( m_modelData.m_models.size() - 1 );

	Model* modInstance = model->GetInstance();
	modInstance->m_instanceId = ++m_modelData.m_modelInstanceNumbers[ model->m_id ];

	m_modelInstances.push_back( modInstance );

	return modInstance;
}


////------------------------------------------------------------------------------------------------
//Material* VisualDatabase::CreateOrGetMaterial( FbxSurfaceMaterial* fbxMaterial )
//{
//	if ( fbxMaterial != nullptr )
//	{
//		for ( Material*& mat : m_materialData.m_materials )
//		{
//			if ( _strcmpi( mat->m_matName.c_str(), fbxMaterial->GetName() ) == 0 )
//			{
//				//Material exists
//				Material* materialInstance = new Material();
//				materialInstance->m_matName = mat->m_matName; 
//				materialInstance->m_id = mat->m_id;
//				materialInstance->m_instanceID = ++m_materialData.m_materialInstances[ mat->m_id ];
//				materialInstance->m_texture = mat->m_texture;
//				materialInstance->m_shader = mat->m_shader;
//
//				m_materialInstances.push_back( materialInstance );
//				return materialInstance;
//			}
//		}
//
//		Material* newMaterial = new Material();
//		newMaterial->m_matName = std::string( fbxMaterial->GetName() );
//		FbxProperty prop = fbxMaterial->FindProperty( fbxMaterial->sDiffuse );
//		int textures = prop.GetSrcObjectCount<FbxTexture>();
//		if ( textures > 0 )
//		{
//			FbxFileTexture* texture = FbxCast<FbxFileTexture>( prop.GetSrcObject<FbxTexture>( 0 ) );
//			if ( texture != nullptr )
//			{
//				std::string texDesc = std::string( texture->GetFileName() );
//				newMaterial->m_texture = g_theRenderer->CreateOrGetTextureFromFile( texDesc.c_str() );
//			}
//		}
//
//		FbxProperty normalProp = fbxMaterial->FindProperty( fbxMaterial->sNormalMap );
//		
//		if ( normalProp.IsValid() )
//		{
//			FbxFileTexture* texture = FbxCast<FbxFileTexture>( normalProp.GetSrcObject<FbxTexture>( 0 ) );
//			if ( texture != nullptr )
//			{
//				std::string texDesc = std::string( texture->GetFileName() );
//				newMaterial->m_normalTexture = g_theRenderer->CreateOrGetTextureFromFile( texDesc.c_str() );
//			}
//		}
//
//		std::string shaderName = std::string( fbxMaterial->ShadingModel.Get().Buffer() );
//		std::string shaderPath = "Data/Shaders/";
//		shaderPath.append( shaderName );
//
//		Shader* shader = g_theRenderer->CreateOrGetShaderFromFile( shaderPath.c_str() );
//		newMaterial->m_shader = shader;
//
//		m_materialData.m_materials.push_back( newMaterial );
//		m_materialData.m_materialInstances.push_back( 0 );
//
//		newMaterial->m_id = static_cast< uint >( m_materialData.m_materials.size() - 1 );
//
//		Material* materialInstance        = new Material();
//		materialInstance->m_matName       = newMaterial->m_matName;
//		materialInstance->m_id            = newMaterial->m_id;
//		materialInstance->m_instanceID    = ++m_materialData.m_materialInstances[ newMaterial->m_id ];
//		materialInstance->m_texture       = newMaterial->m_texture;
//		materialInstance->m_normalTexture = newMaterial->m_normalTexture;
//		materialInstance->m_shader        = newMaterial->m_shader;
//
//		m_materialInstances.push_back( materialInstance );
//		return materialInstance;
//	}
//	
//	ERROR_AND_DIE( "Trying to create a material with null FBXSurfaceMaterial Node" );
//}
//
//
////------------------------------------------------------------------------------------------------
//Material* VisualDatabase::CreateOrGetMaterial( std::string matName, Texture* texture, Shader* shader )
//{
//	for ( Material*& mat : m_materialData.m_materials )
//	{
//		if ( _strcmpi( mat->m_matName.c_str(), matName.c_str() ) == 0 )
//		{
//			Material* materialInstance = new Material();
//			materialInstance->m_matName = mat->m_matName;
//			materialInstance->m_id = mat->m_id;
//			materialInstance->m_instanceID = ++m_materialData.m_materialInstances[ mat->m_id ];
//			materialInstance->m_texture = mat->m_texture;
//			materialInstance->m_shader = mat->m_shader;
//
//			m_materialInstances.push_back( materialInstance );
//			return materialInstance;
//		}
//	}
//
//	Material* newMaterial = new Material();
//	newMaterial->m_matName = matName;
//	newMaterial->m_texture = texture;
//	newMaterial->m_shader = shader;
//	m_materialData.m_materials.push_back( newMaterial );
//	m_materialData.m_materialInstances.push_back( 0 );
//	newMaterial->m_id = static_cast< uint >( m_materialData.m_materials.size() - 1 );
//
//	Material* materialInstance = new Material();
//	materialInstance->m_matName = newMaterial->m_matName;
//	materialInstance->m_id = newMaterial->m_id;
//	materialInstance->m_instanceID = ++m_materialData.m_materialInstances[ newMaterial->m_id ];
//	materialInstance->m_texture = newMaterial->m_texture;
//	materialInstance->m_shader = newMaterial->m_shader;
//
//	m_materialInstances.push_back( materialInstance );
//	return materialInstance;
//}


//------------------------------------------------------------------------------------------------
uint VisualDatabase::CreateOrGetVertsFromFBX( FbxNode* node, char const* filepath )
{
	bool isMesh = false;
	std::string name;

	FbxNodeAttribute* attribute = nullptr;
	for ( int attributeNum = 0; attributeNum < node->GetNodeAttributeCount(); ++attributeNum )
	{
		attribute = node->GetNodeAttributeByIndex( attributeNum );
		if ( attribute->GetAttributeType() == FbxNodeAttribute::eMesh )
		{
			name = std::string( filepath ) + std::string("_") + std::to_string(attribute->GetUniqueID());
			isMesh = true;
			break;
		}
	}

	ASSERT_OR_DIE( isMesh, "Node is not a mesh node" );

	for ( MeshData& data : m_meshData )
	{
		if ( _strcmpi( name.c_str(), data.m_meshName.c_str() ) == 0 )
		{
			return data.m_id;
		}
	}

	MeshData data;
	data.m_id = static_cast< uint >( m_meshData.size() );
	data.m_meshName = name;

	std::vector<Vertex_PCUTBN> meshVerts;
	FbxMesh* mesh = node->GetMesh();

	mesh->GenerateTangentsDataForAllUVSets();

	FbxStringList uvNames;
	mesh->GetUVSetNames( uvNames );
	
	int polyCount = mesh->GetPolygonCount();
	meshVerts.reserve( polyCount * 3 );

	bool hasTangents  = mesh->GetElementTangentCount() > 0 ? true : false;
	bool hasBinormals = mesh->GetElementBinormalCount() > 0 ? true : false;
	bool hasNormals   = mesh->GetElementNormalCount() > 0 ? true : false;

	for ( int polygonNum = 0; polygonNum < polyCount; ++polygonNum )
	{
		for ( int vertexNum = 0; vertexNum < 3; ++vertexNum )
		{
			Vertex_PCUTBN vertex;
			FbxVector2 texCoord;
			FbxVector4 normal;
			bool unmapped = false;
			int vertexIndex = mesh->GetPolygonVertex( polygonNum, vertexNum );
			FbxVector4 vertPos = mesh->GetControlPointAt( vertexIndex );

			int ctrlPoint = mesh->GetPolygonVertex( polygonNum, vertexNum );

			mesh->GetPolygonVertexUV( polygonNum, vertexNum, uvNames[ 0 ], texCoord, unmapped );
			mesh->GetPolygonVertexNormal( polygonNum, vertexNum, normal );
			int vertIndex = polygonNum * 3 + vertexNum;

			if ( hasNormals )
			{
				ReadNormal( mesh, ctrlPoint, vertIndex, vertex.m_normal );
			}
			if ( hasTangents )
			{
				ReadTangent(mesh, ctrlPoint, vertIndex, vertex.m_tangent );
			}
			if ( hasBinormals )
			{
				ReadBinormal( mesh, ctrlPoint, vertIndex, vertex.m_binormal );
			}

			vertex.m_position.x = static_cast< float >( vertPos.mData[ 0 ] );
			vertex.m_position.y = static_cast< float >( vertPos.mData[ 1 ] );
			vertex.m_position.z = static_cast< float >( vertPos.mData[ 2 ] );
			vertex.m_uvTexCoords.x = static_cast< float >( texCoord.mData[ 0 ] );
			vertex.m_uvTexCoords.y = static_cast< float >( texCoord.mData[ 1 ] );

			meshVerts.push_back( vertex );
		}
	}


	data.m_vbo = g_theRenderer->CreateDynamicVertexBuffer( sizeof( Vertex_PCUTBN ) );
	data.m_mesh = meshVerts;
	data.m_vbo->CopyVertexData( meshVerts.data(), meshVerts.size() * sizeof( Vertex_PCUTBN ), sizeof( Vertex_PCUTBN ) );
	data.m_vertexCount = static_cast< uint >( meshVerts.size() );
	m_meshData.push_back( data );

	return data.m_id;
}


//------------------------------------------------------------------------------------------------
//Material* VisualDatabase::GetMaterialInstance( Material* material )
//{
//	if ( material == nullptr )
//	{
//		ERROR_AND_DIE("Material is NULL - Trying to Instance null");
//	}
//
//	Material* materialInstance        = new Material();
//	materialInstance->m_matName       = material->m_matName;
//	materialInstance->m_id            = material->m_id;
//	materialInstance->m_instanceID    = ++m_materialData.m_materialInstances[ material->m_id ];
//	materialInstance->m_texture       = material->m_texture;
//	materialInstance->m_normalTexture = material->m_normalTexture;
//	materialInstance->m_shader        = material->m_shader;
//
//	m_materialInstances.push_back( materialInstance );
//	return materialInstance;
//}
