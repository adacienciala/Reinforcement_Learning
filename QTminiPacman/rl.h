#pragma once

#include <vector>
#include <map>
#include <string>
#include <deque>

#include "mdp.h"
#include "neuralNetwork/neuralNetwork.h"

enum learningMode_t { NOT_SET, VPITERATIONS, QLEARNING, SARSA, FA, DQL };

struct memory_bit_t
{
	state_t state;
	action_t action;
	int reward;
	state_t next_state;
	bool done;
};

class rl
{

private:

	mdp* environment;
	float gamma; // the "forgetting" factor

	// VALUE&POLICY iterations
	std::map<state_t, action_t> state_policies;
	
	// DEEP QLEARNING
	int memory_capacity;
	std::deque<memory_bit_t> memoryDQL;
	neuralNetwork network;

public:

	rl(mdp* environment);
	enum learningMode_t mode;
	
	// VALUE&POLICY iterations
	int iterations;
	std::map<state_t, float> state_values; // state and its value

	float computeQValue(const state_t& state, const action_t& action);
	void runValueIteration();
	void runPolicyIteration();
	action_t getBestPolicy(const state_t& state) const;
	void clearStateValues();

	// QLEARNING + SARSA
	float epsilon; // the propability of taking a random action
	float alpha; // the learning rate
	int episodes;
	std::map<state_t, std::map<action_t, float>> state_QValues; // state and possible actions with values

	void runQLearning(const state_t& starting_state);
	bool stepQLearning(state_t& state);
	bool stepSarsa(state_t& state, bool reset);
	float computeValFromQVal(const state_t& state);
	action_t computeActionFromQVal(const state_t& state);
	float getQValue(const state_t& state, const action_t& action);
	action_t getAction(const state_t& state, learningMode_t mode);

	// FUNCTION APPROXIMATION
	std::map<feature_t, std::pair<float, float>> features_weights; // features with their weights

	void resetFeatureWeights();
    bool stepFA(state_t& state);
	float getQValueFA(const state_t& state, const action_t& action);
	float computeValFromQValFA(const state_t& state);
	action_t computeActionFromQValFA(const state_t& state);

	// DEEP QLEARNING
	void resetDQLNetwork();
	void replayDQL(int batch_size);
	bool stepDQL(state_t& state);
	float getQValueDQL(const state_t& state, const action_t& action);
	float computeValFromQValDQL(const state_t& state);
	action_t computeActionFromQValDQL(const state_t& state);

};
