#include <stdio.h>
#include <stdlib.h>
#include "rl.h"

int main(void) 
{
  printf("All states:\n");
  char ** AllStates = getAllStates();
  for (int i=0; AllStates && *(AllStates+i); ++i)
  {
    printf("%s ", *(AllStates+i));
  }

  printf("\n\nAll possible actions:");
  for (int i=0; AllStates && *(AllStates+i); ++i)
  {
    char ** PossibleActions = getPossibleActions(*(AllStates+i));
    printf("\nState %s: ", *(AllStates+i));
    for (int j=0; PossibleActions && *(PossibleActions+j); ++j)
    {
      printf("%s ", *(PossibleActions+j));
    }
    free(PossibleActions);
  }
  

  printf("\n\nTransitions from states:\n");
  for (int i=0; AllStates && *(AllStates+i); ++i)
  {
    char ** PossibleActions = getPossibleActions(*(AllStates+i));
    printf("\nState %s:\n", *(AllStates+i));
    for (int j=0; PossibleActions && *(PossibleActions+j); ++j)
    {
      printf("- action %s: \n", *(PossibleActions+j));
      struct prob_t ** Transitions = getTransitions(*(AllStates+i), *(PossibleActions+j));
      for (int k=0; Transitions && *(Transitions+k); ++k)
      {
        printf("\t*next state: %s\n\t*probability: %.2f\n\t*reward: %d\n", (*(Transitions+k))->name, (*(Transitions+k))->probability, (*(Transitions+k))->reward);
      }
      free(Transitions);
    }
    free(PossibleActions);
  }
  free(AllStates);

  char ** NextStates = getNextStates("s2", "a0");
  for (int i=0; NextStates && *(NextStates+i); ++i)
  {
    printf("%s ", *(NextStates+i));
  }

  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("QVal %f", computeQValue("s2", "a1"));

  printf("\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
  printf("s0 %.3f, s1 %.3f, s2 %.3f\n\n", environment.states_values->value, (environment.states_values+1)->value, (environment.states_values+2)->value);
  runValueIteration();
  printf("s0 %.3f, s1 %.3f, s2 %.3f\n\n", environment.states_values->value, (environment.states_values+1)->value, (environment.states_values+2)->value);

  return 0;
}