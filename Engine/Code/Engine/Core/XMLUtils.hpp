#pragma once
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "ThirdParty/TinyXML2/tinyxml2.h"


//-----------------------------------------------------------------------------------------------
typedef tinyxml2::XMLElement XmlElement;
typedef tinyxml2::XMLError XmlResult;


//-----------------------------------------------------------------------------------------------
int          ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue);
char         ParseXmlAttribute(XmlElement const& element, char const* attributeName, char defaultValue);
bool         ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue);
float        ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue);
Rgba8        ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue);
Vec2         ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue);
Vec3         ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec3 const& defaultValue);
IntVec2      ParseXmlAttribute(XmlElement const& element, char const* attributeName, IntVec2 const& defaultValue);
std::string  ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue);
Strings      ParseXmlAttribute(XmlElement const& element, char const* attributeName, Strings const& defaultValues);

std::string  ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const* defaultValue);
