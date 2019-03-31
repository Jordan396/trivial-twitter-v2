/**
 * @file linked_lists.h
 * @brief Header for linked list data structure and associated functions.
 *
 * Header file for linked list data structure definition and
 * commonly used linked list functions. Don't modify any code in this file!
 *
 * @author Nanyang Technological University
 */

#ifndef LINKEDLISTS_H
#define LINKEDLISTS_H

#include <stdlib.h>
#include <stdio.h>

// Struct for a single ListNode
typedef struct ListNode
{
    int item;
    struct ListNode *next;
} ListNode;

// Visible functions
void printList(ListNode *head);
ListNode *findNode(ListNode *head, int index);
int insertNode(ListNode **ptrHead, int index, int value);
int removeNode(ListNode **ptrHead, int index);
int sizeList(ListNode *head);

#endif // LINKEDLISTS_H
