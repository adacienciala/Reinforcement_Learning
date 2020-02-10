#pragma once

#include <vector>
#include <map>
#include <string>

#include "mdp.h"

class rl
{

private:

	std::map<state_t, action_t> state_policies;
	float gamma; // the "forgetting" factor
	float delta; // the difference between state value in each iteration
	float epsilon; // the propability of taking a random action
	float alpha; // the learning rate
	mdp* environment;

public:

	int iterations;
	int episodes;
	std::map<state_t, float> state_values;

	rl(mdp* environment);

	float computeQValue(const state_t& state, const action_t& action);
	void runValueIteration();
	void runPolicyIteration();
	action_t getBestPolicy(const state_t& state) const;
	void clearStateValues();

	void runQLearning(const state_t& starting_state);
	bool rl::stepQLearning(state_t& state);
	float computeValFromQVal(const state_t& state);
	action_t computeActionFromQVal(const state_t& state);
	float getQValue(const state_t& state, const action_t& action);
	action_t getAction(const state_t& state, bool player = true);

};