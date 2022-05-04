#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <fstream>


//-----------------------------------------------------------------------------------------------
bool FileReadToBuffer(std::vector<uint8_t>& outBuffer, std::string const& filename)
{
	std::ifstream SourceTextFile;
	SourceTextFile.open(filename, std::ios::binary);
	
	if (SourceTextFile.fail()) 
	{
		return false;
	}
	
	SourceTextFile.seekg(0, std::ios::end);

	int sizeOfFile = static_cast<int>(SourceTextFile.tellg()) + 1;

	SourceTextFile.seekg(0, std::ios::beg);
	outBuffer.reserve(sizeOfFile);

	char charFromFile;

	while (SourceTextFile.get(charFromFile))
	{
		outBuffer.push_back(static_cast<uint8_t>(charFromFile));
	}

	outBuffer.push_back(0);
	SourceTextFile.close();

	return true;
}


//-----------------------------------------------------------------------------------------------
bool FileReadToString(std::string& outString, std::string const& filename)
{
	std::vector<uint8_t> outBuffer;

	bool status = FileReadToBuffer(outBuffer, filename);

	if (status)
	{
		outString.assign(reinterpret_cast<char*>(outBuffer.data()));
	}

	return status;
}


//-----------------------------------------------------------------------------------------------
bool BufferWriteToFile( std::vector<uint8_t>& outBuffer, std::string const& filename )
{
	std::ofstream DestinationTextFile;
	DestinationTextFile.open( filename, std::ios::binary );

	if ( DestinationTextFile.fail() )
	{
		return false;
	}

	DestinationTextFile.seekp( 0, std::ios::beg );
	DestinationTextFile.write( reinterpret_cast< char* >( outBuffer.data() ), outBuffer.size() );
	DestinationTextFile.close();

	return true;
}


//-----------------------------------------------------------------------------------------------
bool StringWriteToFile( std::string& outString, std::string const& filename )
{
	std::vector<uint8_t> outBuffer( outString.begin(), outString.end() );
	return BufferWriteToFile( outBuffer, filename );
}
