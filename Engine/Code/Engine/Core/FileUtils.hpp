#pragma once
#include "Engine/Core/EngineCommon.hpp"

#include <string>
#include <vector>


//-----------------------------------------------------------------------------------------------
// Will read the contents of the file to a buffer of bytes.
bool FileReadToBuffer( std::vector<uint8_t>& outBuffer, std::string const& filename); 


// Similar to above, but will fill a string with the contents// returns true if file was successfully read
bool FileReadToString( std::string& outString, std::string const& filename );  


//-----------------------------------------------------------------------------------------------
// Will output the contents of the file to a buffer of bytes.
bool BufferWriteToFile( std::vector<uint8_t>& outBuffer, std::string const& filename );


// Similar to above, but will fill a string with the contents// returns true if file was successfully read
bool StringWriteToFile( std::string& outString, std::string const& filename );