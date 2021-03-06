#include <algorithm>
#include "mdp.h"

mdp::mdp(const std::map<std::pair<int, int>, int>& grid, int width, int height)
{
	this->height = height;
	this->width = width;
	this->grid = grid;
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
	// returns a vector of enums (you can request actions into walls, too)
	// gonna have to refactor that cause it's extremely stupid

	return { NORTH, EAST, SOUTH, WEST };
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
	// calculates possible ghost's moves (when forbidden action, returns STAY), deletes duplicates 
	// if terminal or unknown state, returns an empty vector
	// otherwise returns a map of pairs {action, nextState(x, y)} with probabilities 

	if (isTerminal(state) || isAvailable(state.player, action) == 0) return std::map<std::pair<action_t, state_t>, float>();
	
	const auto& allStates = getAllStates();
	auto it = allStates.begin();
	for (; it < allStates.end(); ++it)
	{
		if (it->player == state.player && it->ghost == state.ghost && it->coin == state.coin) break;
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
		std::pair<action_t, state_t> tempPair = { action, { tempPlayer, moveGhost, state.coin } };
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

std::vector<action_t> mdp::getPossibleActionsQLearning(const std::pair<int, int>& coords, bool player) const
{
	// returns possible actions - not into walls
	// additionally, the ghost have an option to STAY

	std::vector<action_t> possibleActions = std::vector<action_t>();

	for (auto& action : { NORTH, EAST, SOUTH, WEST })
	{
		if (this->isAvailable(coords, action) == true)
		{
			possibleActions.push_back(action);
		}
	}
	if (player == false) possibleActions.push_back(STAY);

	return possibleActions;
}

std::tuple<state_t, int, bool> mdp::makeQStep(const state_t& cur_state, const action_t& actionPlayer)
{
	// makes a QLearning step
	// returns the next_state, reward for the state and if it's terminal

	state_t next_state = cur_state;

	switch (actionPlayer)
	{
	case NORTH:
		next_state.player.second -= 1;
		break;
	case SOUTH:
		next_state.player.second += 1;
		break;
	case EAST:
		next_state.player.first += 1;
		break;
	case WEST:
		next_state.player.first -= 1;
		break;
	}

	bool is_terminal = isTerminal(next_state);
	if (is_terminal == true)
	{
		int reward = getReward(next_state);
		return std::make_tuple(next_state, reward, is_terminal);
	}

	action_t actionGhost = randomAction(next_state.ghost, false);

	switch (actionGhost)
	{
	case NORTH:
		next_state.ghost.second -= 1;
		break;
	case SOUTH:
		next_state.ghost.second += 1;
		break;
	case EAST:
		next_state.ghost.first += 1;
		break;
	case WEST:
		next_state.ghost.first -= 1;
		break;
	}

	int reward = getReward(next_state);
	is_terminal = isTerminal(next_state);

	return std::make_tuple(next_state, reward, is_terminal);
}

action_t mdp::randomAction(const std::pair<int, int>& coords, bool player)
{
	// returns random of all possible actions
	// the ghost has an additional STAY action

	std::vector<action_t> possibleActions = this->getPossibleActionsQLearning(coords, player);
	return possibleActions[rand() % possibleActions.size()];
}

std::vector<double> mdp::getViewAsVector(const state_t& state)
{
	// returns the walls, player, ghost and food from grid with specific values as a vector
	// empty	-	-0.1
	// wall		-	-1.0
	// player	-	 0.2
	// ghost	-	-0.5
	// coin		-	 0.5

	std::vector<double> data;
	data.reserve(width * height);

	std::map<std::pair<int, int>, int> gridCopy = this->grid;

	gridCopy[state.player] = 20;
	gridCopy[state.ghost] = -100;
	gridCopy[state.coin] = 70;

	for (auto& [position, value] : gridCopy)
	{
		if (value) data.push_back((double)value / 100.0f);
		else data.push_back(-0.2f); 
	}

	return data;
}