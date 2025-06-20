#include "helpers/history.h"
#include "jogo/comandos.h"
#include <stdlib.h>

TabHistory *push_history(TabHistory *head, ParsedCommand *cmd) {
  TabHistory *new_node = (TabHistory *)malloc(sizeof(TabHistory));
  if (!new_node)
    return head;

  new_node->cmd = deep_copy_cmd(cmd);
  if (!new_node->cmd) {
    free(new_node);
    return head;
  }

  new_node->next = NULL;

  if (head == NULL) {
    return new_node; // New node becomes head
  }

  TabHistory *current = head;
  while (current->next != NULL)
    current = current->next;

  current->next = new_node;
  return head;
}

ParsedCommand *pop_history(TabHistory **head) {
  if (*head == NULL)
    return NULL;

  ParsedCommand *removed_cmd;

  // one element, return
  if ((*head)->next == NULL) {
    removed_cmd = (*head)->cmd;
    free(*head);
    *head = NULL;
    return removed_cmd;
  }

  TabHistory *current = *head;
  TabHistory *prev = NULL;
  while (current->next != NULL) {
    prev = current;
    current = current->next;
  }

  removed_cmd = current->cmd;
  if (prev != NULL) {
    prev->next = NULL;
  }
  free(current);
  return removed_cmd;
}

ParsedCommand *get_history_element(TabHistory *head, int index) {
  if (index < 0)
    return NULL;
  int i = 0;
  TabHistory *current = head;
  while (current != NULL && i < index) {
    current = current->next;
    i++;
  }
  return (current ? current->cmd : NULL);
}
