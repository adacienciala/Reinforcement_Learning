#pragma once

#include "vector"

enum function { none, RELU, SIGMOID, TANH, SOFTMAX };

double reluFunction(double value);
double reluDeriv(double value);

double sigmoidFunction(double value);
double sigmoidDeriv(double value);

double tanhFunction(double value);
double tanhDeriv(double value);

std::vector<double> softmaxFunction(const std::vector<double>& valueVector);