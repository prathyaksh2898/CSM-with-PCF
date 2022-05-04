#include "Game/Game.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/LightStructure.hpp"

#include "ThirdParty/imgui/imgui.h"

//----------------------------------------------------------------------------------------------------
void Game::UpdateShaderLightDataUsingUI()
{
	ImGui::SetNextWindowPos( ImVec2( 0.0f, 0.0f ), ImGuiCond_Always );
	ImGui::Begin( "Artifact Data - Options" );
	if ( ImGui::CollapsingHeader( "Lights", ImGuiTreeNodeFlags_None ) )
	{
		if ( ImGui::Button( "Add Light" ) && m_numLights < MAXLIGHTS )
		{
			m_shaderLightData.m_lights[ m_numLights ] = m_lightPresets[ DIRECTIONAL_LIGHT ];
			m_numLights++;
		}

		ImGui::SameLine();

		if ( ImGui::Button( "Remove Light" ) && m_numLights > 0 )
		{
			m_numLights--;
			m_shaderLightData.m_lights[ m_numLights ] = LightDataC();
			m_lightCamFollow[ m_numLights ] = false;
			m_lightCamDir[ m_numLights ] = false;
		}

		for ( int lightNum = 0; lightNum < m_numLights; lightNum++ )
		{
			if ( ImGui::TreeNode( Stringf( "Light %d", lightNum + 1 ).c_str() ) )
			{
				uint lightType = m_shaderLightData.m_lights[ lightNum ].m_lightType;
				ImGui::Combo( "Light Type", reinterpret_cast< int* >( &m_shaderLightData.m_lights[ lightNum ].m_lightType ), "Invalid\0Directional Light\0Spot Light\0Point Light\0" );
				
				if ( lightType != m_shaderLightData.m_lights[ lightNum ].m_lightType )
				{
					lightType = m_shaderLightData.m_lights[ lightNum ].m_lightType;
					m_shaderLightData.m_lights[ lightNum ] = m_lightPresets[ lightType ];
					m_lightRotating[lightNum] = false;
					m_lightMoving[lightNum] = false;
				}

				if ( m_shaderLightData.m_lights[lightNum].m_lightType == INVALID_LIGHT )
				{
					ImGui::TreePop();
					continue;
				}

				ImGui::InputFloat( "Light intensity", &m_shaderLightData.m_lights[ lightNum ].m_intensity );
				m_shaderLightData.m_lights[ lightNum ].m_intensity = ClampZeroToOne( m_shaderLightData.m_lights[ lightNum ].m_intensity );

				if ( lightType != DIRECTIONAL_LIGHT )
				{
					ImGui::Checkbox( "Follow Camera", &m_lightCamFollow[ lightNum ] );

					float position[ 3 ];
					position[ 0 ] = m_shaderLightData.m_lights[ lightNum ].m_worldPosition.x;
					position[ 1 ] = m_shaderLightData.m_lights[ lightNum ].m_worldPosition.y;
					position[ 2 ] = m_shaderLightData.m_lights[ lightNum ].m_worldPosition.z;

					if ( m_lightCamFollow[ lightNum ] || m_lightMoving[lightNum] )
					{
						ImGui::BeginDisabled( true );
					}

					ImGui::InputFloat3( "Light Position", position );

					if ( m_lightCamFollow[ lightNum ] || m_lightMoving[lightNum] )
					{
						ImGui::EndDisabled();
					}					
					
					m_shaderLightData.m_lights[ lightNum ].m_worldPosition.x = position[ 0 ];
					m_shaderLightData.m_lights[ lightNum ].m_worldPosition.y = position[ 1 ];
					m_shaderLightData.m_lights[ lightNum ].m_worldPosition.z = position[ 2 ];

					if ( !m_lightCamFollow[lightNum] )
					{
						bool isMoving = m_lightMoving[lightNum];
						ImGui::Checkbox( "Is Moving", &m_lightMoving[lightNum] );
						if ( m_lightMoving[lightNum] )
						{
							if ( !isMoving )
							{
								if ( m_lightStartPosition[lightNum] != m_lightEndPosition[lightNum] &&
									m_lightStartPosition[lightNum] != Vec3( 777.0f, 777.0f, 777.0f ) &&
									m_lightEndPosition[lightNum] != Vec3( 777.0f, 777.0f, 777.0f ) &&
									m_lightMoveSpeed[lightNum] > 0.0f )
								{
									m_shaderLightData.m_lights[lightNum].m_worldPosition = m_lightStartPosition[lightNum];
								}
							}

							float startPosition[3];
							startPosition[0] = m_lightStartPosition[lightNum].x;
							startPosition[1] = m_lightStartPosition[lightNum].y;
							startPosition[2] = m_lightStartPosition[lightNum].z;

							float endPosition[3];
							endPosition[0] = m_lightEndPosition[lightNum].x;
							endPosition[1] = m_lightEndPosition[lightNum].y;
							endPosition[2] = m_lightEndPosition[lightNum].z;

							ImGui::InputFloat3( "Light Start Position", startPosition );
							ImGui::InputFloat3( "Light End Position", endPosition );

							if ( startPosition[0] != m_lightStartPosition[lightNum].x ||
								startPosition[1] != m_lightStartPosition[lightNum].y ||
								startPosition[2] != m_lightStartPosition[lightNum].z ||
								endPosition[0] != m_lightEndPosition[lightNum].x ||
								endPosition[1] != m_lightEndPosition[lightNum].y ||
								endPosition[2] != m_lightEndPosition[lightNum].z )
							{
								m_shaderLightData.m_lights[lightNum].m_worldPosition = m_lightStartPosition[lightNum];

								m_lightEndPosition[lightNum].x = endPosition[0];
								m_lightEndPosition[lightNum].y = endPosition[1];
								m_lightEndPosition[lightNum].z = endPosition[2];
								
								m_lightStartPosition[lightNum].x = startPosition[0];
								m_lightStartPosition[lightNum].y = startPosition[1];
								m_lightStartPosition[lightNum].z = startPosition[2];
							}

							ImGui::InputFloat( "Light Move Speed", &m_lightMoveSpeed[lightNum] );
						}
					}
				}
				
				if ( lightType != POINT_LIGHT )
				{
					ImGui::Checkbox( "Light Direction follows Camera forward", &m_lightCamDir[ lightNum ] );

					float direction[ 3 ];
					direction[ 0 ] = m_shaderLightData.m_lights[ lightNum ].m_direction.x;
					direction[ 1 ] = m_shaderLightData.m_lights[ lightNum ].m_direction.y;
					direction[ 2 ] = m_shaderLightData.m_lights[ lightNum ].m_direction.z;

					if ( m_lightCamDir[ lightNum ] )
					{
						ImGui::BeginDisabled( true );
					}

					ImGui::InputFloat3( "Light Direction", direction );

					if ( m_lightCamDir[ lightNum ] )
					{
						ImGui::EndDisabled();
					}

					m_shaderLightData.m_lights[ lightNum ].m_direction.x = direction[ 0 ];
					m_shaderLightData.m_lights[ lightNum ].m_direction.y = direction[ 1 ];
					m_shaderLightData.m_lights[ lightNum ].m_direction.z = direction[ 2 ];
					m_shaderLightData.m_lights[ lightNum ].m_direction.Normalize();
				}
				
				if ( lightType != DIRECTIONAL_LIGHT )
				{
					float attenuation[ 3 ];
					attenuation[ 0 ] = m_shaderLightData.m_lights[ lightNum ].m_attenuation.x;
					attenuation[ 1 ] = m_shaderLightData.m_lights[ lightNum ].m_attenuation.y;
					attenuation[ 2 ] = m_shaderLightData.m_lights[ lightNum ].m_attenuation.z;

					ImGui::InputFloat3( "Attenuation", attenuation );

					m_shaderLightData.m_lights[ lightNum ].m_attenuation.x = attenuation[ 0 ];
					m_shaderLightData.m_lights[ lightNum ].m_attenuation.y = attenuation[ 1 ];
					m_shaderLightData.m_lights[ lightNum ].m_attenuation.z = attenuation[ 2 ];

					float specularAttenuation[ 3 ];
					specularAttenuation[ 0 ] = m_shaderLightData.m_lights[ lightNum ].m_specularAttenuation.x;
					specularAttenuation[ 1 ] = m_shaderLightData.m_lights[ lightNum ].m_specularAttenuation.y;
					specularAttenuation[ 2 ] = m_shaderLightData.m_lights[ lightNum ].m_specularAttenuation.z;

					ImGui::InputFloat3( "Specular Attenuation", specularAttenuation );

					m_shaderLightData.m_lights[ lightNum ].m_specularAttenuation.x = specularAttenuation[ 0 ];
					m_shaderLightData.m_lights[ lightNum ].m_specularAttenuation.y = specularAttenuation[ 1 ];
					m_shaderLightData.m_lights[ lightNum ].m_specularAttenuation.z = specularAttenuation[ 2 ];

					if ( lightType == SPOT_LIGHT )
					{
						ImGui::InputFloat( "Dot Inner Angle", &m_shaderLightData.m_lights[ lightNum ].m_dotInnerAngle );
						ImGui::InputFloat( "Dot Outer Angle", &m_shaderLightData.m_lights[ lightNum ].m_dotOuterAngle );
					}
				}
				
				if ( lightType != POINT_LIGHT )
				{
					ImGui::Checkbox( "Light Rotating?", &m_lightRotating[lightNum] );
					if ( m_lightRotating[lightNum] )
					{
						ImGui::Combo( "Rotation Axis", reinterpret_cast<int*>(&m_lightRotationAxes[lightNum]), "+X\0-X\0+Y\0-Y\0+Z\0-Z" );
					}

					bool shadowCasting = m_shaderLightData.m_lights[lightNum].m_isShadowCasting ? true : false;
					ImGui::Checkbox( "Shadow Casting?", &shadowCasting );
					m_shaderLightData.m_lights[lightNum].m_isShadowCasting = shadowCasting ? 1 : 0;
				}

				ImGui::TreePop();
			}
		}
	}

	if ( ImGui::CollapsingHeader( "Cascade Options", ImGuiTreeNodeFlags_None ) )
	{
		ImGui::Checkbox( "Enable PCF", &m_enablePCF );

		ImGui::SliderInt( "Number of Cascades", &m_numCascades, 1, NUM_CASCADES );

		for ( int cascadeNum = 0; cascadeNum < m_numCascades; cascadeNum++ )
		{
			int cascadeMin = 0;
			int cascadeMax = 100;

			ImGui::SliderInt( Stringf( "Depth of Cascade %d", cascadeNum ).c_str(), &m_cascadeDepthPercent[cascadeNum], cascadeMin, cascadeMax );

			if ( cascadeNum != 0 && m_cascadeDepthPercent[cascadeNum] < m_cascadeDepthPercent[cascadeNum - 1] )
			{
				for ( int recursiveCascade = cascadeNum - 1; recursiveCascade >= 0; recursiveCascade-- )
				{
					if ( m_cascadeDepthPercent[cascadeNum] < m_cascadeDepthPercent[recursiveCascade] )
					{
						m_cascadeDepthPercent[recursiveCascade] = m_cascadeDepthPercent[cascadeNum];
					}
				}
			}

			if ( cascadeNum != NUM_CASCADES - 1 && m_cascadeDepthPercent[cascadeNum] > m_cascadeDepthPercent[cascadeNum + 1] )
			{
				for ( int recursiveCascade = cascadeNum + 1; recursiveCascade < NUM_CASCADES; recursiveCascade++ )
				{
					if ( m_cascadeDepthPercent[cascadeNum] > m_cascadeDepthPercent[recursiveCascade] )
					{
						m_cascadeDepthPercent[recursiveCascade] = m_cascadeDepthPercent[cascadeNum];
					}
				}				
			}
		}
	}

	if ( ImGui::CollapsingHeader( "Debug Lights", ImGuiTreeNodeFlags_None ) )
	{
		ImGui::Checkbox( "Debug Specific Light?", &m_debugSpecificLight );

		if ( m_debugSpecificLight )
		{
			if ( m_debugLightNumber >= m_numLights )
				m_debugLightNumber = 0;

			std::vector<char> specificLightString;
			for ( int lightNum = 0; lightNum < m_numLights; lightNum++ )
			{
				specificLightString.push_back( static_cast< char >( lightNum + 49 ) );
				specificLightString.push_back( 0 );
			}
			specificLightString.push_back( 0 );

			ImGui::Combo( "Debug Light", &m_debugLightNumber, specificLightString.data() );

			if ( m_shaderLightData.m_lights[m_debugLightNumber].m_lightType == DIRECTIONAL_LIGHT )
			{
				ImGui::Checkbox( "Debug Frustum", &m_debugFrustum );
				ImGui::Checkbox( "Debug Cascades", &m_debugCascades );
				ImGui::Checkbox( "Debug Specific Cascade?", &m_debugSpecCascades );

				ImGui::BeginDisabled( !m_debugSpecCascades );
				ImGui::Combo( "Cascade Number", &m_specCascadeNum, " 0\0 1\0 2\0" );
				ImGui::EndDisabled();
			}

			ImGui::Separator();
			ImGui::Checkbox( "Debug DepthBuffer", &m_debugDepthBuffer );

			ImGui::BeginDisabled( !m_debugDepthBuffer );
			if ( m_shaderLightData.m_lights[m_debugLightNumber].m_lightType == DIRECTIONAL_LIGHT )
			{
				ImGui::Checkbox( "Debug All DepthBuffers", &m_debugAllDepthBuffers );
				ImGui::BeginDisabled( m_debugAllDepthBuffers );
				ImGui::Combo( "Depth Buffer Number", &m_debugCascadeNum, " 0\0 1\0 2\0" );
				ImGui::EndDisabled();
			}
			else
			{
				m_debugCascadeNum = 0;
			}
			ImGui::EndDisabled();
		}
	}
	
	ImGui::End();
}

