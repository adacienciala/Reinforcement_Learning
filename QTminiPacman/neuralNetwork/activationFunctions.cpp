#include <numeric> // accumulate
#include <math.h>

#include "activationFunctions.h"

double reluFunction(double value)
{
	return (value < 0) ? 0.0 : value;
}

double reluDeriv(double value)
{
	return (value <= 0) ? 0.0 : 1.0;
}

double sigmoidFunction(double value)
{
	return 1.0 / (1 + exp(-1 * value));
}

double sigmoidDeriv(double value)
{
	return value * (1.0 - value);
}

double tanhFunction(double value)
{
	return (exp(2*value) - 1) / (exp(2*value) + 1);
}

double tanhDeriv(double value)
{
	return 1.0 - (value * value);
}

std::vector<double> softmaxFunction(const std::vector<double>& valueVector)
{
	std::vector<double> returnValues = valueVector;
	for (auto& value : returnValues)
	{
		value = exp(value);
	}
	double sum = std::accumulate(returnValues.begin(), returnValues.end(), 0.0);
	for (auto& value : returnValues)
	{
		value = value / sum;
	}
	return returnValues;
}