#pragma once
#include "Game/Object.hpp"

#include <string>


//------------------------------------------------------------------------------------------------
class Model;
class ModelNode;
class Texture;


//------------------------------------------------------------------------------------------------
class FBXSceneObject : public Object
{
public:
	FBXSceneObject( Game* Owner, Vec3 const& startPosition, EulerAngles const& orientation, std::string objectPath );
	~FBXSceneObject();

	virtual void Update( float deltaseconds ) override;
	virtual void Render() const override;
	virtual void DebugRender() const override;

protected:
	virtual void UpdateTraverse( ModelNode* node );
	virtual void RenderTraverse( ModelNode* node ) const;

public:
	Model* m_fbxModel = nullptr;

	Texture* m_texture       = nullptr;
	Texture* m_normalTexture = nullptr;
};