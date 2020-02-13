#pragma once

#include <vector>
#include <map>
#include <string>

#include "mdp.h"

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
	
	// VALUE&POLICY iterations
	int iterations;
	std::map<state_t, float> state_values; // state and its value

	float computeQValue(const state_t& state, const action_t& action);
	void runValueIteration();
	void runPolicyIteration();
	action_t getBestPolicy(const state_t& state) const;
	void clearStateValues();

	// QLEARNING
	float epsilon; // the propability of taking a random action
	float alpha; // the learning rate
	int episodes;
	std::map<state_t, std::map<action_t, float>> state_QValues; // state and possible actions with values

	void runQLearning(const state_t& starting_state);
	bool rl::stepQLearning(state_t& state);
	float computeValFromQVal(const state_t& state);
	action_t computeActionFromQVal(const state_t& state);
	float getQValue(const state_t& state, const action_t& action);
	action_t getAction(const state_t& state, bool player = true);

};