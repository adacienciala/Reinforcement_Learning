#include <iostream>
#include <algorithm>
#include "rl.h"

rl::rl(const std::map<std::string, std::map<std::string, std::vector<prob_t>>> & map)
{
	this->environment.insert(map.begin(), map.end());

	const auto& allStates = getAllStates();
	for (const auto& state : allStates)
	{
		this->state_values[state] = 0.0;
	}

	this->gamma = 0.9f;
	this->iterations = 100;
};

std::vector<std::string> rl::getAllStates() const
{
	// iteration after all the possible states in the environment
	// if there's no states, returns an empty vector
	// otherwise returns a vector of strings 

	if (this->environment.empty()) return std::vector<std::string>();

	std::vector<std::string> AllStates;
	for (const auto& it : this->environment)
	{
		AllStates.push_back(it.first);
	}

	return AllStates;
};

std::vector<std::string> rl::getPossibleActions(const std::string& state) const
{
	// finds a given state in the map and gets all the possible actions from that state
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of strings 

	//std::map<std::string, std::map<std::string, std::vector<prob_t>>>::iterator it;

	const auto it = this->environment.find(state);

	if (it == this->environment.end()) return std::vector<std::string>();

	std::vector<std::string> possibleActions;
	for (const auto& action : it->second)
	{
		possibleActions.push_back(action.first);
	}

	return possibleActions;
};

std::vector<std::string> rl::getNextStates(const std::string& state, const std::string& action) const
{
	// gets Transitions for the given state and action and looks for all the next states
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of strings 

	std::vector<prob_t> transitions = getTransitions(state, action);
	if (transitions.empty())
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> nextStates;
	for (const auto& it : transitions)
	{
		nextStates.push_back(it.next_state);
	}

	return nextStates;
};

bool rl::isTerminal(const std::string& state)
{
	// returns 1 if the state is terminal (there's no possible actions)
	// otherwise returns 0

	if (getPossibleActions(state).empty()) return 1;
	else return 0;
};

std::vector<prob_t> rl::getTransitions(const std::string& state, const std::string& action) const
{
	// looks for the given action in the specified state
	// if said action is not possible, returns an empty vector
	// otherwise returns a vector of structs prob_t 

	std::vector<prob_t> rV;

	//bezpiecznik
	if (this->environment.count(state) == 0 || this->environment.at(state).count(action) == 0)
	{
		return rV;
	}
	rV = this->environment.at(state).at(action);
	//rV = this->environment[state][action];

	return rV;
};

int rl::getReward(const std::string& state, const std::string& action, const std::string& next_state)
{
	// gets Transitions for the given state and action and looks for the given next_state
	// returns reward for next_state
	// if wrong data, returns 0

	int reward = 0;
	std::vector<prob_t> transitions = getTransitions(state, action);
	if (transitions.empty()) return reward;

	//dwie opcje
	//albo iteracja
	//albo...
	//find_if to algorymt to wyszukiwania w kolekcji.
	//rozszerzony o mozliwosc definiowania lambda warunku wyszukiwania
	//a w ramach treningu zakomentuj to sobie poxniej i napisz normalnie, iterujac :3
	//to co zaznaczylem, to lambda wlasnie, czyli anonimowa funckja

	const auto searchingTrans = std::find_if(transitions.begin(), transitions.end(), [next_state](const prob_t& prb)
		{
			return prb.next_state == next_state;
		});

	if (searchingTrans != transitions.end())
	{
		reward = searchingTrans->reward;
	}

	return reward;
};

float rl::computeQValue(const std::string& state, const std::string& action)
{
	// compute Q(s, a) value
	// Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
	// returns value or 0, if the state is terminal

	float value = 0, prev_value = 0;

	std::vector<prob_t> transitions = getTransitions(state, action);
	for (const auto& it : transitions)
	{
		prev_value = this->state_values[it.next_state];
		value += it.probability * (it.reward + (this->gamma * prev_value));
	}

	return value;
};

void rl::runValueIteration()
{
	// creates a new states_values map to store new QValues 
	// for each state finds maximum QValue and saves it to the original map

	std::map<std::string, float> new_state_values;

	float QVal = 0, max = 0;

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<std::string> allStates = getAllStates();
		for (const auto& state : allStates)
		{
			std::vector<std::string> possibleActions = getPossibleActions(state);
			for (const auto& action : possibleActions)
			{
				QVal = computeQValue(state, action);
				if (action == possibleActions[0] || QVal > max)
				{
					max = QVal;
				}
			}
			new_state_values[state] = max;
		}
		this->state_values = new_state_values;
	}
};