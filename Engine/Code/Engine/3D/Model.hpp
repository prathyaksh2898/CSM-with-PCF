#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Mat44.hpp"

#include <string>


//------------------------------------------------------------------------------------------------
class ModelNode;
class VisualDatabase;


//------------------------------------------------------------------------------------------------
class Model
{
	friend class VisualDatabase;
private:
	Model* GetInstance();
public:
	Model( char const* m_filePath );
	~Model();

public:
	std::string m_filePath;
	uint        m_id          = 0;
	uint        m_instanceId  = 0;
	ModelNode*  m_node        = nullptr;

};