#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <stdarg.h>
#include <stringapiset.h>

//-----------------------------------------------------------------------------------------------
constexpr int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( char const* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( int maxLength, char const* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if ( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if ( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}


//-----------------------------------------------------------------------------------------------
Strings SplitStringOnDelimiter( const std::string& originalString, char delimiterToSplitOn )
{
	Strings partStrings;
	std::size_t found = static_cast< size_t >( 0 );
	std::size_t lastFound = found;
	found = originalString.find( delimiterToSplitOn, found );

	while ( found != std::string::npos )
	{
		partStrings.push_back( originalString.substr( lastFound, found - lastFound ) );
		lastFound = found + static_cast< size_t >( 1 );
		found = originalString.find( delimiterToSplitOn, lastFound );
	}

	if ( lastFound <= originalString.length() )
	{
		partStrings.push_back( originalString.substr( lastFound, originalString.length() - lastFound ) );
	}

	return partStrings;
}


//------------------------------------------------------------------------------------------------
std::wstring StringToWideString( std::string const& string )
{
	std::wstring result;
	result.resize( string.length() );
	for ( int index = 0; index < string.length(); index++ )
	{
		result[ index ] = string[ index ];
	}

	return result;
}



