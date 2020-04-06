#pragma once

#include <vector>
#include <map>
#include <string>

#include "mdp.h"

enum mode_t { NOT_SET, VPITERATIONS, QLEARNING, SARSA };

class rl
{

private:

	mdp* environment;

	// VALUE&POLICY iterations
	std::map<state_t, action_t> state_policies;
	float gamma; // the "forgetting" factor
	float delta; // the difference between state value in each iteration
	
	// QLEARNING

public:

	rl(mdp* environment);
	enum mode_t mode;
	
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
	action_t getAction(const state_t& state);

	// FUNCTION APPROXIMATION
	std::map<feature_t, std::pair<float, float>> features_weights; // features with their weights

	bool rl::stepFA(state_t& state, bool reset);
	float getQValueFA(const state_t& state, const action_t& action);

};