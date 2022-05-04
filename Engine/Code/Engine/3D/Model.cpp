#include "Model.hpp"

#include "Engine/3D/ModelNode.hpp"


//------------------------------------------------------------------------------------------------
Model::Model( char const* filePath ):
	m_filePath( filePath )
{

}


//------------------------------------------------------------------------------------------------
Model::~Model()
{

}


//------------------------------------------------------------------------------------------------
Model* Model::GetInstance()
{
	Model* model = new Model( m_filePath.c_str() );
	model->m_id = m_id;
	
	model->m_node = m_node->GetInstance();

	return model;
}
