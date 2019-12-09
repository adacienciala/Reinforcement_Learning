#include <iostream>
#include <algorithm>
#include "rl.h"

rl::rl(const std::map<std::pair<int, int>, int>& reward, int width, int height)
{
	this->height = height;
	this->width = width;
	this->rewards = reward;
	this->noise = 0.2f;
	this->gamma = 0.9f;
	this->iterations = 0;

	clearState_Values();
};

void rl::test() 
{
	const auto allStates = getAllStates();
	for (const auto& state : allStates)
	{
		const auto possibleActions = getPossibleActions(state);
		std::cout << "(" << state.first << ", " << state.second << "):" << std::endl << "NEXT:" << std::endl;
		for (const auto& action : possibleActions)
		{
			std::cout << "\t* " << action << ":" << std::endl;
			const auto nextStates = getNextStates(state, action);
			for (const auto& next : nextStates)
			{
				std::cout << "\t\t- " << "(" << next.first << ", " << next.second << ")" << std::endl;
			}
		}
		std::cout << "TRANS:" << std::endl;
		for (const auto& action : possibleActions)
		{
			std::cout << "\t* " << action << ":" << std::endl;
			const auto transitions = getTransitions(state, action);
			for (const auto& trans : transitions)
			{
				std::cout << "\t\t-action taken: " << trans.first.first;
				std::cout << " -next: (" << trans.first.second.first << ", " << trans.first.second.second << ")" << std::endl;
				std::cout << " -val: " << trans.second << std::endl;
			}
		}
	}

	runValueIteration();
	for (const auto& state : this->state_values)
	{
		std::cout << state.first.first << state.first.second << " " << state.second << std::endl;
	}
}

std::vector<std::pair<int, int>> rl::getAllStates() const
{
	// iteration after all the possible states in the environment
	// if there's no states, returns an empty vector
	// otherwise returns a vector of pairs with coordinates (x, y)

	if (this->rewards.empty()) return std::vector<std::pair<int, int>>();
	
	std::vector<std::pair<int, int>> AllStates;
	for (const auto& coordinate : this->rewards)
	{
		if (coordinate.second != -10)
		{
			std::pair<int, int> tempPair = { coordinate.first.first, coordinate.first.second };
			AllStates.push_back(tempPair);
		}
	}

	return AllStates;
};

std::vector<std::string> rl::getPossibleActions(const std::pair<int, int>& state) const
{
	// iterates for a given state in the vector and gets all the possible actions from that state
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of strings (you  can request actions into walls)

	const auto allStates = getAllStates();
	auto it = allStates.begin();
	for (; it < allStates.end(); ++it)
	{
		if (it->first == state.first && it->second == state.second) break;
	}
	if (it == allStates.end()) return std::vector<std::string>();

	return { "north", "east", "south", "west" };
};

std::vector<std::pair<int, int>> rl::getNextStates(const std::pair<int, int>& state, const std::string& action) const
{
	// gets Transitions for the given state and action and looks for all the next states
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of pairs (x, y)

	std::map<std::pair<std::string, std::pair<int, int>>, float> transitions = getTransitions(state, action);
	if (transitions.empty())
	{
		return std::vector<std::pair<int, int>>();
	}

	std::vector<std::pair<int, int>> nextStates;
	for (const auto& it : transitions)
	{
		nextStates.push_back(it.first.second);
	}

	return nextStates;
};

bool rl::isAvailable(const std::pair<int, int>& state, const std::string& action) const
{
	// returns 1 if the given state is avaiable - it's not a wall or a forbidden state (reward -10)
	// otherwise, returns 0

	if (action == "north")
	{
		if (state.second - 1 < 0 || this->rewards.at(std::make_pair(state.first, state.second - 1)) == -10) return 0;
	}
	else if (action == "south")
	{
		if (state.second + 1 >= this->height || this->rewards.at(std::make_pair(state.first, state.second + 1)) == -10) return 0;
	}
	else if (action == "west")
	{
		if (state.first - 1 < 0 || this->rewards.at(std::make_pair(state.first - 1, state.second)) == -10) return 0;
	}
	else
	{
		if (state.first + 1 >= this->width || this->rewards.at(std::make_pair(state.first + 1, state.second)) == -10) return 0;
	}

	return 1;
}

bool rl::isTerminal(const std::pair<int, int>& state) const
{
	// returns 1 if the state is terminal (there's a reward)
	// otherwise returns 0

	if (this->rewards.at(state) == 1 || this->rewards.at(state) == -1) return 1;
	else return 0;
};

std::map<std::pair<std::string, std::pair<int, int>>, float> rl::getTransitions(const std::pair<int, int>& state, const std::string& action) const
{
	// looks for the given action in the specified state
	// if terminal state or unknown state, returns an empty vector
	// otherwise returns a map of pairs {action, nextState(x, y)} with probabilities 

	if (isTerminal(state)) return std::map<std::pair<std::string, std::pair<int, int>>, float>();
	const auto allStates = getAllStates();
	auto it = allStates.begin();
	for (; it < allStates.end(); ++it)
	{
		if (it->first == state.first && it->second == state.second) break;
	}
	if (it == allStates.end()) return std::map<std::pair<std::string, std::pair<int, int>>, float>();

	std::map<std::pair<std::string, std::pair<int, int>>, float> transitions;

	std::pair<int, int> northState = !isAvailable(state, "north") ? state : std::make_pair(state.first, state.second - 1);
	std::pair<int, int> southState = !isAvailable(state, "south") ? state : std::make_pair(state.first, state.second + 1);
	std::pair<int, int> eastState = !isAvailable(state, "east") ? state : std::make_pair(state.first + 1, state.second);
	std::pair<int, int> westState = !isAvailable(state, "west") ? state : std::make_pair(state.first - 1, state.second);

	std::pair<std::string, std::pair <int, int>> tempPair;
	if (action == "east" || action == "west")
	{
		if (action == "east")
		{
			tempPair = { action, eastState };
		}
		else
		{
			tempPair = { action, westState };
		}
		transitions[tempPair] = 1 - this->noise;

		tempPair = { "north", northState };
		transitions[tempPair] = this->noise / 2.0f;
		tempPair = { "south", southState };
		transitions[tempPair] = this->noise / 2.0f;
	}

	if (action == "north" || action == "south")
	{
		if (action == "north")
		{
			tempPair = { action, northState };
		}
		else
		{
			tempPair = { action, southState };
		}
		transitions[tempPair] = 1 - this->noise;

		tempPair = { "east", eastState };
		transitions[tempPair] = this->noise / 2.0f;
		tempPair = { "west", westState };
		transitions[tempPair] = this->noise / 2.0f;
	}

	return transitions;
};

int rl::getReward(const std::pair<int, int>& state)
{
	// returns reward for the given state
	// if wrong data, returns 0

	const auto allStates = getAllStates();
	if (allStates.empty()) return 0;

	for (const auto& curState : allStates)
	{
		if (curState.first == state.first && curState.second == state.second) return this->rewards[curState];
	}
	 
	//const auto searchingState = std::find_if(this->state_values.begin(), this->state_values.end(), [state](const auto& currentState)
	//	{
	//		return (currentState.first.first == stateTemp.first && currentState.first.second == stateTemp.second);
	//	});

	//if (searchingState != this->state_values.end())
	//{
	//	//reward = searchingState->second;
	//}
	
	
	return 0;
};

float rl::computeQValue(const std::pair<int, int>& state, const std::string& action)
{
	// compute Q(s, a) value
	// Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
	// returns value or 0, if the state is terminal

	float value = 0, prev_value = 0;

	std::map<std::pair<std::string, std::pair<int, int>>, float> transitions = getTransitions(state, action);
	for (const auto& it : transitions)
	{
		prev_value = this->state_values[it.first.second];
		value += it.second * (this->rewards[state] + (this->gamma * prev_value));
	}

	return value;
};

void rl::runValueIteration()
{
	// creates a new states_values map to store new QValues 
	// for each state finds maximum QValue and saves it to the original map

	std::map<std::pair<int, int>, float> new_state_values;

	float QVal = 0, max = 0;

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<std::pair<int, int>> allStates = getAllStates();
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
			if (isTerminal(state)) max = getReward(state);
			new_state_values[state] = max;
		}
		this->state_values = new_state_values;
	}
}

void rl::clearState_Values()
{
	const auto allStates = getAllStates();
	for (auto& state : allStates)
	{
		this->state_values[state] = 0.0;
	}
}
