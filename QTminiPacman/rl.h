#pragma once

#include <vector>
#include <map>
#include <string>

struct state_t
{
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

class rl
{

private:

	std::map<std::pair<int, int>, int> grid;
	std::map<state_t, std::string> state_policies;
	int width;
	int height;
	float gamma;
	int forbVal;

public:

	int iterations;
	std::map<state_t, float> state_values;
	rl(const std::map<std::pair<int, int>, int>& grid, int width, int height);
	void test();
	std::vector<state_t> getAllStates() const;
	std::vector<std::string> getPossibleActions(const state_t& state) const;
	std::vector<state_t> getNextStates(const state_t& state, const std::string& action) const;
	std::map<std::pair<std::string, state_t>, float> getTransitions(const state_t& state, const std::string& action) const;
	bool isTerminal(const state_t& state) const;
	bool isAvailable(const std::pair<int, int>& coordinate, const std::string& action) const;
	float computeQValue(const state_t& state, const std::string& action);
	int getReward(const state_t& state) const;
	void runValueIteration();
	void runPolicyIteration();
	std::string getBestPolicy(const state_t& state) const;
	void clearState_Values();

};