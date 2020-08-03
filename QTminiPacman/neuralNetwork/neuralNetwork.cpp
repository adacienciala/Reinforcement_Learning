#include <iostream>
#include <algorithm> // transform
#include <time.h>

#include "neuralNetwork/neuralNetwork.h"


// ---------------------------------- //
// ---------- class Neuron ---------- //



void Neuron::computeValue(const vector1D(double)& dataIn, double (*function) (double))
{
	// computes the value of the neuron
	// a sum of (data*weight)s
	// if there's a function provided, use it

	value = 0;
	for (int i = 0; i < dataIn.size(); ++i)
	{
		value += dataIn[i] * weights[i];
	}
	if (function) value = function(value);
}

vector1D(double)* Neuron::getWeights()
{
	// returns values of the weights

	return &weights;
}

double* Neuron::getValue()
{
	// returns the value of the neuron

	return &value;
}

void Neuron::setWeights(const vector1D(double)& _weights)
{
	// sets weights for all the inputs

	weights = _weights;
}



// ----------------------------------------- //
// ---------- class neuralNetwork ---------- //



neuralNetwork::neuralNetwork(double _alpha, const vector2D(double)& _rightValues, const std::vector<std::pair<int, function>>& neuronsNums)
{
	this->setRightValues(_rightValues);
	this->setAlpha(_alpha);

	srand(time(NULL));
	networkLayers = std::vector<std::pair<std::vector<Neuron>, function >>();

	if (!neuronsNums.size()) return;

	// generate input layer with one weight = 1.0 per neuron
	const auto& [num, funID] = neuronsNums.front();
	vector1D(Neuron) inputLayer;
	inputLayer.reserve(num);
	for (int i = 0; i < num; ++i)
	{
		inputLayer.push_back(Neuron({ 1.0 }));
	}
	networkLayers.push_back(std::make_pair(inputLayer, none));
	int prevInputCounter = num;

	// generate hidden and output layers with random weights (negative and positive)
	// and create the dropout mask
	dropoutMask = vector2D(double)(neuronsNums.size() - 2);
	for (int layer = 1; layer < neuronsNums.size(); ++layer)
	{
		const auto& [num, funID] = neuronsNums[layer];
		vector1D(Neuron) newLayer;
		for (int i = 0; i < num; ++i)
		{
			vector1D(double) randWeights;
			randWeights.reserve(prevInputCounter);
			for (int j = 0; j < prevInputCounter; ++j)
			{
				randWeights.push_back(((rand() % 21) - 10) / 100.0);
			}
			newLayer.push_back(Neuron(randWeights));
		}
		if (layer != neuronsNums.size()-1) 
			dropoutMask[layer-1] = vector1D(double)(num, 0.0);
		networkLayers.push_back(std::make_pair(newLayer, funID));
		prevInputCounter = num;
	}
}

void neuralNetwork::setInputData(vector1D(double)& _inputData)
{
	// sets the data in the input layer

	int i = 0;
	for (auto& neuron : networkLayers.front().first)
	{
		neuron.computeValue({ _inputData[i++] });
	}
}

void neuralNetwork::setRightValues(const vector2D(double)& _rightValues)
{
	// sets the values that the network is supposed to compute

	rightValues = _rightValues;
}

void neuralNetwork::setAlpha(double& _alpha)
{
	// sets the learnig alpha factor for the network

	alpha = _alpha;
}

std::vector<vector1D(double)*> neuralNetwork::getLayerWeights(std::pair<std::vector<Neuron>, function>& layer)
{
	// returns vector of values of the given layer's neurons

	std::vector<vector1D(double)*> layerWeights;
	layerWeights.reserve(layer.first.size());
	for (auto& neuron : layer.first)
	{
		layerWeights.push_back(neuron.getWeights());
	}
	return layerWeights;
}

vector1D(double) neuralNetwork::getLayerValues(std::pair<std::vector<Neuron>, function>& layer)
{
	// returns vector of values of the given layer's neurons

	vector1D(double) layerValues;
	layerValues.reserve(layer.first.size());
	for (auto& neuron : layer.first)
	{
		layerValues.push_back(*(neuron.getValue()));
	}
	return layerValues;
}

vector1D(double) neuralNetwork::getOutputValues()
{
	// returns vector of values of the output neurons

	return getLayerValues(networkLayers.back());
}

vector2D(double*) neuralNetwork::getHiddenValues()
{
	// returns vector of pointers to values of the hidden layers' neurons
	
	vector2D(double*) hiddenValues(dropoutMask.size());
	for (int i = 0; i < dropoutMask.size(); ++i)
	{
		for (int j = 0; j < dropoutMask[i].size(); ++i)
		{
			hiddenValues[i][j] = networkLayers[i + 1].first[j].getValue();
		}
	}

	return hiddenValues;
}

void neuralNetwork::computeValues(const vector2D(double)& inputData, double dropoutValOff)
{
	// computes values of every neuron layer by layer of every series
	// as current input takes the previous layer's values
	// applies dropout, if it's the training phase 

	networkValues = vector3D(double)(inputData.size());
	function outputFunction;
	for (int series = 0; series < inputData.size(); ++series)
	{
		networkValues[series] = vector2D(double)(networkLayers.size());
		outputFunction = none;

		// random dropout values for this series
		if (dropoutValOff)
		{
			for (int i = 0; i < dropoutMask.size(); ++i)
			{
				for (int j = 0; j < dropoutMask[i].size(); ++j)
				{
					if ((rand() % 100) > dropoutValOff * 100)
						dropoutMask[i][j] = 1 / (1.0 - dropoutValOff);
				}
			}
		}

		networkValues[series][0] = inputData[series];
		vector1D(double) currentData = inputData[series];
		vector1D(double) tempData;
		for (int layerID = 1; layerID < networkLayers.size(); ++layerID)
		{
			auto& [layer, funID] = networkLayers[layerID];
			if (funID >= funTable.size())
			{
				outputFunction = funID;
				funID = none;
			}
			for (int neuron = 0; neuron < layer.size(); ++neuron)
			{
				layer[neuron].computeValue(currentData, funTable[funID].first);
				double* value = layer[neuron].getValue();
				if (layerID != networkLayers.size() - 1 && dropoutValOff)
					 *value *= dropoutMask[layerID - 1][neuron];
				tempData.push_back(*value);
				networkValues[series][layerID].push_back(*value);
			}
			if (outputFunction)
			{
				vector1D(double) newOutputValues = getOutputValues();
				newOutputValues = outFunTable[outputFunction - funTable.size()](newOutputValues);
				double* value;
				for (int i = 0; i < newOutputValues.size(); ++i)
				{
					value = networkLayers[layerID].first[i].getValue();
					*value = newOutputValues[i];
					networkValues[series][layerID][i] = *value;
				}
			}
			currentData = tempData;
			tempData.clear();
		}
	}
}

void neuralNetwork::correctWeights(double dropoutValOff)
{
	// corrects the weights of the data

	int batchSize = networkValues.size();
	int layersNum = networkLayers.size();

	vector3D(double) deltas = vector3D(double)(batchSize);
	vector2D(std::vector<double>*) weights;
	weights.reserve(layersNum);
	for (auto& layer : networkLayers)
	{
		weights.push_back(getLayerWeights(layer));
	}

	for (int series = 0; series < batchSize; ++series)
	{
		// compute delta of: output layer
		deltas[series] = vector2D(double)(layersNum);
		deltas[series][layersNum - 1] = networkValues[series].back();
		std::transform(deltas[series][layersNum - 1].begin(), deltas[series][layersNum - 1].end(),
			rightValues[series].begin(), deltas[series][layersNum - 1].begin(), 
			[batchSize](double val1, double val2) { return (val1 - val2) / batchSize; });

		// compute delta of: hidden layers
		int layerID = layersNum - 1;
		std::for_each(networkLayers.rbegin()+1, networkLayers.rend() - 1, [&dropoutValOff, &series, &layerID, &weights, &deltas, this](std::pair<std::vector<Neuron>, function >& layer_pair)
			{
				auto& [layer, funID] = layer_pair;
				for (int neuronID = 0; neuronID < layer.size(); ++neuronID)
				{
					double delta = 0;
					for (int j = 0; j < deltas[series][layerID].size(); ++j)
					{
						delta += deltas[series][layerID][j] * (*weights[layerID][j])[neuronID];
					}
					if (funTable[funID].second) delta *= funTable[funID].second(networkValues[series][layerID-1][neuronID]);
					if (dropoutValOff) delta *= dropoutMask[layerID - 2][neuronID];
					deltas[series][layerID - 1].push_back(delta);
				}
				layerID--;
			});
	}

	// correct weights of all layers, going backwards
	int layerID = layersNum - 1;
	std::for_each(networkLayers.rbegin(), networkLayers.rend() - 1, [&layerID, &weights, &deltas, this](std::pair<std::vector<Neuron>, function >& layer_pair)
		{
			for (int series = 0; series < networkValues.size(); ++series)
			{
				auto& [layer, funID] = layer_pair;
				for (int neuronID = 0, i = 0; neuronID < layer.size(); ++neuronID, i = 0)
				{
					for (auto& weight : *weights[layerID][neuronID])
					{
						double value = networkValues[series][layerID - 1][i++];
						weight -= deltas[series][layerID][neuronID] * value * alpha;
					}
				}
			}
			layerID--;
		});
};

void neuralNetwork::trainNetwork(int iterations, const vector2D(double)& inputD, const vector2D(double)& rightValues, double dropoutValOff, int batchSize)
{
	int inputSize = inputD.size();
	if (!batchSize) batchSize = 1;
	int remainder;
	vector2D(double) batchInput(batchSize);
	vector2D(double) rightBatchValues(batchSize);
	for (int j = 0; j < iterations; ++j)
	{
		for (int i = 0; i + batchSize <= inputSize; i += batchSize)
		{
			batchInput.assign(	inputD.begin() + i, 
								inputD.begin() + (i + batchSize));
			rightBatchValues.assign(rightValues.begin() + i,
									rightValues.begin() + (i + batchSize));
			this->setRightValues(rightBatchValues);
			this->computeValues(batchInput, dropoutValOff); 
			this->correctWeights(dropoutValOff);
		}
		if ((remainder = inputSize % batchSize) != 0)
		{
			int firstToCompute = ((inputSize / batchSize) * batchSize);
			batchInput.clear();
			rightBatchValues.clear();
			batchInput.assign(	inputD.begin() + firstToCompute, 
								inputD.begin() + (firstToCompute + remainder));
			rightBatchValues.assign(rightValues.begin() + firstToCompute,
									rightValues.begin() + (firstToCompute + remainder));
			this->setRightValues(rightBatchValues);
			this->computeValues(batchInput, dropoutValOff);
			this->correctWeights(dropoutValOff);
		}
	}
};

void neuralNetwork::printNetwork()
{
	int i = 0;
	for (auto& [layer, fun] : networkLayers)
	{
		std::cout << i++ << ") Layer: " << std::endl;
		std::cout << "  *Function: " << fun << std::endl;
		for (auto& neuron : layer)
		{
			std::cout << "  *Neuron: " << std::endl;
			std::cout << "    *Weights: ";
			for (const auto weight : *neuron.getWeights())
			{
				std::cout << weight << ", ";
			}
			std::cout << std::endl << "    *Value: " << neuron.getValue() << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
};