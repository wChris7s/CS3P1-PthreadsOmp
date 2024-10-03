#include <stdio.h>
#include <stdlib.h>

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

int  insert(int value, struct list_node_s** head_p);
void print(const struct list_node_s* head_p);
int  find(int value, struct list_node_s* head_p);
int  delete_(int value, struct list_node_s** head_p);
void free_list(struct list_node_s** head_p);
int  is_empty(struct list_node_s* head_p);

char Get_command(void);
int  Get_value(void);

/*-----------------------------------------------------------------*/
int main(void)
{
   char command;
   int  value;
   struct list_node_s* head_p = NULL;  /* start with empty list */

   command = Get_command();
   while (command != 'q' && command != 'Q') {
      switch (command) {
      case 'i':
      case 'I':
         value = Get_value();
         insert(value, &head_p);  /* Ignore return value */
         break;
      case 'p':
      case 'P':
         print(head_p);
         break;
      case 'm':
      case 'M':
         value = Get_value();
         find(value, head_p);   /* Ignore return value */
         break;
      case 'd':
      case 'D':
         value = Get_value();
         delete_(value, &head_p);  /* Ignore return value */
         break;
      default:
         printf("There is no %c command\n", command);
         printf("Please try again\n");
      }
      command = Get_command();
   }
   free_list(&head_p);

   return 0;
}

int insert(const int value, struct list_node_s** head_pp) {
   struct list_node_s* curr_p = *head_pp;
   struct list_node_s* pred_p = NULL;
   while (curr_p != NULL && curr_p->data < value) {
      pred_p = curr_p;
      curr_p = curr_p->next;
   }
   if (curr_p == NULL || curr_p->data > value) {
      struct list_node_s* temp_p = malloc(sizeof(struct list_node_s));
      temp_p->data = value;
      temp_p->next = curr_p;
      if (pred_p == NULL)
         *head_pp = temp_p;
      else
         pred_p->next = temp_p;
      return 1;
   }
   printf("%d is already in the list\n", value);
   return 0;
}


void print(const struct list_node_s* head_p) {
   printf("list = ");
   for(const struct list_node_s* curr_p = head_p; curr_p != NULL; curr_p = curr_p->next) {
      printf("%d ", curr_p->data);
   }
   printf("\n");
}


int find(const int value, struct list_node_s* head_p) {
   struct list_node_s* curr_p;
   for(curr_p = head_p ; curr_p != NULL && curr_p->data < value; curr_p = curr_p->next) {}
   if (curr_p == NULL || curr_p->data > value) {
      printf("%d is not in the list\n", value);
      return 0;
   }
   printf("%d is in the list\n", value);
   return 1;
}

int delete_(int value, struct list_node_s** head_pp) {
   struct list_node_s* curr_p = *head_pp;
   struct list_node_s* pred_p = NULL;

   /* Find value */
   while (curr_p != NULL && curr_p->data < value) {
      pred_p = curr_p;
      curr_p = curr_p->next;
   }
   
   if (curr_p != NULL && curr_p->data == value) {
      if (pred_p == NULL) { /* first element in list */
         *head_pp = curr_p->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr_p);
      } else { 
         pred_p->next = curr_p->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr_p);
      }
      return 1;
   }
   printf("%d is not in the list\n", value);
   return 0;
}

void free_list(struct list_node_s** head_pp) {
   struct list_node_s* curr_p;
   struct list_node_s* succ_p;

   if (is_empty(*head_pp)) return;
   curr_p = *head_pp; 
   succ_p = curr_p->next;
   while (succ_p != NULL) {
#     ifdef DEBUG
      printf("Freeing %d\n", curr_p->data);
#     endif
      free(curr_p);
      curr_p = succ_p;
      succ_p = curr_p->next;
   }
#  ifdef DEBUG
   printf("Freeing %d\n", curr_p->data);
#  endif
   free(curr_p);
   *head_pp = NULL;
}


int  is_empty(struct list_node_s* head_p) {
   return head_p == NULL ? 1 : 0;
}

char Get_command(void) {
   char c;
   printf("Please enter a command:  ");
   scanf(" %c", &c);
   return c;
}

int  Get_value(void) {
   int val;
   printf("Please enter a value:  ");
   scanf("%d", &val);
   return val;
}
