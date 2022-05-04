#pragma once

class RandomNumberGenerator {
public:
	int GetRandomIntLessThan(int maxNotInclusive);
	int GetRandomIntInRange(int minInclusive, int maxInclusive);
	float GetRandomFloatZeroToOne();
	float GetRandomFloatInRange(float minInclusive, float maxInclusive);
	bool GetRandomRollGivenChance(float chance);

private:
	unsigned int m_seed = 0;
	int m_position = 0;
};