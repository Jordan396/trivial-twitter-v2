/**
 * @file linked_lists_str.c
 * @brief Source code for linked list data structure and associated functions.
 *
 * Source code for linked list data structure definition and
 * commonly used linked list functions. 
 * 
 * Each node references a String item.
 * 
 * Don't modify any code in this file!
 *
 * @author Nanyang Technological University
 * @author Modified by Jordan396 <https://github.com/Jordan396>
 */

#include "linked_lists_str.h"

void printList(ListNode *head);
ListNode *findNode(ListNode *head, int i);
int insertNode(ListNode **ptrHead, int index, char *value);
int removeNode(ListNode **ptrHead, int index);
int sizeList(ListNode *head);

// int main()
// {
// 	int i, pos, tmp, counter;
// 	char tmpStr[MAX_ITEM_LEN];
// 	ListNode *cur, *head = NULL; /*INITIALIZE HEAD TO A NULL POINTER*/
// 	char val[MAX_ITEM_LEN];

// 	counter = 0;

// 	printf("Enter 5 strings:\n");
// 	while (counter < 5)
// 	{
// 		scanf("%s", val);
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
// 		strcpy(cur->item, val);
// 		counter++;
// 	}

// 	cur->next = NULL;

// 	printf("Current list: \n");
// 	printList(head);
// 	printf("The size of the current linked list is %d\n", sizeList(head));

// 	printf("Please enter where do you want to insert the value: ");
// 	scanf("%d", &pos);
// 	printf("Please enter what value you want to insert at index %d: ", pos);
// 	scanf("%s", val);
// 	printf("Inserted list:\n");
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
			printf("%s\n", temp->item);
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

int insertNode(ListNode **ptrHead, int index, char *value)
{
	ListNode *pre, *cur;
	if (*ptrHead == NULL || index == 0)
	{
		pre = *ptrHead;
		*ptrHead = malloc(sizeof(ListNode));
		//(*ptrHead)->item = value;
		(*ptrHead)->next = pre;
		return 0; //Remember to return 0 upon successful execution
	}
	else if ((pre = findNode(*ptrHead, index - 1)))
	{
		cur = malloc(sizeof(ListNode));
		strcpy(cur->item, value);
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