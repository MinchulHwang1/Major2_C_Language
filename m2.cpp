/*
* FILE			: m2.cpp
* PROJECT		: Major 2
* PROGRAMMER	: Minchul Hwang
* FIRST VERSION : 2023-02-17
* Description	: This project is just comparing speed of seatching between LL and HT.
					1. Program gets names from 'names file' and saves in LL and HT separately.
					2. User prompt names what user wants to find until user input .
					3. After search, Program print the result of it.
* The functions in this file are used to struct - deleteAllNode(), clearLine(), sordedLinkedList(), djb2(), myHashFunction(), searchLinkedList(), searchForNameTwice()
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma warning(disable: 4996)

#define kNameString		30
#define kHashBusket		127
#define kZeroValue		0
#define kOneValue		1
#define kContinue		1
#define kCompareCount	2
#define kValueOfM		109
typedef struct list {
	char nameInList[kNameString];
	list* next;
	list* prev;
}linkedList;

void clearLine(char* string);
void sortedLinkedList(char* nameString, linkedList** head, linkedList **tail);
unsigned long djb2(const char* str);
int myHashFunction(char* wordString);
linkedList* searchLinkedList(char* searchName, linkedList* linkedLists, int* comparisonCount);
void searchForNameTwice(char* searchName, linkedList* linkedLists, linkedList* hashTable[], int comparisonCount[kCompareCount]);
void deleteAllNode(linkedList* head);

int main() {
	FILE* name = NULL;								// Decalre for file open
	linkedList* headOfList = NULL;					// Linked List of saving name as ascending order
	linkedList* tailOfList = NULL;					// the Tail's of Linked list
	char nameString[kNameString] = {};				// Name string
	linkedList* headOfArray[kHashBusket] = {};		// Hash Table of saveing name as ascending order(buskets : 127)
	linkedList* tailOfArray[kHashBusket] = {};		// Tail of hash table
	int retValHash = kZeroValue;					
	int totalCount = kZeroValue;
	int countOfLL = kZeroValue;
	int countOfHT = kZeroValue;
	int compareCount[kCompareCount] = {};


	// File open
	name = fopen("names.txt", "rt");
	if (name == NULL) {
		printf("there is no File\n");
		return 0;
	}
	while(feof(name) == 0) {			// Get names until file ending
		fgets(nameString, sizeof(nameString), name);
		clearLine(nameString);

		// Put into LL
		sortedLinkedList(nameString, &headOfList, &tailOfList);
		
		// Put into Hash Table
		retValHash = myHashFunction(nameString);
		sortedLinkedList(nameString, &headOfArray[retValHash], &tailOfArray[retValHash]);
		
		if (feof(name) != 0) {
			break;
		}
		if (ferror(name) != 0) {
			printf("Error to read file\n");
		}
	}
	if (fclose(name) != 0) {
		printf("File closing error!\n");
	}

	
	// Start search name until user input .
	while (kContinue) {
		// Get name from user
		fgets(nameString, sizeof(nameString), stdin);
		clearLine(nameString);
		
		// Compare until user input .
		if (strcmp(nameString, ".") != 0) {
			searchForNameTwice(nameString, headOfList, headOfArray, compareCount);
			totalCount++;
			countOfLL += compareCount[kZeroValue];
			countOfHT += compareCount[kOneValue];
		}
		else {
			break;
		}
	}

	printf("\n");
	printf("\tTotal Number of Searches : %d\n", totalCount);
	printf("\tTotal Number of Comparisons in Linked List : %d\n", countOfLL);
	printf("\tTotal Number of Comparisons in Hash Table : %d\n", countOfHT);

	// Free all allocated memory
	deleteAllNode(headOfList);
	for (int i = kZeroValue; i < kHashBusket; i++) {
		deleteAllNode(headOfArray[i]);
	}

	return 0;
}


/*
* Function		: deleteAllNode
* Description	: This function delete all node in the list.
* Parameters	: *head		linkedList		the head pointer of the list
* Returns		: NONE
*/
void deleteAllNode(linkedList* head) {
	linkedList* currentNode = head;
	linkedList* ptr = NULL;
	while (currentNode != NULL) {
		ptr = currentNode;
		currentNode = currentNode->next;
		free(ptr);
	}
}


/*
* Function		: searchForNameTwice
* Description	: This function calls searchLinkedList function to find the name in each list.
*				  It prints appropriate message if there is a name or not.
* Parameters	: *searchName							char			A pointer of name string which user wants to find
*				  *linkedLists							linkedList		A struct pointer which has pure linked list which has name of list as ascending order
*			      *hashTable[]							linkedList		A struct array pointer which is hash table has list of name of list as ascending order
*			      *comparisonCount[kCompareCount]		int				A point of int to compare of count number between LL and HT
* Returns		: NULL
*/
void searchForNameTwice(char* searchName, linkedList* linkedLists, linkedList* hashTable[], int comparisonCount[kCompareCount]) {
	linkedList* currNodeList = NULL;
	linkedList* currNodeArray = NULL;
	int retValHash = kZeroValue;

	// Search in LL
	currNodeList = searchLinkedList(searchName, linkedLists, &comparisonCount[kZeroValue]);

	// Search in HT
	retValHash = myHashFunction(searchName);
	currNodeArray = searchLinkedList(searchName, hashTable[retValHash], &comparisonCount[kOneValue]);
	
	printf("\n");
	
	if (currNodeList != NULL) {
		printf("\t%s was found in the linked list in %d comparisons.\n", searchName, comparisonCount[kZeroValue]);
		printf("\t%s was found in the hash table busket in %d comparisons.\n", searchName, comparisonCount[kOneValue]);
	}
	else  {
		printf("\t%s was NOT found in the linked list in %d comparisons.\n", searchName, comparisonCount[kZeroValue]);
		printf("\t%s was NOT found in the hash table busket in %d comparisons.\n", searchName, comparisonCount[kOneValue]);
	}

	printf("\n");
}


/*
* Function		: searchLinkedList
* Description	: This function search the name within the list as parameter
* Parameter		: *searchName			char			A pointer of char string to find in list
*				  *linkedLists			linkedList		A pointer of list that uicser wants to find name
*				  *comparisonCount		int				A point of int to save count number for comparison
* Returns		: NULL			If there is no matched name in the list
*				  currNode		the node has the matched name
*/
linkedList* searchLinkedList(char* searchName, linkedList* linkedLists, int* comparisonCount) {
	linkedList *currNode = linkedLists;
	int count = kZeroValue;
	char c = searchName[kZeroValue];
	
	// If the current node is NULL
	if (currNode == NULL) {
		*comparisonCount = count;
		return NULL;
	}	
	
	// Search the name from 'a' to 'l'
	if ((int)c < kValueOfM) {
		// Search name until start with initial of searching name
		do {
			count++;
			*comparisonCount = count;
			if (strncmp(currNode->nameInList, searchName, sizeof(searchName)) == 0) {
				break;
			}
			else {
				currNode = currNode->next;
			}
		} while ((int)currNode->nameInList[kZeroValue] <= (int)c);  
	
		if ((int)currNode->nameInList[kZeroValue] >= (int)c+1) { 
			*comparisonCount = count;
			return NULL;
		}
		else
			return currNode;
	}
	// Search the name from 'z' to 'm'
	else {
		// Search name until start with initial till it reaches m from 'z'
		do {
			count++;
			*comparisonCount = count;
			if (strncmp(currNode->nameInList, searchName, sizeof(searchName)) == 0) {
				break;
			}
			else {
				currNode = currNode->prev;
			}
		} while ((int)currNode->nameInList[kZeroValue] >= (int)c);
		
		if ((int)currNode->nameInList[kZeroValue] <= (int)c-1) { 
			*comparisonCount = count;
			return NULL;
		}
		else
			return currNode;
	}
}


/*
* Function		: myHashFunction
* Description	: This function get the string which user want to save, send that word into hash function.
					and calculate appropriate number to return.
* Parameters	:	*wordString		char		which has the word string user want to store.
* Returns		: retCode			int			mod of 11 from hash function
*/
int myHashFunction(char* wordString) {
	int retCode = djb2(wordString) % kHashBusket;		// mod of 127

	return retCode;
}


/*
* Function		: djb2
* Description	: This function is hash function calculates from string into appropriate number.
* Parameters	: *str		const char			the word string from myHashFunction.
* Returns		: hash		unsigned long		a calculated number
*/
unsigned long djb2(const char* str) {
	unsigned long hash = 5381;
	int	c = 0;
	while ((c = *str++) != '\0') {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}


/*
* Function		: sortedLinkedList
* Description	: This function sorts as circular Linked list the name as ascending order until there is no input
* Parameters	: *nameString		char			A pointer of nameString which is stored in list
*				  **head			linkedList		A double ppinter of list which can save names
* Returns		: NONE
*/
void sortedLinkedList(char* nameString, linkedList** head, linkedList** tail) {
	linkedList* currNode = NULL;
	
	currNode = (linkedList*)malloc(sizeof(linkedList));
	
	if (currNode == NULL) {
		printf("There is no memory!\n");
		return;
	}
	strncpy(currNode->nameInList, nameString, strlen(nameString)+1);
	currNode->next = currNode->prev = NULL;
	
	if (*head == NULL) {
		*head = *tail = currNode;
	}
	else if(strncmp((*head)->nameInList, currNode->nameInList, kNameString) > 0) {
		currNode->next = *head;
		(*head)->prev = currNode;
		*head = currNode;
		(*head)->prev = *tail;		// Make a node as circular

	}
	else {
		linkedList* prevNode = *head;
		linkedList* nextNode = (*head)->next;
		
		while (nextNode != NULL) {
			if (strncmp(nextNode->nameInList, currNode->nameInList, kNameString) > 0) {
				break;
			}
			prevNode = nextNode;
			nextNode = nextNode->next;
		}

		currNode->prev = prevNode;
		currNode->next = nextNode;
		prevNode->next = currNode;

		if (nextNode == NULL) {
			*tail = currNode;
			(*head)->prev = *tail;		// Make a node as circular
		}
		else {
			nextNode->prev = currNode;
		}
	}
}


/*
* Function		: clearNewLine()
* Description	: If the received data is blank or contains \n, it is removed.
* Parameters	: *string		char		the pointer has string from user
* Returns		: None
*/
void clearLine(char* string) {
	char* searchNewLine = strchr(string, '\n');
	if (searchNewLine != NULL) {
		*searchNewLine = '\0';
	}
}