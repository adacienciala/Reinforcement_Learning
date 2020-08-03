#pragma once

#include "activationFunctions.h"
#include <vector>

#define vector1D(type) std::vector<type>
#define vector2D(type) std::vector<std::vector<type>>
#define vector3D(type) std::vector<std::vector<std::vector<type>>>


class Neuron
{

private:

	vector1D(double) weights;
	double value;

public:

	Neuron(const vector1D(double)& WEIGHTS = vector1D(double)()) : weights(WEIGHTS), value(0.0) {};

	void computeValue(const vector1D(double)& dataIn, double (*function) (double) = NULL);

	vector1D(double)* getWeights();
	double* getValue();
	void setWeights(const vector1D(double)& _weights);

};

class neuralNetwork
{

private:

	vector2D(double) rightValues;
	double alpha;

	std::vector<std::pair<double (*) (double), double (*) (double)>> funTable = 
				{ { NULL, NULL }, { &reluFunction, &reluDeriv }, { &sigmoidFunction, &sigmoidDeriv }, { &tanhFunction, &tanhDeriv } };
	std::vector<std::vector<double> (*) (const std::vector<double>&)> outFunTable =
				{ &softmaxFunction };


public:

	vector2D(double) dropoutMask;
	vector3D(double) networkValues;

	std::vector<std::pair<std::vector<Neuron>, function >> networkLayers;
	
	neuralNetwork() {};
	neuralNetwork(double _alpha, const vector2D(double)& _rightValues, const std::vector<std::pair<int, function>>& neuronsNums = std::vector<std::pair<int, function>>());

	void setInputData(vector1D(double)& _inputData);
	void setRightValues(const vector2D(double)& _rightValues);
	void setAlpha(double& _alpha);

	std::vector<vector1D(double)*> getLayerWeights(std::pair<std::vector<Neuron>, function>& layer);
	vector1D(double) getLayerValues(std::pair<std::vector<Neuron>, function>& layer);
	vector1D(double) getOutputValues();
	vector2D(double*) getHiddenValues();

	void computeValues(const vector2D(double)& inputData, double dropoutValOff = 0.0); // predict
	void correctWeights(double dropoutValOff = 0.0);

	void trainNetwork(int iterations, const vector2D(double)& inputD, const vector2D(double)& rightValues, double dropoutValOff = 0.0, int batchSize = 0); // fit
	void printNetwork();
};