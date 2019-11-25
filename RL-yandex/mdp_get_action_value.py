def get_action_value(mdp, state_values, state, action, gamma):
    """ Computes Q(s,a) as in formula above """
    next_states = mdp.get_next_states(state, action)
    sum = 0
    for i in next_states:
        sum += mdp.get_transition_prob(state, action, i) * (mdp.get_reward(state, action, i) + gamma*state_values.get(i))
    return sum