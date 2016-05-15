/* Implement and test FIFO queue for use in simulation */
#include <stdio.h>  
#include <stdlib.h>

/* Define linked list node */
typedef struct node {
    float t;
    struct node * next;
} node_t;


void  enqueue(node_t * head, int t);
float dequeue(node_t ** head);
void  print_list(node_t * head);

int main() {
	/* Initialize linked list */
	node_t * head = NULL;
	head = malloc(sizeof(node_t));

	if (head == NULL) {
		return 1;
	}

	head->t = 0.0;
	head->next = NULL;

	/* Mock enqueue/dequeue sequence */
	enqueue(head, 1);
	enqueue(head, 2);
	dequeue(&head);
	enqueue(head, 3);
	dequeue(&head);
	enqueue(head, 4);

	/* Print linked list contents and return */
	print_list(head);
	return 1;
}

/* Push to the queue */
void enqueue(node_t * head, int t) {
    /* Set temporary pointer to head's location */
    node_t * tmp = head;

    /* Iterate to the end of the linked list */
    while(tmp->next != NULL) {
        tmp = tmp->next;
    }

    /* Add this node to the end of the list */
    tmp->next       = malloc(sizeof(node_t));
    tmp->next->t    = t;
    tmp->next->next = NULL;
}

/* Pop from the queue and return the head's data */
float dequeue(node_t ** head) {
    float pop    = 0.0;
    node_t * tmp = NULL;
    
    /* Check for null list */
    if (head == NULL) {
    	return -1.0;
    }

    /* Free head data and assign head pointer to the next node in the queue */
    tmp = (*head)->next;
    pop = (*head)->t;
    free(*head);
    *head = tmp;

    /* Return data for the previous head */
    return pop;
}

/* Print the contents of the linked list */
void print_list(node_t * head) {
	node_t * tmp = head;
	while (tmp != NULL) {
		fprintf(stdout, "%f\n", tmp->t);
		tmp = tmp->next;
	}
}