#include <algorithm>
#include "mdp.h"

mdp::mdp(const std::map<std::pair<int, int>, int>& grid, int width, int height)
{
	this->height = height;
	this->width = width;
	this->grid = grid;
	this->gamma = 0.9f;
	this->forbVal = -100;
}

std::vector<state_t> mdp::getAllStates() const
{
	// iteration after the grid to get all the states in the environment
	// if there's no grid, returns an empty vector
	// otherwise returns a vector of structures { player(x, y), ghost(x, y) }

	if (this->grid.empty()) return std::vector<state_t>();

	std::vector<state_t> allStates;
	for (const auto& state_coin : this->grid)
	{
		for (const auto& state_player : this->grid)
		{
			for (const auto& state_ghost : this->grid)
			{
				if (state_player.second != this->forbVal && state_ghost.second != this->forbVal && state_coin.second != this->forbVal)
				{
					state_t tempState = { state_player.first, state_ghost.first, state_coin.first };
					allStates.push_back(tempState);
				}
			}
		}
	}
	return allStates;
}

std::vector<action_t> mdp::getPossibleActions(const state_t& state) const
{
	// finds the given state in the vector and gets all the possible actions from that state
	// if there's no possible actions, returns an empty vector
	// otherwise returns a vector of enums (you can request actions into walls, too)

	const auto& allStates = getAllStates();
	
	const auto searchingState = std::find_if(allStates.begin(), allStates.end(), [state](const state_t& currentState)
		{
			return (currentState.player == state.player && currentState.ghost == state.ghost);
		});

	if (searchingState != allStates.end())
	{
		return { NORTH, EAST, SOUTH, WEST };
	}
	else return std::vector<action_t>();
}

std::vector<state_t> mdp::getNextStates(const state_t& state, action_t action) const
{
	// gets Transitions for the given state and action and looks for all the next states
	// if there's no possible actions (terminal state), returns an empty vector
	// otherwise returns a vector of pairs structures { player(x, y), ghost(x, y) }

	std::map<std::pair<action_t, state_t>, float> transitions = getTransitions(state, action);
	if (transitions.empty())
	{
		return std::vector<state_t>();
	}

	std::vector<state_t> nextStates;
	for (const auto& [action_and_state, probability] : transitions)
	{
		nextStates.push_back(action_and_state.second);
	}

	return nextStates;
}

std::map<std::pair<action_t, state_t>, float> mdp::getTransitions(const state_t& state, action_t action) const
{
	// looks for the given state, based on the action determines player's coordinations
	// calculates possible ghost's moves, deletes duplicates (so it's action is STAY)
	// if terminal or unknown state, returns an empty vector
	// otherwise returns a map of pairs {action, nextState(x, y)} with probabilities 

	if (isTerminal(state) || isAvailable(state.player, action) == 0) return std::map<std::pair<action_t, state_t>, float>();
	
	const auto& allStates = getAllStates();
	auto it = allStates.begin();
	for (; it < allStates.end(); ++it)
	{
		if (it->player == state.player && it->ghost == state.ghost) break;
	}
	if (it == allStates.end()) return std::map<std::pair<action_t, state_t>, float>();

	std::map<std::pair<action_t, state_t>, float> transitions;

	// determing player's (x, y)
	std::pair<int, int> northPlayer = !isAvailable(state.player, NORTH) ? state.player : std::make_pair(state.player.first, state.player.second - 1);
	std::pair<int, int> southPlayer = !isAvailable(state.player, SOUTH) ? state.player : std::make_pair(state.player.first, state.player.second + 1);
	std::pair<int, int> eastPlayer = !isAvailable(state.player, EAST) ? state.player : std::make_pair(state.player.first + 1, state.player.second);
	std::pair<int, int> westPlayer = !isAvailable(state.player, WEST) ? state.player : std::make_pair(state.player.first - 1, state.player.second);

	std::pair<int, int> tempPlayer;
	switch (action)
	{
	case EAST:
		tempPlayer = eastPlayer;
		break;
	case WEST:
		tempPlayer = westPlayer;
		break;
	case NORTH:
		tempPlayer = northPlayer;
		break;
	case SOUTH:
		tempPlayer = southPlayer;
	}

	// determing ghost's possible (x, y)s
	std::pair<int, int> northGhost = !isAvailable(state.ghost, NORTH) ? state.ghost : std::make_pair(state.ghost.first, state.ghost.second - 1);
	std::pair<int, int> southGhost = !isAvailable(state.ghost, SOUTH) ? state.ghost : std::make_pair(state.ghost.first, state.ghost.second + 1);
	std::pair<int, int> eastGhost = !isAvailable(state.ghost, EAST) ? state.ghost : std::make_pair(state.ghost.first + 1, state.ghost.second);
	std::pair<int, int> westGhost = !isAvailable(state.ghost, WEST) ? state.ghost : std::make_pair(state.ghost.first - 1, state.ghost.second);

	std::vector<std::pair<int, int>> tempGhost = { northGhost, southGhost, eastGhost, westGhost };

	// sorting and deleting the duplicates
	std::sort(tempGhost.begin(), tempGhost.end());
	tempGhost.erase(std::unique(tempGhost.begin(), tempGhost.end()), tempGhost.end());

	for (const auto& moveGhost : tempGhost)
	{
		std::pair<action_t, state_t> tempPair = { action, { tempPlayer, moveGhost } };
		transitions[tempPair] = 1.0f / tempGhost.size();
	}

	return transitions;
}

bool mdp::isTerminal(const state_t& state) const
{
	// returns 1 if the state is terminal: there's a reward +10 (reaching the coin)  or -10 (player's death)
	// otherwise returns 0

	if (getReward(state)) return 1;
	else return 0;
}

bool mdp::isAvailable(const std::pair<int, int>& coordinate, action_t action) const
{
	// returns 1 if the given action is available - it's not outside the grid or into a forbidden state (grid value equals forbVal)
	// otherwise, returns 0

	switch (action)
	{
	case NORTH:
		if (coordinate.second - 1 < 0 || this->grid.at(std::make_pair(coordinate.first, coordinate.second - 1)) == this->forbVal) return false;
		break;
	case SOUTH:
		if (coordinate.second + 1 >= this->height || this->grid.at(std::make_pair(coordinate.first, coordinate.second + 1)) == this->forbVal) return false;
		break;
	case WEST:
		if (coordinate.first - 1 < 0 || this->grid.at(std::make_pair(coordinate.first - 1, coordinate.second)) == this->forbVal) return false;
		break;
	case EAST:
		if (coordinate.first + 1 >= this->width || this->grid.at(std::make_pair(coordinate.first + 1, coordinate.second)) == this->forbVal) return false;
		break;
	}
	return true;
}

int mdp::getReward(const state_t& state) const
{
	// returns reward for the given state
	// +10 (reaching the coin)  or -10 (player's death)
	// if wrong data, returns 0

	if (state.player == state.ghost) return -10;
	if (state.player == state.coin) return 10;

	return 0;
}