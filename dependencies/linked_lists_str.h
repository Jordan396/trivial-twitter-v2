/**
 * @file linked_lists_str.h
 * @brief Header for linked list data structure and associated functions.
 *
 * Header file for linked list data structure definition and
 * commonly used linked list functions. 
 * 
 * Each node references a String item.
 * 
 * Don't modify any code in this file!
 *
 * @author Nanyang Technological University
 * @author Modified by Jordan396 <https://github.com/Jordan396>
 */

#ifndef LINKEDLISTS_H
#define LINKEDLISTS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ITEM_LEN 250

// Struct for a single ListNode
typedef struct ListNode
{
    char item[MAX_ITEM_LEN];
    struct ListNode *next;
} ListNode;

// Visible functions
void printList(ListNode *head);
ListNode *findNode(ListNode *head, int index);
int insertNode(ListNode **ptrHead, int index, char *value);
int removeNode(ListNode **ptrHead, int index);
int sizeList(ListNode *head);

#endif // LINKEDLISTS_H
