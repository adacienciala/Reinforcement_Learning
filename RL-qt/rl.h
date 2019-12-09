#pragma once

#include <vector>
#include <map>
#include <QMainWindow>

class rl : public QObject
{
	Q_OBJECT

private:

	std::map<std::pair<int, int>, int> rewards;
	int width;
	int height;
	float noise;
	float gamma;

public:

	int iterations;
	std::map<std::pair<int, int>, float> state_values;

	rl(const std::map<std::pair<int, int>, int>& reward, int width, int height);
	void test();
	std::vector<std::pair<int, int>> getAllStates() const;
	std::vector<std::string> getPossibleActions(const std::pair<int, int>& state) const;
	std::vector<std::pair<int, int>> getNextStates(const std::pair<int, int>& state, const std::string& action) const;
	std::map<std::pair<std::string, std::pair<int, int>>, float> getTransitions(const std::pair<int, int>& state, const std::string& action) const;
	bool isTerminal(const std::pair<int, int>& state) const;
	bool isAvailable(const std::pair<int, int>& state, const std::string& action) const;
	float computeQValue(const std::pair<int, int>& state, const std::string& action);
	int getReward(const std::pair<int, int>& state);
	void runValueIteration();
	void clearState_Values();

};