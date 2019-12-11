#pragma once

#include <vector>
#include <map>
#include <QMainWindow>

struct state_t
{
	std::pair<int, int> player;
	std::pair<int, int> ghost;
	std::pair<int, int> coin;
};

class rl : public QObject
{
	Q_OBJECT

private:

	std::map<std::pair<int, int>, int> rewards;
	int width;
	int height;
	float gamma;
	int forbVal;

public:

	int iterations;
	std::map<state_t, float> state_values;
	rl(const std::map<std::pair<int, int>, int>& reward, int width, int height);
	void test();
	std::vector<state_t> getAllStates() const;
	std::vector<std::string> getPossibleActions(const state_t& state) const;
	std::vector<state_t> getNextStates(const state_t& state, const std::string& action) const;
	std::map<std::pair<std::string, state_t>, float> getTransitions(const state_t& state, const std::string& action) const;
	bool isTerminal(const state_t& state) const;
	bool isAvailable(const std::pair<int, int>& coordinate, const std::string& action) const;
	float computeQValue(const state_t& state, const std::string& action);
	int getReward(const state_t& state);
	void runValueIteration();
	void clearState_Values();

};