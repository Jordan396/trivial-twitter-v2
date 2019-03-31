/**
 * @file linked_lists.c
 * @brief Source code for linked list data structure and associated functions.
 *
 * Source code for linked list data structure definition and
 * commonly used linked list functions. Don't modify any code in this file!
 *
 * @author Nanyang Technological University
 */

#include "linked_lists.h"

void printList(ListNode *head);
ListNode *findNode(ListNode *head, int i);
int insertNode(ListNode **ptrHead, int index, int value);
int removeNode(ListNode **ptrHead, int index);
int sizeList(ListNode *head);

// int main()
// {
// 	int i, pos, val, tmp;
// 	ListNode *cur, *head = NULL; /*INITIALIZE HEAD TO A NULL POINTER*/

// 	printf("Enter a list of numbers, terminated by the value -1:\n");

// 	scanf("%d", &i); /*ASK FOR INPUT BEFORE WHILE LOOP*/
// 	while (i != -1)
// 	{
// 		if (head == NULL)
// 		{ /*CHECK IF HEAD IS NULL TO ALLOCATE SPACE TO THE FIRST NODE*/
// 			head = malloc(sizeof(ListNode));
// 			cur = head;
// 		}
// 		else
// 		{
// 			cur->next = malloc(sizeof(ListNode));
// 			cur = cur->next;
// 		}
// 		cur->item = i;
// 		scanf("%d", &i);
// 	}

// 	cur->next = NULL;

// 	printf("Current list: ");
// 	printList(head);
// 	printf("The size of the current linked list is %d\n", sizeList(head));

// 	printf("Please enter where do you want to insert the value: ");
// 	scanf("%d", &pos);
// 	printf("Please enter what value you want to insert at index %d: ", pos);
// 	scanf("%d", &val);
// 	printf("Inserted list: ");
// 	tmp = insertNode(&head, pos, val);
// 	printList(head);
// 	printf("The size of the current linked list is %d\n", sizeList(head));

// 	printf("Please enter the index number of the value you want to delete: ");
// 	scanf("%d", &pos);
// 	tmp = removeNode(&head, pos);
// 	printf("List with node removed: ");
// 	printList(head);
// 	printf("The size of the current linked list is %d\n", sizeList(head));

// 	return 0;
// }

void printList(ListNode *head)
{

	ListNode *temp = head;
	if (temp == NULL)
	{
		return;
		/*THIS IS VERY IMPORTANT SINCE IF USERS ENTER -1 */
		/*THEN THE PROGRAM WILL RUN IN ERROR WITHOUT THIS LINE*/
	}
	else
	{
		while (temp)
		{
			printf("%d ", temp->item);
			temp = temp->next;
		}
		printf("\n");
	}
}

ListNode *findNode(ListNode *head, int i)
{
	ListNode *temp = head;
	if (head == NULL || i < 0)
	{
		return NULL;
	}
	while (i > 0)
	{
		temp = temp->next;
		if (temp == NULL)
		{
			return NULL;
		}
		i--;
	}
	return temp;
}

int insertNode(ListNode **ptrHead, int index, int value)
{
	ListNode *pre, *cur;
	if (*ptrHead == NULL || index == 0)
	{
		pre = *ptrHead;
		*ptrHead = malloc(sizeof(ListNode));
		(*ptrHead)->item = value;
		(*ptrHead)->next = pre;
		return 0; //Remember to return 0 upon successful execution
	}
	else if ((pre = findNode(*ptrHead, index - 1)))
	{
		cur = malloc(sizeof(ListNode));
		cur->item = value;
		cur->next = pre->next;
		pre->next = cur;
		return 0;
	}
	return -1;
}

int removeNode(ListNode **ptrHead, int index)
{
	ListNode *pre, *cur;
	if (*ptrHead == NULL)
	{
		return -1;
	}
	else if (index == 0)
	{
		cur = *ptrHead;
		*ptrHead = (*ptrHead)->next;
		free(cur);
		return 0;
	}
	else if ((pre = findNode(*ptrHead, index - 1)))
	{
		cur = pre->next;
		pre->next = pre->next->next;
		free(cur);
		return 0;
	}
	return -1;
}

int sizeList(ListNode *head)
{
	int size = 0;
	ListNode *cur = head;
	while (cur)
	{
		size++;
		cur = cur->next;
	}
	return size;
}