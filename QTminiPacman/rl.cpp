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
	this->episodes = 1000;
};

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

void rl::runQLearning(const state_t& starting_state)
{
	this->state_values.clear();

	state_t starting_state = { { 0, 4 }, { 3, 0 }, {2, 2} };

	float QVal;
	for (int i = 0; i < this->episodes; ++i)
	{
		state_t state = starting_state;
		state_t next_state = state;
		while (this->environment->isTerminal(state) == 0)
		{
			action_t actionPlayer = getAction(state, true);
			action_t actionGhost = getAction(state, false);

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
			// stary stab + alfa * R nowego stanu + y* sprawdzam mozliwe akcje z nowego stanu i wtedy nextstate'y sa srednia w zaleznosci od ruchu ducha - stary stan
			QVal = getQValue(state, actionPlayer) + this->alpha * (this->environment->getReward(next_state) + (this->delta * computeValFromQVal(next_state)) - getQValue(state, actionPlayer));
			this->state_values[next_state] = QVal;
			state = next_state;
		}
	}
	FILE* fp;
	fp = fopen("Qvalues.txt", "w");
	for (const auto& state : this->state_values)
	{
		fprintf(fp, "player: (%d, %d), ghost: (%d, %d), coin: (%d, %d) - %d\n", state.first.player.first, state.first.player.second, state.first.ghost.first, state.first.ghost.second, state.first.coin.first, state.first.coin.second, state.second);
	}
	fclose(fp);
}

// done?
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
		std::vector<action_t> possibleActions = this->environment->getPossibleActionsQLearning(coords);
		return possibleActions[rand() % possibleActions.size()];
	}
}

// done?
float rl::getQValue(const state_t& state, const action_t& action)
{
	// returns the QValue of the next state
	// average QValues of the possible next states (different ghost moves)
	// if the state was not encountered yet, appends the state with the 0.0 value

	// determing player's new (x, y)
	std::pair<int, int> northPlayer = !this->environment->isAvailable(state.player, NORTH) ? state.player : std::make_pair(state.player.first, state.player.second - 1);
	std::pair<int, int> southPlayer = !this->environment->isAvailable(state.player, SOUTH) ? state.player : std::make_pair(state.player.first, state.player.second + 1);
	std::pair<int, int> eastPlayer = !this->environment->isAvailable(state.player, EAST) ? state.player : std::make_pair(state.player.first + 1, state.player.second);
	std::pair<int, int> westPlayer = !this->environment->isAvailable(state.player, WEST) ? state.player : std::make_pair(state.player.first - 1, state.player.second);

	std::pair<int, int> nextPlayer;
	switch (action)
	{
	case EAST:
		nextPlayer = eastPlayer;
		break;
	case WEST:
		nextPlayer = westPlayer;
		break;
	case NORTH:
		nextPlayer = northPlayer;
		break;
	case SOUTH:
		nextPlayer = southPlayer;
	}

	// determing ghost's possible (x, y)s
	std::vector<std::pair<int, int>> nextGhost = std::vector<std::pair<int, int>>();
	if (this->environment->isAvailable(state.ghost, NORTH)) nextGhost.push_back(std::make_pair(state.ghost.first, state.ghost.second - 1));
	if (this->environment->isAvailable(state.ghost, SOUTH)) nextGhost.push_back(std::make_pair(state.ghost.first, state.ghost.second + 1));
	if (this->environment->isAvailable(state.ghost, EAST)) nextGhost.push_back(std::make_pair(state.ghost.first + 1, state.ghost.second));
	if (this->environment->isAvailable(state.ghost, WEST)) nextGhost.push_back(std::make_pair(state.ghost.first - 1, state.ghost.second));
	
	float QVal = 0;
	for (const auto& possibleGhost : nextGhost)
	{
		state_t tempState = { nextPlayer, possibleGhost, state.coin };
		// szukaj o ten stan z ghost i player
		// jak ni ma to dodaj z 0
		const auto new_state = this->state_values.find(tempState);
		if (new_state == this->state_values.end())
		{
			this->state_values[tempState] = 0.0f;
		}
		// dodaj do puli, z ktorej potem policzysz srednia
		QVal += this->state_values.at(tempState);
	}
	return QVal / nextGhost.size; // na pewno zwroci floata? (C by pewno nie XD)
}

// done?
float rl::computeValFromQVal(const state_t& state)
{
	// searches for the given state
	// gets values for all legal actions you can do from that state
	// returns max of those values (or random of equal max ones)
	// if the state is not found, appends the state and returns 0

	auto searchedState = this->state_values.find(state);

	if (searchedState != this->state_values.end())
	{
		std::vector<float> values;
		float QVal, max;

		std::vector<action_t> possibleActions = this->environment->getPossibleActionsQLearning(state.player);
		if (possibleActions.empty) return 0.0f;

		for (const auto& action : possibleActions)
		{
			QVal = getQValue(state, action);
			values.push_back(QVal);
			if (action == possibleActions[0] || QVal > max)
			{
				max = QVal;
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
			QVal = getQValue(state, action);
			if (QVal == max)
			{
				if (id-- == 0) return QVal;
			}
		}
	}
	else
	{
		state_values[state] = 0.0f;
		return 0.0f;
	}
}

// done?
action_t rl::computeActionFromQVal(const state_t& state)
{
	// searches for the given state
	// gets values for all legal actions you can do from that state
	// returns action linked to the max of those values (or to random of equal max ones)
	// if the state is not found, appends the state and returns STAY

	auto searchedState = this->state_values.find(state);

	if (searchedState != this->state_values.end())
	{
		std::vector<float> values;
		float QVal, max;

		std::vector<action_t> possibleActions = this->environment->getPossibleActionsQLearning(state.player);
		if (possibleActions.empty) return STAY;

		for (const auto& action : possibleActions)
		{
			QVal = getQValue(state, action);
			values.push_back(QVal);
			if (action == possibleActions[0] || QVal > max)
			{
				max = QVal;
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
			QVal = getQValue(state, action);
			if (QVal == max)
			{
				if (id-- == 0) return action;
			}
		}
	}
	else
	{
		state_values[state] = 0.0f;
		return STAY;
	}
}