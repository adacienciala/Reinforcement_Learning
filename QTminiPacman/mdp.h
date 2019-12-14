#pragma once

#include <vector>
#include <map>
#include <string>

enum action_t { NORTH, EAST, SOUTH, WEST, STAY };

struct state_t
{
	std::pair<int, int> player;
	std::pair<int, int> ghost;

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
				}
			}
		}
		return false;
	}
};

class mdp
{

private:

	std::map<std::pair<int, int>, int> grid;
	state_t cur_state;
	int width;
	int height;
	float gamma;
	int forbVal;
	
public:
	mdp(const std::map<std::pair<int, int>, int>& grid, int width, int height, std::pair<int, int> coin);

	std::pair<int, int> coin;
	std::vector<state_t> getAllStates() const;
	std::vector<action_t> getPossibleActions(const state_t& state) const;
	std::vector<state_t> getNextStates(const state_t& state, action_t action) const;
	std::map<std::pair<action_t, state_t>, float> getTransitions(const state_t& state, action_t action) const;
	bool isTerminal(const state_t& state) const;
	bool isAvailable(const std::pair<int, int>& coordinate, action_t action) const;
	int getReward(const state_t& state) const;

};

