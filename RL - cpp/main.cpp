#include <iostream>

#include "rl.h"


int main()
{
	std::map<std::string, std::map<std::string, std::vector<prob_t> > > env =
	{
		  {"s0", {
			  {"a0", {
				  {"s0", 0.5f, 0},
				  {"s2", 0.5f, 0}}},
			  {"a1", {
				  {"s2", 1, 0 }}}}},
		  {"s1", {
			  {"a0", {
				  {"s0", 0.7f, 5},
				  {"s1", 0.1f, 0},
				  {"s2", 0.2f, 0}}},
			  {"a1", {
				  {"s0", 0.95f, 0},
				  {"s2", 0.05f, 0}}}}},
		  {"s2", {
			  {"a0", {
				  {"s0", 0.4f, 0},
				  {"s2", 0.6f, 0}}},
			  {"a1", {
				  {"s0", 0.3f, -1},
				  {"s1", 0.3f, 0},
				  {"s2", 0.4f, 0}}}}}
	};


	rl rlTest(env);

	std::vector<std::string> allStates = rlTest.getAllStates();
	for (const auto& state : allStates)
	{
		std::cout << state << ":" << std::endl;
		std::vector<std::string> allActions = rlTest.getPossibleActions(state);
		for (const auto& action : allActions)
		{
			std::cout << "\t" << action << ":" << std::endl << "\t\t[NEXT]" << std::endl;
			const auto nextStates = rlTest.getNextStates(state, action);
			for (const auto& next : nextStates)
			{
				std::cout << "\t- " << next << std::endl;
			}
			std::cout << "\t\t[TRANSITION]" << std::endl;
			std::vector<prob_t> allTransitions = rlTest.getTransitions(state, action);
			for (const auto& transition : allTransitions)
			{
				std::cout << "\t\t- next: " << transition.next_state << std::endl;
				std::cout << "\t\t- probability: " << transition.probability << std::endl;
				std::cout << "\t\t- reward: " << transition.reward << std::endl;
			}
		}
	}

	rlTest.runValueIteration();
	for (const auto& QVal : rlTest.state_values)
	{
		std::cout << QVal.first << ": " << QVal.second << std::endl;
	}

	return 0;
}