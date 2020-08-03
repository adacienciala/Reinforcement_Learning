#include <iostream>
#include <algorithm>
#include <deque>

#include "rl.h"

#define EPSILON (0.3f)
#define ALPHA (0.05f)

rl::rl(mdp* environment)
{
	this->gamma = 0.9f;
	this->iterations = 100;
	this->environment = environment;
	this->epsilon = EPSILON;
	this->alpha = ALPHA;
	this->episodes = 1000; 
	this->mode = NOT_SET;
	this->features_weights = { { FOOD_DISTANCE, { 0.0, 0.0 } }, { GHOST_DISTANCE, { 0.0, 0.0 } },
								{ GHOSTS_NEAR, { 0.0, 0.0 } }, { FOOD_NEAR, { 0.0, 0.0 } } };
	this->memory_capacity = 2000;
	this->memoryDQL = std::deque<memory_bit_t>();
	this->network = neuralNetwork(0.05, {}, { { environment->width * environment->height, none }, { 50, RELU }, { 4, none } });
};

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// -------- VALUE and POLICY iterations -------- /////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

float rl::computeQValue(const state_t& state, const action_t& action)
{
	// compute Q(s, a) value
	// Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
	// returns value or 0, if the state is terminal

	float value = 0, next_value = 0;

	std::map<std::pair<action_t, state_t>, float> transitions = this->environment->getTransitions(state, action);
	for (const auto& it : transitions)
	{
		next_value = this->state_values[it.first.second];
		value += it.second * (this->environment->getReward(state) + (this->gamma * next_value));
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
// ------------------- Q(S,A) = Q(S,A) + alfa (R + gamma * maxQ(S',a) - Q(S,A)) ------------------- //
// Q(S,A)			- QVal of the state with a given action
// alfa			- learning rate
// R				- reward in the given state
// gamma			- forgetting factor
// maxQ(S',a)		- max QVal of the actions taken from the next state
//////////////////////////////////////////////////////////////////////////////////////////////////////
// -------------------- Q(S,A) = Q(S,A) + alfa (R + gamma * Q(S',A') - Q(S,A)) -------------------- //
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
			action_t actionPlayer = getAction(state, QLEARNING);

			std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer);

			QVal = getQValue(state, actionPlayer) + (this->alpha * (reward + (this->gamma * computeValFromQVal(next_state)) - getQValue(state, actionPlayer)));
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

	action_t actionPlayer = getAction(state, QLEARNING);

	std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer);

	float QVal = getQValue(state, actionPlayer) + (this->alpha * (reward + (this->gamma * computeValFromQVal(next_state)) - getQValue(state, actionPlayer)));
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

	static action_t actionPlayer = getAction(state, SARSA);
	if (reset == true)
	{
		actionPlayer = getAction(state, SARSA);
	}

	std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer);

	next_player = computeActionFromQVal(next_state);
	float QVal = getQValue(state, actionPlayer) + (this->alpha * (reward + (this->gamma * getQValue(next_state, next_player)) - getQValue(state, actionPlayer)));
	this->state_QValues[state][actionPlayer] = QVal;

	state = next_state;
	actionPlayer = next_player;
	return is_terminal;
}

action_t rl::getAction(const state_t& state, learningMode_t mode)
{
	// randomizes a number and, depending on the set epsilon, chooses an action
	// if prob <0, epsilon>, returns random action
	// else returns best possible action for the given state

	float prob = rand() % 100 / 100.0f;
	if (prob > this->epsilon)
	{
		if (mode == FA) return computeActionFromQValFA(state);
		else if (mode == DQL) return computeActionFromQValDQL(state);
		else return computeActionFromQVal(state);
	}
	else
	{
		std::pair<int, int> coords = state.player;
		//std::cout << "Random ";
		return this->environment->randomAction(coords, true);
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

void rl::resetFeatureWeights()
{
	// randomize weights for the features

	for (auto& [feature, valWeight] : features_weights)
	{
		valWeight.first = 0.0f;
		valWeight.second = ((rand() % 21) - 10) / 100.0f;
	}
}

bool rl::stepFA(state_t& state)
{
	// a step is making a move from the given state
	// gets actions for the ghost and the player
	// gets feedback from the mdp
	// calculates the diff to correct the features' weights
	// returns info whether the next state is terminal (so the episode can end)

	state_t next_state;
	int reward = 0;
	bool is_terminal = false;

	action_t actionPlayer = getAction(state, FA);

	std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer);

	float diff = (reward + (this->gamma * computeValFromQValFA(next_state))) - getQValueFA(state, actionPlayer);
	for (auto& [feature, valWeight] : features_weights)
	{
		valWeight.second = valWeight.second + (this->alpha * diff * valWeight.first);
	}

	state = next_state;
	return is_terminal;
}

float rl::getQValueFA(const state_t& state, const action_t& action)
{
	// feautures:
	// - distance to food
	// - distance to ghost
	// - number of ghosts 1 step away
	// - if food 1 step away

	float foodDistance = 0.0f;
	float ghostDistance = 0.0f;
	std::deque<std::pair<state_t, int>> search;
	std::map<state_t, bool> visited;
	state_t nextState = state;
	switch (action)
	{
	case NORTH:
		nextState.player.second -= 1;
		break;
	case SOUTH:
		nextState.player.second += 1;
		break;
	case EAST:
		nextState.player.first += 1;
		break;
	case WEST:
		nextState.player.first -= 1;
		break;
	}
	search.push_back({ nextState, 0 });
	float ghosts_near = 0.0f;
	float food_near = 0.0f;
	std::vector<action_t> possibleActions;

	while ((!foodDistance || !ghostDistance) && search.size())
	{
		std::pair<state_t, int> temp = search.front();
		search.pop_front();
		visited[temp.first] = true;
		
		if (this->environment->isTerminal(temp.first))
		{
			if (!foodDistance && temp.first.player.first == temp.first.coin.first && temp.first.player.second == temp.first.coin.second)
			{
				foodDistance = temp.second;
				if (foodDistance == 0) food_near = 1.0f;
			}
			if (!ghostDistance && temp.first.player.first == temp.first.ghost.first && temp.first.player.second == temp.first.ghost.second)
			{
				ghostDistance = temp.second;
				if (ghostDistance == 1) ghosts_near += 1;
			}
		}

		possibleActions = this->environment->getPossibleActions(temp.first);
		for (const auto& action : possibleActions)
		{
			if (this->environment->isAvailable(temp.first.player, action))
			{
				int x = 0;
				int y = 0;
				state_t tempState = temp.first;
				switch (action)
				{
				case NORTH:
					y -= 1;
					break;
				case SOUTH:
					y += 1;
					break;
				case EAST:
					x += 1;
					break;
				case WEST:
					x -= 1;
					break;
				default:
					continue;
				}
				tempState.player.first += x;
				tempState.player.second += y;
				if (visited.find(tempState)==visited.end()) search.push_back({ tempState, temp.second + 1}); 
			}
		}
	}

	int boardSize = this->environment->height * this->environment->width;
	features_weights.at(FOOD_DISTANCE).first = foodDistance / boardSize;
	features_weights.at(GHOST_DISTANCE).first = ghostDistance / boardSize;
	features_weights.at(GHOSTS_NEAR).first = ghosts_near / 2;
	features_weights.at(FOOD_NEAR).first = food_near;

	float value = 0;
	for (const auto& [feature, valWeight] : features_weights)
	{
		value += valWeight.first * valWeight.second;
	}
	return value;
}

float rl::computeValFromQValFA(const state_t& state)
{
	// returns max of the possible actions' values

	float QVal, max = 0;
	bool first = true;
	std::vector<action_t> possibleActions = this->environment->getPossibleActions(state);
	for (const auto& action : possibleActions)
	{
		if (this->environment->isAvailable(state.player, action))
		{
			QVal = getQValueFA(state, action);
			if (first == true || QVal > max)
			{
				max = QVal;
			}
			first = false;
		}
	}

	return max;
}

action_t rl::computeActionFromQValFA(const state_t& state)
{
	// returns action linked to the max of the possible actions' values (or to random of equal max ones)
	
	std::vector<float> values;
	float QVal, max = 0;
	bool first = true;
	std::vector<action_t> possibleActions = this->environment->getPossibleActions(state);
	for (const auto& action : possibleActions)
	{
		if (this->environment->isAvailable(state.player, action))
		{
			QVal = getQValueFA(state, action);
			values.push_back(QVal);
			if (first == true || QVal > max)
			{
				max = QVal;
			}
			first = false;
		}
	}

	int counter = 0;
	for (const auto& val : values)
	{
		if (val == max) counter++;
	}

	int id = rand() % counter;
	for (const auto& action : possibleActions)
	{
		if (this->environment->isAvailable(state.player, action))
		{
			QVal = getQValueFA(state, action);
			if (QVal == max)
			{
				if (id-- == 0) return action;
			}
		}
	}
}

void rl::resetDQLNetwork()
{
	// make new network and clear memory buffer

	this->network = neuralNetwork(0.05, {}, { { environment->width * environment->height, none }, { 50, RELU }, { 4, none } });
	this->memoryDQL.clear();
}

void rl::replayDQL(int batch_size)
{
	// train the network by "reliving" previous experience
	// gets a given number of random samples from memory
	// trains the network on those samples

	if (batch_size > this->memoryDQL.size()) return;

	int empty_yet = this->memoryDQL.size();

	bool* chosen = new bool[this->memoryDQL.size()];
	memset(chosen, false, this->memoryDQL.size() * sizeof(bool));

	for (int i = 0; i < batch_size; ++i)
	{
		int number = (rand() % empty_yet) + 1;
		for (int j = 0; j < this->memoryDQL.size(); ++j)
		{
			if (!chosen[j]) number--;
			if (!number)
			{
				chosen[j] = 1;
				break;
			}
		}
		empty_yet--;
	}

	for (int i = 0; i < this->memoryDQL.size(); ++i)
	{
		if (chosen[i])
		{
			memory_bit_t sample = this->memoryDQL[i];
			float QVal = sample.reward;
			std::vector<double> input = this->environment->getViewAsVector(sample.state);
			if (!sample.done) QVal = sample.reward + (this->gamma * computeValFromQValDQL(sample.next_state));
			network.computeValues({ input });
			std::vector<double> values = network.getOutputValues();
			values[sample.action] = QVal;
			this->network.trainNetwork(1, { input }, { values });
		}
	}

	delete[] chosen;
}

bool rl::stepDQL(state_t& state)
{
	// a step is making a move from the given state
	// gets actions for the ghost and the player
	// gets feedback from the mdp
	// calculates QVal of the state and saves it to train the network
	// returns info whether the next state is terminal (so the episode can end)

	state_t next_state;
	int reward = 0;
	bool is_terminal = false;

	action_t actionPlayer = getAction(state, DQL);
	//std::cout << "Action: " << actionPlayer << std::endl;

	std::tie(next_state, reward, is_terminal) = this->environment->makeQStep(state, actionPlayer);

	if (this->memoryDQL.size() >= this->memory_capacity) this->memoryDQL.pop_front();
	this->memoryDQL.push_back({ state, actionPlayer, reward/10, next_state, is_terminal });

	state = next_state;
	return is_terminal;
}

// done?
float rl::getQValueDQL(const state_t& state, const action_t& action)
{
	// gets the input data: grid and state
	// feeds network with reshaped data
	// returns the QValue of the action from the given state

	std::vector<double> input = this->environment->getViewAsVector(state);
	network.computeValues({ input });
	std::vector<double> values = network.getOutputValues();

	return (float)values.at(action);
}

// done?
float rl::computeValFromQValDQL(const state_t& state)
{
	// gets the input data: grid and state
	// feeds network with reshaped data
	// returns max of the possible actions' values

	std::vector<double> input = this->environment->getViewAsVector(state);
	network.computeValues({ input });
	std::vector<double> values = network.getOutputValues();

	bool initialized = false;
	double max;
	for (int id = 0; id < values.size(); ++id)
	{
		if (this->environment->isAvailable(state.player, (action_t)id))
		{
			if (!initialized || values[id] > max)
			{
				initialized = true;
				max = values[id];
			}
		}
	}
	if (!initialized)
	{
		std::cout << "Error: no max initialized, agent's stuck" << std::endl;
		return 0.0f;
	}
	return (float)max;
}

// done?
action_t rl::computeActionFromQValDQL(const state_t& state)
{
	// gets the input data: grid and state
	// feeds network with reshaped data
	// returns action linked to the max of the possible actions' values (or to random of equal max ones)
	// actions in vector: 0 - north, 1 - east, 2 - south, 3 - west

	std::vector<double> input = this->environment->getViewAsVector(state);
	network.computeValues({ input });
	std::vector<double> values = network.getOutputValues();
	/*for (auto& val : values)
		std::cout << val << " | ";
	std::cout << std::endl;*/

	bool initialized = false;
	double max;
	int counter = 0;
	for (int id = 0; id < values.size(); ++id)
	{
		if (this->environment->isAvailable(state.player, (action_t)id)) 
		{
			if (!initialized || values[id] > max)
			{
				initialized = true;
				max = values[id];
				counter = 0;
			}
			if (values[id] == max) counter++;
		}
	}
	if (!initialized) return STAY;
	int id = rand() % counter;
	for (int i = 0; i < values.size(); ++i)
	{
		if (this->environment->isAvailable(state.player, (action_t)i))
		{
			if (values[i] == max)
			{
				if (id-- == 0) return (action_t)i;
			}
		}
	}
}