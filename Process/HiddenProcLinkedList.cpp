#include <stdio.h>
#include <windows.h>
#include "StdAfx.h"
#include "HiddenProcLinkedList.h"


void initList(LINKED_LIST *list)
{
	list->head = NULL;
	list->tail = NULL;
}


void freeList(LINKED_LIST *list)
{
	LINKED_LIST_NODE *curNode = list->head;
	LINKED_LIST_NODE *nextNode;

	while(curNode)
	{
		free(curNode->data);
		nextNode = curNode->next;
		free(curNode);
		curNode = nextNode;
	}	
	initList(list);
}

BOOL insertItem(LINKED_LIST *list, LPVOID data)
{
	LINKED_LIST_NODE *llNode = (LINKED_LIST_NODE *)malloc(sizeof(LINKED_LIST_NODE));
	
	if(llNode)
	{
		llNode->data = data;
		llNode->next = NULL;		

		if(!list->head && !list->tail)
		{
			llNode->prev = NULL;
			list->head = llNode;
			list->tail = llNode;
		}
		else
		{
			llNode->prev = list->tail;
			list->tail->next = llNode;
			list->tail = llNode;
		}
		return TRUE;
	}
	return FALSE;
}



LPVOID findItem(LINKED_LIST *list, FIND_ITEM_FUNC fi, LPVOID toFind)
{
	LINKED_LIST_NODE *curNode = list->head;

	while(curNode)
	{
		if(fi(curNode->data, toFind))
		{
			return curNode->data;
		}
		curNode = curNode->next;
	}
	return NULL;
}


// addr of nodedata will be returned, so caller can free it
LPVOID deleteSingleItem(LINKED_LIST *list, FIND_ITEM_FUNC fi, LPVOID toDelete)
{
	LINKED_LIST_NODE *curNode = list->head;

	while(curNode)
	{
		if(fi(curNode->data, toDelete))
		{
			if(curNode == list->head && curNode == list->tail)
			{
				list->head = list->tail = NULL;
			}
			else if(curNode == list->head)
			{
				list->head = curNode->next;
				if(list->head)
					list->head->prev = NULL;
			}
			else if(curNode == list->tail)
			{
				list->tail = curNode->prev;
				if(list->tail)
					list->tail->next = NULL;
			}
			else
			{
				curNode->prev->next = curNode->next;
				curNode->next->prev = curNode->prev;
			}
			LPVOID nodeData = curNode->data;
			free(curNode);

			return nodeData;
		}
		curNode = curNode->next;
	}
	return NULL;
}


// caller must free node data in callback func
int deleteMultipleItems(LINKED_LIST *list, FIND_ITEM_FUNC fi, LPVOID toDelete)
{
	LINKED_LIST_NODE *curNode = list->head;
	int count = 0;

	while(curNode)
	{
		if(fi(curNode->data, toDelete))
		{
			count++;
			if(curNode == list->head && curNode == list->tail)
			{
				list->head = list->tail = NULL;
				free(curNode);
				return count;
			}
			else if(curNode == list->head)
			{
				list->head = curNode->next;
				if(list->head)
					list->head->prev = NULL;
				free(curNode);
				curNode = list->head;
			}
			else if(curNode == list->tail)
			{
				list->tail = curNode->prev;
				if(list->tail)
					list->tail->next = NULL;
				free(curNode);
				return count;
			}
			else
			{
				curNode->prev->next = curNode->next;
				curNode->next->prev = curNode->prev;
				LINKED_LIST_NODE *tempNode = curNode->next;

				free(curNode);
				curNode = tempNode;
			}
		}
		else
			curNode = curNode->next;
	}
	return count;
}



void printList(LINKED_LIST *list, PRINT_ITEM_FUNC pi)
{
	LINKED_LIST_NODE *curNode = list->head;

	while(curNode)
	{
		pi(curNode->data);
		curNode = curNode->next;
	}	
}


void printListBackwards(LINKED_LIST *list, PRINT_ITEM_FUNC pi)
{
	LINKED_LIST_NODE *curNode = list->tail;

	while(curNode)
	{
		pi(curNode->data);
		curNode = curNode->prev;
	}	
}
