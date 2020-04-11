#pragma once

#include <vector>
#include <map>
#include <string>

enum action_t { NORTH, EAST, SOUTH, WEST, STAY };
enum feature_t { FOOD_DISTANCE, GHOST_DISTANCE };

struct state_t
{
	// a STATE is described by 3 factors:
	// - coordinates of the player
	// - coordinates of the ghost
	// - coordinates of the coin
	// so a state is, in some way, a "screenshoot" of the game

	std::pair<int, int> player;
	std::pair<int, int> ghost;
	std::pair<int, int> coin;

	bool operator< (const state_t& a) const
	{
		if (this->player.first < a.player.first) return true;
		if (this->player.first == a.player.first)
		{
			if (this->player.second < a.player.second) return true;
			if (this->player.second == a.player.second)
			{
				if (this->ghost.first < a.ghost.first) return true;
				if (this->ghost.first == a.ghost.first)
				{
					if (this->ghost.second < a.ghost.second) return true;
					if (this->ghost.second == a.ghost.second)
					{
						if (this->coin.first < a.coin.first) return true;
						if (this->coin.first == a.coin.first)
						{
							if (this->coin.second < a.coin.second) return true;
						}
					}
				}
			}
		}
		return false;
	}
};

class mdp
{

private:

	// GRID - map of < (x, y), WALL/EMPTY >
	std::map<std::pair<int, int>, int> grid;
	state_t cur_state;
	float gamma;
	int forbVal;
	
public:

	int width;
	int height;
	mdp(const std::map<std::pair<int, int>, int>& grid, int width, int height);

	// VALUE and POLICY iterations' methods
	std::vector<state_t> getAllStates() const;
	std::vector<action_t> getPossibleActions(const state_t& state) const;
	std::vector<state_t> getNextStates(const state_t& state, action_t action) const;
	std::map<std::pair<action_t, state_t>, float> getTransitions(const state_t& state, action_t action) const;
	bool isTerminal(const state_t& state) const;
	bool isAvailable(const std::pair<int, int>& coordinate, action_t action) const;
	int getReward(const state_t& state) const;

	// QLEARNING's methods
	std::vector<action_t> getPossibleActionsQLearning(const std::pair<int, int>& coords, bool player) const;
	std::tuple<state_t, int, bool> makeQStep(const state_t& cur_state, const action_t& actionPlayer);
    action_t randomAction(const std::pair<int, int>& coords, bool player);

};

