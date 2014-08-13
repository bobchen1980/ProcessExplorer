#ifndef _PORTSEQREQLIST_
#define _PORTSEQREQLIST_


typedef struct _LINKED_LIST_NODE
{
	struct _LINKED_LIST_NODE *prev;
	struct _LINKED_LIST_NODE *next;
	LPVOID data;
} LINKED_LIST_NODE;


typedef struct _LINKED_LIST
{
	LINKED_LIST_NODE *head;
	LINKED_LIST_NODE *tail;
} LINKED_LIST;


typedef void (*PRINT_ITEM_FUNC)(LPVOID data);
typedef BOOL (*FIND_ITEM_FUNC)(LPVOID data, LPVOID toFind);
typedef BOOL (*PROCESS_ITEM_FUNC)(LPVOID nodeData, LPVOID procData);

void initList(LINKED_LIST *list);
void freeList(LINKED_LIST *list);
BOOL insertItem(LINKED_LIST *list, LPVOID data);
void printList(LINKED_LIST *list, PRINT_ITEM_FUNC pi);
LPVOID findItem(LINKED_LIST *list, FIND_ITEM_FUNC fi, LPVOID toFind);
LPVOID deleteSingleItem(LINKED_LIST *list, FIND_ITEM_FUNC fi, LPVOID toDelete);
int deleteMultipleItems(LINKED_LIST *list, FIND_ITEM_FUNC fi, LPVOID toDelete);
void printListBackwards(LINKED_LIST *list, PRINT_ITEM_FUNC pi);

#endif