#include <iostream>
#include <algorithm>

#include "rl.h"

rl::rl(const std::map<std::pair<int, int>, int>& grid, int width, int height)
{
	this->height = height;
	this->width = width;
	this->grid = grid;
	this->gamma = 0.9f;
	this->iterations = 100;
	this->forbVal = -100;

	clearState_Values();
};

void rl::test()
{
	const auto& allStates = getAllStates();
	for (const auto& state : allStates)
	{
		const auto possibleActions = getPossibleActions(state);

		std::cout << "{ player(" << state.player.first << ", " << state.player.second << ")" << std::endl;
		std::cout << "  ghost(" << state.ghost.first << ", " << state.ghost.second << ")" << std::endl;
		std::cout << "  coin(" << state.coin.first << ", " << state.coin.second << ") } :" << std::endl;

		std::cout << "NEXT:" << std::endl;
		for (const auto& action : possibleActions)
		{
			std::cout << "\t* " << action << ":" << std::endl;
			const auto nextStates = getNextStates(state, action);
			for (const auto& next : nextStates)
			{
				std::cout << "\t\t{ player(" << next.player.first << ", " << next.player.second << ")" << std::endl;
				std::cout << "\t\t  ghost(" << next.ghost.first << ", " << next.ghost.second << ")" << std::endl;
				std::cout << "\t\t  coin(" << next.coin.first << ", " << next.coin.second << ") } :" << std::endl;
			}
		}
		std::cout << "TRANS:" << std::endl;
		for (const auto& action : possibleActions)
		{
			std::cout << "\t* " << action << ":" << std::endl;
			const auto transitions = getTransitions(state, action);
			for (const auto& trans : transitions)
			{
				std::cout << "\t\t-next:"<< std::endl;
				std::cout << "\t\t\t{ player(" << trans.first.second.player.first << ", " << trans.first.second.player.second << ")" << std::endl;
				std::cout << "\t\t\t  ghost(" << trans.first.second.ghost.first << ", " << trans.first.second.ghost.second << ")" << std::endl;
				std::cout << "\t\t\t  coin(" << trans.first.second.coin.first << ", " << trans.first.second.coin.second << ") } :" << std::endl;
				std::cout << "\t\t-val: " << trans.second << std::endl;
			}
		}
	}

	runValueIteration();
	for (const auto& state : this->state_values)
	{
		std::cout << "* player - " << state.first.player.first << state.first.player.second << std::endl;
		std::cout << "* ghost - " << state.first.ghost.first << state.first.ghost.second << std::endl;
		std::cout << "* coin - " << state.first.coin.first << state.first.coin.second << std::endl;
		std::cout << "* QVal - " << state.second << std::endl;
	}
};

std::vector<state_t> rl::getAllStates() const
{
	// iteration after all the possible states in the environment
	// if there's no states, returns an empty vector
	// otherwise returns a vector of pairs with coordinates (x, y)

	if (this->grid.empty()) return std::vector<state_t>();
	
	std::vector<state_t> allStates;
	
	for (const auto& state_player : this->grid)
	{
		for (const auto& state_ghost : this->grid)
		{
			if (state_player.second != this->forbVal && state_ghost.second != this->forbVal)
			{
				state_t tempState = { state_player.first, state_ghost.first, this->coin };
				allStates.push_back(tempState);
			}
		}
	}
	return allStates;

	return std::vector<state_t>();
};

std::vector<std::string> rl::getPossibleActions(const state_t& state) const
{
	// iterates for a given state in the vector and gets all the possible actions from that state
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of strings (you can request actions into walls)

	const auto& allStates = getAllStates();
	/*auto it = allStates.begin();
	for (; it < allStates.end(); ++it)
	{
		if (it->first == state.first && it->second == state.second) break;
	}
	if (it == allStates.end()) return std::vector<std::string>();*/

	const auto searchingState = std::find_if(allStates.begin(), allStates.end(), [state](const state_t& currentState)
		{
			return (currentState.player == state.player && currentState.ghost == state.ghost && currentState.coin == state.coin);
		});

	if (searchingState != allStates.end())
	{
		return { "north", "east", "south", "west" };
	}
	else return std::vector<std::string>();
};

std::vector<state_t> rl::getNextStates(const state_t& state, const std::string& action) const
{
	// gets Transitions for the given state and action and looks for all the next states
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of pairs (x, y)

	std::map<std::pair<std::string, state_t>, float> transitions = getTransitions(state, action);
	if (transitions.empty())
	{
		return std::vector<state_t>();
	}

	std::vector<state_t> nextStates;
	for (const auto& it : transitions)
	{
		nextStates.push_back(it.first.second);
	}

	return nextStates;
};

bool rl::isAvailable(const std::pair<int, int>& coordinate, const std::string& action) const
{
	// returns 1 if the given move is avaiable - it's not into a wall or into a forbidden state (reward equals forbVal)
	// otherwise, returns 0

	if (action == "north")
	{
		if (coordinate.second - 1 < 0 || this->grid.at(std::make_pair(coordinate.first, coordinate.second - 1)) == this->forbVal) return 0;
	}
	else if (action == "south")
	{
		if (coordinate.second + 1 >= this->height || this->grid.at(std::make_pair(coordinate.first, coordinate.second + 1)) == this->forbVal) return 0;
	}
	else if (action == "west")
	{
		if (coordinate.first - 1 < 0 || this->grid.at(std::make_pair(coordinate.first - 1, coordinate.second)) == this->forbVal) return 0;
	}
	else
	{
		if (coordinate.first + 1 >= this->width || this->grid.at(std::make_pair(coordinate.first + 1, coordinate.second)) == this->forbVal) return 0;
	}

	return 1;
}

bool rl::isTerminal(const state_t& state) const
{
	// returns 1 if the state is terminal (there's a reward)
	// otherwise returns 0

	if (getReward(state)) return 1;
	else return 0;
};

std::map<std::pair<std::string, state_t>, float> rl::getTransitions(const state_t& state, const std::string& action) const
{
	// looks for the given action in the specified state
	// if terminal state or unknown state, returns an empty vector
	// otherwise returns a map of pairs {action, nextState(x, y)} with probabilities 

	if (isTerminal(state) || isAvailable(state.player, action) == 0) return std::map<std::pair<std::string, state_t>, float>();
	const auto& allStates = getAllStates();
	auto it = allStates.begin();
	for (; it < allStates.end(); ++it)
	{
		if (it->player == state.player && it->ghost == state.ghost) break;
	}
	if (it == allStates.end()) return std::map<std::pair<std::string, state_t>, float>();

	std::map<std::pair<std::string, state_t>, float> transitions;

	// determing player's (x, y)
	std::pair<int, int> northPlayer = !isAvailable(state.player, "north") ? state.player : std::make_pair(state.player.first, state.player.second - 1);
	std::pair<int, int> southPlayer = !isAvailable(state.player, "south") ? state.player : std::make_pair(state.player.first, state.player.second + 1);
	std::pair<int, int> eastPlayer = !isAvailable(state.player, "east") ? state.player : std::make_pair(state.player.first + 1, state.player.second);
	std::pair<int, int> westPlayer = !isAvailable(state.player, "west") ? state.player : std::make_pair(state.player.first - 1, state.player.second);

	std::pair<int, int> tempPlayer;
	if (action == "east")
	{
		tempPlayer = eastPlayer;
	}
	else if (action == "west")
	{
		tempPlayer = westPlayer;
	}
	else if (action == "north")
	{
		tempPlayer = northPlayer;
	}
	else
	{
		tempPlayer = southPlayer;
	}

	// determing ghost's possible (x, y)s
	std::pair<int, int> northGhost = !isAvailable(state.ghost, "north") ? state.ghost : std::make_pair(state.ghost.first, state.ghost.second - 1);
	std::pair<int, int> southGhost = !isAvailable(state.ghost, "south") ? state.ghost : std::make_pair(state.ghost.first, state.ghost.second + 1);
	std::pair<int, int> eastGhost = !isAvailable(state.ghost, "east") ? state.ghost : std::make_pair(state.ghost.first + 1, state.ghost.second);
	std::pair<int, int> westGhost = !isAvailable(state.ghost, "west") ? state.ghost : std::make_pair(state.ghost.first - 1, state.ghost.second);

	std::vector<std::pair<int, int>> tempGhost = { northGhost, southGhost, eastGhost, westGhost };

	// sorting and deleting the duplicates
	std::sort(tempGhost.begin(), tempGhost.end());
	tempGhost.erase(std::unique(tempGhost.begin(), tempGhost.end()), tempGhost.end());

	for (const auto& moveGhost : tempGhost)
	{
		std::pair<std::string, state_t> tempPair = { action, { tempPlayer, moveGhost, state.coin } };
		transitions[tempPair] = 1.0f / tempGhost.size();
	}

	return transitions;
};

int rl::getReward(const state_t& state) const
{
	// returns reward for the given state
	// if wrong data, returns 0

	if (state.player == state.ghost) return -10;
	if (state.player == state.coin) return 10;
	
	return 0;
};

float rl::computeQValue(const state_t& state, const std::string& action)
{
	// compute Q(s, a) value
	// Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
	// returns value or 0, if the state is terminal

	float value = 0, prev_value = 0;

	std::map<std::pair<std::string, state_t>, float> transitions = getTransitions(state, action);
	for (const auto& it : transitions)
	{
		prev_value = this->state_values[it.first.second];
		value += it.second * (getReward(state) + (this->gamma * prev_value));
	}

	return value;
};

void rl::runValueIteration()
{
	// creates a new states_values map to store new QValues 
	// for each state finds maximum QValue and saves it to the original map

	std::map<state_t, float> new_state_values;

	float QVal = 0, max = 0;

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<state_t> allStates = getAllStates();
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
			if (isTerminal(state)) max = (float)getReward(state);
			new_state_values[state] = max;
		}
		this->state_values = new_state_values;
	}
}

void rl::runPolicyIteration()
{
	// runs over all the states and finds the best policy for each of them
	// saves the best actions as a map <state_t, string>
	
	std::map<state_t, std::string> new_state_policies;

	float QVal = 0, max = 0;
	std::string best_action = "stop";

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<state_t> allStates = getAllStates();
		for (const auto& state : allStates)
		{
			std::vector<std::string> possibleActions = getPossibleActions(state);
			for (const auto& action : possibleActions)
			{
				QVal = computeQValue(state, action);
				if (action == possibleActions[0] || QVal > max)
				{
					max = QVal;
					best_action = action;
				}
			}
			if (isTerminal(state))
			{
				max = (float)getReward(state);
				best_action = "stop";
			}
			new_state_policies[state] = best_action;
		}
		this->state_policies = new_state_policies;
	}
}

std::string rl::getBestPolicy(const state_t& state) const
{
	// returns the best action from the given state

	return this->state_policies.at(state);
}

void rl::clearState_Values()
{
	const auto& allStates = getAllStates();
	for (auto& state : allStates)
	{
		this->state_values[state] = 0.0;
	}
}
