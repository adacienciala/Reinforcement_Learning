#pragma once

#include <vector>
#include <map>
#include <string>

#include "mdp.h"

class rl
{

private:

	std::map<state_t, action_t> state_policies;
	float gamma; //the "forgetting" factor
	float delta; //the difference between state value in each iteration
	mdp* environment;

public:

	int iterations;
	std::map<state_t, float> state_values;
	rl(mdp* environment);
	float computeQValue(const state_t& state, const action_t& action);
	void runValueIteration();
	void clearStateValues();
	void runPolicyIteration();
	action_t getBestPolicy(const state_t& state) const;

};