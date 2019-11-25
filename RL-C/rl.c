#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rl.h"

struct environment_t environment = {
  (struct state_t[]){
    { "s0", 
      (struct action_t[]) {
        { "a0",
          (struct prob_t[]) {
            { "s0", 0.5, 0 }, 
            { "s2", 0.5, 0 }
          }, 2 },
        { "a1",
          (struct prob_t[]) {
            { "s2", 1, 0 }
          }, 1 },
      }, 2 },
    { "s1", 
      (struct action_t[]) {
        { "a0",
          (struct prob_t[]) {
            { "s0", 0.7, 5 }, 
            { "s1", 0.1, 0 }, 
            { "s2", 0.2, 0 }
          }, 3 },
        { "a1",
          (struct prob_t[]) {
            { "s0", 0.95, 0 }, 
            { "s2", 0.05, 0 }
          }, 2 },
      }, 2 },
    { "s2", 
      (struct action_t[]) {
        { "a0",
          (struct prob_t[]) {
            { "s0", 0.4, 0 }, 
            { "s2", 0.6, 0 }
          }, 2 },
        { "a1",
          (struct prob_t[]) {
            { "s0", 0.3, -1 }, 
            { "s1", 0.3, 0 }, 
            { "s2", 0.4, 0 }
          }, 3 },
      }, 2 }
  }, 3, 
    (struct state_val_t []){
      { "s0", 0 },
      { "s1", 0 },
      { "s2", 0 } }, 0.9, 100 };


char ** getAllStates (void)
{
  // iteration after all the possible states in the environment
  // if there's no states, returns NULL
  // otherwise returns an array of pointers to char of size+1, where the last pointer is NULL
  // shallow copy of data!

  if (!environment.size) return NULL;

  char ** AllStates = (char **)malloc(sizeof(char *)*(environment.size+1));
  for (int i=0; i<environment.size; ++i)
  {
    *(AllStates+i) = (environment.states+i)->name;
  }
  *(AllStates+environment.size) = NULL;
  return AllStates;
};

char ** getPossibleActions (char *state)
{
  // iteration after all the possible actions in the environment for a given state
  // returns an array of pointers to char of size+1, where the last pointer is NULL
  // if there's no possible actions, returns NULL
  // shallow copy of data!

  for (int i=0; i<environment.size; ++i)
  {
    if (!strcmp((environment.states+i)->name, state))
    {
      char ** PossibleActions = (char **)malloc(sizeof(char *)*((environment.states+i)->size+1));
      for (int j=0; j<(environment.states+i)->size; ++j)
      {
        *(PossibleActions+j) = ((environment.states+i)->actions+j)->name;
      }
      *(PossibleActions+(environment.states+i)->size) = NULL;
      return PossibleActions;
    }
  }
  return NULL;
};

char ** getNextStates (char *state, char *action)
{
  // gets Transitions for the given state and action and searches for all the next states
  // returns an array of pointers to char of size+1, where the last pointer is NULL
  // if there's no next states, returns NULL
  // shallow copy of data!

  char ** NextStates = NULL;
  int counter = 0;
  struct prob_t ** Transitions = getTransitions(state, action);
  for (int i=0; Transitions && *(Transitions+i); ++i)
  {
    counter++;
    NextStates = (char **)realloc(NextStates, sizeof(char *)*(counter+1));
    *(NextStates+i) = (*(Transitions+i))->name;
  }
  if (NextStates)*(NextStates+counter) = NULL;
  if (Transitions) free(Transitions);
  return NextStates;
};

int isTerminal (char *state)
{
  // returns 1 if the state is terminal (there's no possible actions)
  // otherwise returns 0

  if (!getPossibleActions(state)) return 1;
  else return 0;
};

struct prob_t ** getTransitions (char *state, char *action)
{
  //shallow copy of struct prob_t for the given state and action
  // returns an array of pointers to struct prob_t of size+1, where the last pointer is NULL
  for (int i=0; i<environment.size; ++i)
  {
    if (!strcmp((environment.states+i)->name, state))
    {
      for (int j=0; j<(environment.states+i)->size; ++j)
      {
        if (!strcmp(((environment.states+i)->actions+j)->name, action))
        {
          struct prob_t ** Transitions = (struct prob_t **)malloc(sizeof(struct prob_t *)*(((environment.states+i)->actions+j)->size)+1);
          for (int k=0; k<((environment.states+i)->actions+j)->size; ++k)
          {
            *(Transitions+k) = ((environment.states+i)->actions+j)->probs+k;
          }
          *(Transitions+((environment.states+i)->actions+j)->size) = NULL;
          return Transitions;
        }
      }
    }
  }
  return NULL;
};

int getReward (char *state, char *action, char *next_state)
{
  // gets Transitions for the given state and action and searches for the given next_state
  // returns reward for next_state
  // if wrong data, returns 0

  int reward = 0;
  struct prob_t ** Transitions = getTransitions(state, action);
  for (int i=0; Transitions && *(Transitions+i); ++i)
  {
    if (!strcmp((*(Transitions+i))->name, next_state))
    {
      reward = (*(Transitions+i))->reward;
      break;
    }
  }
  if (Transitions) free(Transitions);
  return reward;
};

float computeQValue (char *state, char *action)
{
  // compute Q(s, a) value
  // Q(s, a) - how good is to be in that state, what possible rewards can you get from there by taking given action
  // returns value or 0, if the state is terminal

  float value = 0, prev_value=0;

  struct prob_t ** Transitions = getTransitions(state, action);

  for (int i=0; Transitions && *(Transitions+i); ++i)
  {
    for (int j=0; j<environment.size; ++j)
    {
      if (!strcmp((environment.states_values+j)->name, (*(Transitions+i))->name)) prev_value = (environment.states_values+j)->value;
    }
    value += (*(Transitions+i))->probability * ((*(Transitions+i))->reward + (environment.gamma * prev_value));
  }

  if (Transitions) free(Transitions);

  return value;
};

void runValueIteration ()
{
  // creates a new states_values tab to store new QValues 
  // finds maximum and saves that to the original

  float * new_state_val = (float *)calloc(sizeof(float), environment.size);
  if (!new_state_val) return ;

  float QVal=0, max=0;

  for (int i=0; i<environment.iterations; ++i)
  {
    char ** AllStates = getAllStates();
    for (int j=0; AllStates && *(AllStates+j); ++j)
    {
      char ** PossibleActions = getPossibleActions(*(AllStates+j));
      for (int k=0; PossibleActions && *(PossibleActions+k); ++k)
      {
        QVal = computeQValue(*(AllStates+j), *(PossibleActions+k));
        if (!k || QVal > max)
        {
          max = QVal;
        }
      }
      new_state_val[j] = max;
    }
    for (int l=0; l<environment.size; ++l)
    {
      (environment.states_values+l)->value = new_state_val[l];
    }
  }
  
  return ;
};