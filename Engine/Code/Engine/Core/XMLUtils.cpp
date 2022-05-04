#include "XMLUtils.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
int ParseXmlAttribute( XmlElement const& element, char const* attributeName, int defaultValue )
{
	char const* intAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &intAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		int value = atoi( intAttributeValue );
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
char ParseXmlAttribute( XmlElement const& element, char const* attributeName, char defaultValue )
{
	char const* charAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &charAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		char value = *charAttributeValue;
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
bool ParseXmlAttribute( XmlElement const& element, char const* attributeName, bool defaultValue )
{
	char const* boolAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &boolAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		bool value = false;
		if ( _strcmpi( boolAttributeValue, "true" ) == 0 )
			value = true;
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
float ParseXmlAttribute( XmlElement const& element, char const* attributeName, float defaultValue )
{
	char const* floatAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &floatAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		float value = static_cast< float >( atof( floatAttributeValue ) );
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Rgba8 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue )
{
	char const* rgba8AttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &rgba8AttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		Rgba8 value;
		value.SetFromText( rgba8AttributeValue );
		return value;
	}

	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Vec2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec2 const& defaultValue )
{
	char const* vec2AttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &vec2AttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		Vec2 value;
		value.SetFromText( vec2AttributeValue );
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Vec3 ParseXmlAttribute( XmlElement const& element, char const* attributeName, Vec3 const& defaultValue )
{
	char const* vec3AttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &vec3AttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		Vec3 value;
		value.SetFromText( vec3AttributeValue );
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
IntVec2 ParseXmlAttribute( XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue )
{
	char const* intVec2AttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &intVec2AttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		IntVec2 value;
		value.SetFromText( intVec2AttributeValue );
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, std::string const& defaultValue )
{
	char const* stringAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &stringAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		std::string value( stringAttributeValue );
		return value;
	}
	return defaultValue;
}


//-----------------------------------------------------------------------------------------------
Strings ParseXmlAttribute( XmlElement const& element, char const* attributeName, Strings const& defaultValues )
{
	char const* stringsAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &stringsAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		Strings value = SplitStringOnDelimiter( stringsAttributeValue );
		return value;
	}
	return defaultValues;
}


//-----------------------------------------------------------------------------------------------
std::string ParseXmlAttribute( XmlElement const& element, char const* attributeName, char const* defaultValue )
{
	char const* stringAttributeValue = nullptr;
	XmlResult xmlQueryStatus = element.QueryStringAttribute( attributeName, &stringAttributeValue );
	if ( xmlQueryStatus == tinyxml2::XML_SUCCESS )
	{
		std::string value( stringAttributeValue );
		return value;
	}
	return std::string( defaultValue );
}


