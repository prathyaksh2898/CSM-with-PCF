#include "NamedStrings.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"

//-----------------------------------------------------------------------------------------------
void NamedStrings::PopulateFromXmlElementAttributes( XmlElement const& element )
{
	tinyxml2::XMLAttribute const* attribute = element.FirstAttribute();
	while ( attribute )
	{
		std::string attributeName( attribute->Name() );
		std::string attributeValue( attribute->Value() );
		SetValue( attributeName, attributeValue );
		attribute = attribute->Next();
	}
}


//-----------------------------------------------------------------------------------------------
void NamedStrings::SetValue( std::string const& keyName, std::string const& newValue )
{
	m_keyValuePairs.insert( std::pair<std::string, std::string>( keyName, newValue ) );
}


//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, std::string const& defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		return m_keyValuePairs.find( keyName )->second;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
bool NamedStrings::GetValue( std::string const& keyName, bool defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		bool value = false;
		if ( _strcmpi( m_keyValuePairs.find( keyName )->second.c_str(), "true" ) == 0 )
			value = true;
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
int NamedStrings::GetValue( std::string const& keyName, int defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		return atoi( m_keyValuePairs.find( keyName )->second.c_str() );
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
float NamedStrings::GetValue( std::string const& keyName, float defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		return static_cast< float >( atof( m_keyValuePairs.find( keyName )->second.c_str() ) );
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
std::string NamedStrings::GetValue( std::string const& keyName, char const* defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		return m_keyValuePairs.find( keyName )->second;
	}
	return std::string( defaultValue );
}


//-----------------------------------------------------------------------------------------------
Rgba8 NamedStrings::GetValue( std::string const& keyName, Rgba8 const& defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		Rgba8 rgba8;
		rgba8.SetFromText( m_keyValuePairs.find( keyName )->second.c_str() );
		return rgba8;
	}

	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Vec2 NamedStrings::GetValue( std::string const& keyName, Vec2 const& defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		Vec2 vec2;
		vec2.SetFromText( m_keyValuePairs.find( keyName )->second.c_str() );
		return vec2;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Vec3 NamedStrings::GetValue( std::string const& keyName, Vec3 const& defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		Vec3 vec3;
		vec3.SetFromText( m_keyValuePairs.find( keyName )->second.c_str() );
		return vec3;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
IntVec2 NamedStrings::GetValue( std::string const& keyName, IntVec2 const& defaultValue ) const
{
	if ( m_keyValuePairs.find( keyName ) != m_keyValuePairs.end() )
	{
		IntVec2 intVec2;
		intVec2.SetFromText( m_keyValuePairs.find( keyName )->second.c_str() );
		return intVec2;
	}
	return defaultValue;
}
