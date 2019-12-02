#pragma once

//musisz dac include tutaj, bo inaczej nie kompliator nie bedzie ogarnial nazw
//jezeli dasz je tutaj, to w cpp juz nie musisz. 
//Ale pamietaj, ze tutaj powinny byc tylko te potrzebne w naglowku, a reszta w cpp. 
//Im mniej jest w h includow tym lepiej
#include <map>
#include <vector>
#include <list>

struct prob_t
{
	std::string next_state;
	float probability;
	int reward;
};

class rl
{
private:

	std::map<std::string, std::map<std::string, std::vector<prob_t>>> environment;
	float gamma;
	int iterations;

public:

	std::map<std::string, float> state_values;
	rl(const std::map<std::string, std::map<std::string, std::vector<prob_t>>>& map);
	std::vector<std::string> getAllStates() const;
	std::vector<std::string> getPossibleActions(const std::string& state) const;
	std::vector<std::string> getNextStates(const std::string& state, const std::string& action) const;
	std::vector<prob_t> getTransitions(const std::string& state, const std::string& action) const;
	bool isTerminal(const std::string& state);
	float computeQValue(const std::string& state, const std::string& action);
	int getReward(const std::string& state, const std::string& action, const std::string& next_state);
	void runValueIteration();
};