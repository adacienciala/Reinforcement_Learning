#ifndef RL_H_INCLUDED
#define RL_H_INCLUDED

struct prob_t {
  char *name;
  float probability;
  int reward;
};

struct action_t {
  char *name;
  struct prob_t *probs;
  int size;
};

struct state_t {
  char *name;
  struct action_t *actions;
  int size;
};

struct state_val_t {
  char *name;
  float value;
};

struct environment_t {
  struct state_t *states;
  int size;
  struct state_val_t *states_values;
  float gamma;
  int iterations;
};

extern struct environment_t environment;

char ** getAllStates (void);
char ** getPossibleActions (char *state);
char ** getNextStates (char *state, char *action);
int isTerminal (char *state);
struct prob_t ** getTransitions (char *state, char *action);
int getReward (char *state, char *action, char *next_state);
float computeQValue (char *state, char *action);
void runValueIteration ();


#endif