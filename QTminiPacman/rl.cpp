#include <iostream>
#include <algorithm>

#include "rl.h"

rl::rl(mdp* environment)
{
	this->gamma = 0.9f;
	this->delta = 0.05f;
	this->iterations = 100;
	this->environment = environment;
	this->epsilon = 0.3f;
	this->alpha = 0.5f;
	this->episodes = 500;
	this->mode = NOT_SET;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// -------- VALUE and POLICY iterations -------- /////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

float rl::computeQValue(const state_t& state, const action_t& action)
{
	// compute Q(s, a) value
	// Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
	// returns value or 0, if the state is terminal

	float value = 0, prev_value = 0;

	std::map<std::pair<action_t, state_t>, float> transitions = this->environment->getTransitions(state, action);
	for (const auto& it : transitions)
	{
		prev_value = this->state_values[it.first.second];
		value += it.second * (this->environment->getReward(state) + (this->gamma * prev_value));
	}

	return value;
};

void rl::runValueIteration()
{
	// creates a new states_values map to store new QValues 
	// for each state finds maximum QValue
	// saves the best actions as a map <state_t, float>

	clearStateValues();
	std::map<state_t, float> new_state_values;

	float QVal = 0, max = 0;

	for (int i = 0; i < this->iterations; ++i)
	{
		std::vector<state_t> allStates = this->environment->getAllStates();
		for (const auto& state : allStates)
		{
			std::vector<action_t> possibleActions = this->environment->getPossibleActions(state);
			for (const auto& action : possibleActions)
			{
				QVal = computeQValue(state, action);
				if (action == possibleActions[0] || QVal > max)
				{
					max = QVal;
				}
			}
			if (this->environment->isTerminal(state)) max = (float)this->environment->getReward(state);
			new_state_values[state] = max;
		}
		this->state_values = new_state_values;
	}
	FILE* fp;
	fp = fopen("values.txt", "w");
	for (const auto& state : this->state_values)
	{
		fprintf(fp, "player: (%d, %d), ghost: (%d, %d), coin: (%d, %d) - %f\n", state.first.player.first, state.first.player.second, state.first.ghost.first, state.first.ghost.second, state.first.coin.first, state.first.coin.second, state.second);
	}
	fclose(fp);
}

void rl::clearStateValues()
{
	// clears the state_values map, zeroing values of all the states

	const auto allStates = this->environment->getAllStates();
	for (auto& state : allStates)
	{
		this->state_values[state] = 0.0;
	}
}

void rl::runPolicyIteration()
{
	// creates a new new_state_policies map to store new policies 
	// for each state finds maximum QValue and saves its action
	// saves the best actions as a map <state_t, action_t>
	
	std::map<state_t, action_t> new_state_policies;

	float QVal = 0, max = 0;
	action_t best_action = STAY;

	std::vector<state_t> allStates = this->environment->getAllStates();
	for (const auto& state : allStates)
	{
		std::vector<action_t> possibleActions = this->environment->getPossibleActions(state);
		for (const auto& action : possibleActions)
		{
			QVal = computeQValue(state, action);
			if (action == possibleActions[0] || QVal > max)
			{
				max = QVal;
				best_action = action;
			}
		}
		if (this->environment->isTerminal(state))
		{
			max = (float)this->environment->getReward(state);
			best_action = STAY;
		}
		new_state_policies[state] = best_action;
	}
	this->state_policies = new_state_policies;
	FILE* fp;
	fp = fopen("policies.txt", "w");
	for (const auto& state : this->state_policies)
	{
		fprintf(fp, "player: (%d, %d), ghost: (%d, %d), coin: (%d, %d) - %d\n", state.first.player.first, state.first.player.second, state.first.ghost.first, state.first.ghost.second, state.first.coin.first, state.first.coin.second, state.second);
	}
	fclose(fp);
}

action_t rl::getBestPolicy(const state_t& state) const
{
	// returns the best action from the given state

	return this->state_policies.at(state);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// -------- QLEARNING + SARSA -------- /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
// ------------------- Q(S,A) = Q(S,A) + alfa (R + delta * maxQ(S',a) - Q(S,A)) ------------------- //
// Q(S,A)			- QVal of the state with a given action
// alfa			- learning rate
// R				- reward in the given state
// gamma			- forgetting factor
// maxQ(S',a)		- max QVal of the actions taken from the next state
//////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------- Q(S,A) = Q(S,A) + alfa (R + delta * Q(S',A') - Q(S,A)) -------------------- //
// Q(S,A)			- QVal of the state with a given action
// alfa			- learning rate
// R				- reward in the given state
// gamma			- forgetting factor
// Q(S',A')		- QVal of the next state with the best action (that will be taken)
//////////////////////////////////////////////////////////////////////////////////////////////////////

void rl::runQLearning(const state_t& starting_state)
{
	// for the given number of episodes tries different actions
	// an episode ends, when the state is terminate - player won or lost
	// gathered knowledge is saved in the state_QValues <state_t <action_t float>> map

	float QVal;
	for (int i = 0; i < this->episodes; ++i)
	{
		state_t state = starting_state;
		state_t next_state;
		int reward = 0;
		bool is_terminal = false;
		while (is_terminal == false)
		{
			action_t actionGhost = getAction(state, false);
			action_t actionPlayer = getAction(state, true);

			std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer, actionGhost);

			QVal = getQValue(state, actionPlayer) + (this->alpha * (reward + (this->delta * computeValFromQVal(next_state)) - getQValue(state, actionPlayer)));
			this->state_QValues[state][actionPlayer] = QVal;
			state = next_state;
		}
	}

	FILE* fp;
	fp = fopen("Qvalues.txt", "w");
	for (const auto& state : this->state_QValues)
	{
		fprintf(fp, "* player: (%d, %d), ghost: (%d, %d), coin: (%d, %d):\n", state.first.player.first, state.first.player.second, state.first.ghost.first, state.first.ghost.second, state.first.coin.first, state.first.coin.second);
		for (const auto& action : state.second)
		{
			fprintf(fp, "\t-%f\n", action.second);
		}
	}
	fclose(fp);
}

bool rl::stepQLearning(state_t& state)
{
	// a step is making a move from the given state
	// gets actions for the ghost and the player
	// gets feedback from the mdp
	// calculates QVal of the state and saves it
	// returns info whether the next state is terminal (so the episode can end)

	state_t next_state;
	int reward = 0;
	bool is_terminal = false;

	action_t actionPlayer = getAction(state, true);
	action_t actionGhost = getAction(state, false);

	std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer, actionGhost);

	float QVal = getQValue(state, actionPlayer) + (this->alpha * (reward + (this->delta * computeValFromQVal(next_state)) - getQValue(state, actionPlayer)));
	this->state_QValues[state][actionPlayer] = QVal;
	
	state = next_state;
	return is_terminal;
}

bool rl::stepSarsa(state_t& state, bool reset)
{
	// a step is making a move from the given state
	// randoms actions for the ghost
	// gets feedback from the mdp
	// calculates best next action and saves it
	// calculates QVal of the state and saves it
	// returns info whether the next state is terminal (so the episode can end)

	state_t next_state;
	action_t next_player;
	int reward = 0;
	bool is_terminal = false;

	static action_t actionPlayer = getAction(state, true);
	if (reset == true)
	{
		actionPlayer = getAction(state, true);
	}
	action_t actionGhost = getAction(state, false);

	std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer, actionGhost);

	next_player = computeActionFromQVal(next_state);
	float QVal = getQValue(state, actionPlayer) + (this->alpha * (reward + (this->delta * computeValFromQVal(next_state)) - getQValue(state, actionPlayer)));
	this->state_QValues[state][actionPlayer] = QVal;

	state = next_state;
	actionPlayer = next_player;
	return is_terminal;
}

action_t rl::getAction(const state_t& state, bool player)
{
	// for the player: [player == true]
	// randomizes a number and, depending on the set epsilon, chooses an action
	// if prob <0, epsilon>, returns random action
	// else returns best possible action for the given state

	// for the ghost: [player == false]
	// returns random action

	float prob = rand() % 100 / (float)100;
	if (prob > this->epsilon && player == true)
	{
		return computeActionFromQVal(state);
	}
	else
	{
		std::pair<int, int> coords = (player == true) ? state.player : state.ghost;
		return this->environment->randomAction(coords, player);
	}
}

float rl::getQValue(const state_t& state, const action_t& action)
{
	// looks up the given state and gets value for the given action
	// if the state was not encountered or the action wasn't taken yet: 
	// - appends the state with the reward value for the given action (important for terminal states)
	// returns the QValue of the action from the given state
	
	const auto sought_state = this->state_QValues.find(state);
	if (sought_state == this->state_QValues.end() || sought_state->second.find(action) == sought_state->second.end())
	{
		this->state_QValues[state][action] = (float)this->environment->getReward(state);
	}
	
	float QVal = this->state_QValues.at(state).at(action);
	
	return QVal;
}

float rl::computeValFromQVal(const state_t& state)
{
	// searches for the given state
	// returns max of the possible actions' values
	// if the state is not found, appends the state with all possible actions and returns 0

	const auto sought_state = this->state_QValues.find(state);

	if (sought_state != this->state_QValues.end())
	{
		float QVal, max = 0;
		bool first = true;
		for (const auto& action : this->state_QValues.at(state))
		{
			QVal = getQValue(state, action.first);
			if (first == true || QVal > max)
			{
				max = QVal;
			}
			first = false;
		}

		return max;
	}
	else
	{
		std::vector<action_t> possibleActions;
		if (!this->environment->isTerminal(state)) possibleActions = this->environment->getPossibleActionsQLearning(state.player, true);
		else possibleActions.push_back(STAY);
		if (possibleActions.empty()) return 0.0f;

		for (const auto& action : possibleActions)
		{
			this->state_QValues[state][action] = (float)this->environment->getReward(state);
		}
		return 0.0f;
	}
}

action_t rl::computeActionFromQVal(const state_t& state)
{
	// searches for the given state
	// returns action linked to the max of the possible actions' values (or to random of equal max ones)
	// if the state is not found, appends the state and returns random

	auto searchedState = this->state_QValues.find(state);

	if (searchedState != this->state_QValues.end())
	{
		std::vector<float> values;
		float QVal, max = 0;
		bool first = true;
		for (const auto& action : this->state_QValues.at(state))
		{
			QVal = getQValue(state, action.first);
			values.push_back(QVal);
			if (first == true || QVal > max)
			{
				max = QVal;
			}
			first = false;
		}

		int counter = 0;
		for (const auto& val : values)
		{
			if (val == max) counter++;
		}

		int id = rand() % counter;
		for (const auto& action : this->state_QValues.at(state))
		{
			QVal = getQValue(state, action.first);
			if (QVal == max)
			{
				if (id-- == 0) return action.first;
			}
		}
	}
	else
	{
		std::vector<action_t> possibleActions;
		if (!this->environment->isTerminal(state)) possibleActions = this->environment->getPossibleActionsQLearning(state.player, true);
		else possibleActions.push_back(STAY);
		if (possibleActions.empty()) return STAY;

		for (const auto& action : possibleActions)
		{
			this->state_QValues[state][action] = (float)this->environment->getReward(state);
		}
		return possibleActions[rand() % possibleActions.size()];
	}
}