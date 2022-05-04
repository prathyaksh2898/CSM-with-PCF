#include "RandomNumberGenerator.hpp"
#include <stdlib.h>


//------------------------------------------------------------------------------------------------
RandomNumberGenerator g_rng;


//------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntLessThan( int maxNotInclusive )
{
	return rand() % maxNotInclusive;
}


//------------------------------------------------------------------------------------------------
int RandomNumberGenerator::GetRandomIntInRange( int minInclusive, int maxInclusive )
{
	int range = maxInclusive - minInclusive;
	return minInclusive + rand() % (range + 1);
}


//------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
	return static_cast< float >(rand()) / static_cast< float >(RAND_MAX);
}


//------------------------------------------------------------------------------------------------
float RandomNumberGenerator::GetRandomFloatInRange( float minInclusive, float maxInclusive )
{
	float range = maxInclusive - minInclusive;
	return minInclusive + GetRandomFloatZeroToOne() * (range);
}


//------------------------------------------------------------------------------------------------
bool RandomNumberGenerator::GetRandomRollGivenChance( float chance )
{
	float randomRoll = GetRandomFloatInRange( 0.0f, 1.0f );
	if ( randomRoll < chance )
		return true;
	else
		return false;
}
