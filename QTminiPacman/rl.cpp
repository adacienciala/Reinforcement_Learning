#include <iostream>
#include <algorithm>

#include "rl.h"

rl::rl(mdp* environment)
{
	this->gamma = 0.9f;
	this->iterations = 100;
	this->delta = 0.05f;
	this->environment = environment;
};

float rl::computeQValue(const state_t& state, const action_t& action)
{
	// compute Q(s, a) value
	// Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
	// returns value or 0, if the state is terminal

	float value = 0, prev_value = 0;

	std::map<std::pair<action_t, state_t>, float> transitions = this->environment->getTransitions(state, action);
	for (const auto& it : transitions)
	{
		prev_value = this->state_values[it.first.second];
		value += it.second * (this->environment->getReward(state) + (this->gamma * prev_value));
	}

	return value;
};

void rl::runValueIteration()
{
	// creates a new states_values map to store new QValues 
	// for each state finds maximum QValue
	// saves the best actions as a map <state_t, float>

	clearStateValues();
	std::map<state_t, float> new_state_values;

	float QVal = 0, max = 0;

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<state_t> allStates = this->environment->getAllStates();
		for (const auto& state : allStates)
		{
			std::vector<action_t> possibleActions = this->environment->getPossibleActions(state);
			for (const auto& action : possibleActions)
			{
				QVal = computeQValue(state, action);
				if (action == possibleActions[0] || QVal > max)
				{
					max = QVal;
				}
			}
			if (this->environment->isTerminal(state)) max = (float)this->environment->getReward(state);
			new_state_values[state] = max;
		}
		this->state_values = new_state_values;
	}
}

void rl::clearStateValues()
{
	const auto allStates = this->environment->getAllStates();
	for (auto& state : allStates)
	{
		this->state_values[state] = 0.0;
	}
}

void rl::runPolicyIteration()
{
	// creates a new new_state_policies map to store new policies 
	// for each state finds maximum QValue and saves it's action
	// saves the best actions as a map <state_t, action_t>
	
	std::map<state_t, action_t> new_state_policies;

	float QVal = 0, max = 0;
	action_t best_action = STAY;

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<state_t> allStates = this->environment->getAllStates();
		for (const auto& state : allStates)
		{
			std::vector<action_t> possibleActions = this->environment->getPossibleActions(state);
			for (const auto& action : possibleActions)
			{
				QVal = computeQValue(state, action);
				if (action == possibleActions[0] || QVal > max)
				{
					max = QVal;
					best_action = action;
				}
			}
			if (this->environment->isTerminal(state))
			{
				max = (float)this->environment->getReward(state);
				best_action = STAY;
			}
			new_state_policies[state] = best_action;
		}
		this->state_policies = new_state_policies;
	}
}

action_t rl::getBestPolicy(const state_t& state) const
{
	// returns the best action from the given state

	return this->state_policies.at(state);
}
