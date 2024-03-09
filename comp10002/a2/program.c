/* A simplified community detection algorithm:
 *
 * Skeleton code written by Jianzhong Qi, May 2023
 * Edited by: Michael Ren (09 May 2023)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

/* stage numbers */
#define STAGE_NUM_ONE 1
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4

/* stage header format string */
#define STAGE_HEADER "Stage %d\n==========\n"

#define MAX_USERS 50
#define MAX_TAGS 10
#define MAX_TAG_LENGTH 21

/* data_t represent a word */
typedef char data_t[MAX_TAG_LENGTH];

typedef struct {
	int id;
	int year;
	int tag_count;
	data_t tags[MAX_TAGS];
} user_t;

/* linked list type definitions below, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/
typedef struct node node_t;

struct node {
	data_t data;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
} list_t;

/****************************************************************/

/* function prototypes */
list_t *make_empty_list(void);
void free_list(list_t *list);
void print_list(list_t *list);
list_t *insert_unique_in_order(list_t *list, data_t value);

void print_stage_header(int stage_num);

void stage_one(user_t users[], int *user_count);
void stage_two(user_t users[], int user_count, int frn_mtx[][MAX_USERS]);
void stage_three(int user_count, int frn_mtx[][MAX_USERS], float soc_mtx[][MAX_USERS]);
void stage_four(user_t users[], int user_count, int frn_mtx[][MAX_USERS],
	float soc_mtx[][MAX_USERS]);

/* add your own function prototypes here */

int array_contains(int arr[], int count, int value);
void print_tags(user_t *user);
void read_matrix(int frn_mtx[][MAX_USERS], int size);
int get_friends(int id, int user_count, int frn_mtx[][MAX_USERS], int ret[]);
int sum_intersection(int arr1[], int count1, int arr2[], int count2);
int sum_union(int arr1[], int count1, int arr2[], int count2);
float compute_soc(int user1_id, int user2_id, int user_count, int frn_mtx[][MAX_USERS]);
list_t *insert_tags(list_t *tags, user_t *user);

/****************************************************************/

/* algorithms are fun */
/* main function controls all the action; modify if needed */
int
main(int argc, char *argv[]) {
	user_t users[MAX_USERS];
	int user_count = 0;
	int frn_mtx[MAX_USERS][MAX_USERS]; /* friendship matrix */
	float soc_mtx[MAX_USERS][MAX_USERS]; /* strength of connection matrix */

	/* stage 1: read user profiles */
	stage_one(users, &user_count);
	
	/* stage 2: compute the strength of connection between u0 and u1 */
	stage_two(users, user_count, frn_mtx);
	
	/* stage 3: compute the strength of connection for all user pairs */
	stage_three(user_count, frn_mtx, soc_mtx);
	
	/* stage 4: detect communities and topics of interest */
	stage_four(users, user_count, frn_mtx, soc_mtx);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* check if an array contains a value */
int array_contains(int arr[], int count, int value) {
	for (int i = 0; i < count; i++) {
		if (arr[i] == value) {
			return 1;
		}
	}
	return 0;
}

/* print hashtags given a user */
void print_tags(user_t *user) {
	int count = user->tag_count;
	for (int i = 0; i < count; i++) {
		printf("#%s", user->tags[i]);
		if (i < count - 1) {
			printf(" ");
		}
	}
	printf("\n");
}

/* read the matrix input */
void read_matrix(int frn_mtx[][MAX_USERS], int size) {
	char c;
	for (int row = 0; row < size; row++) {
		int col = 0;
		while ((c = getchar()) != '\n') {
			if (isdigit(c)) {
				int num = c - '0'; /* convert char code to a number */
				frn_mtx[row][col] = num;
				col++;
			}
		}
	}
}

/* get all friends given a user */
int get_friends(int id, int user_count, int frn_mtx[][MAX_USERS], int ret[]) {
	int count = 0;
	for (int i = 0; i < user_count; i++) {
		int value = frn_mtx[id][i];
		if (value) {
			ret[count] = i;
			count++;
		}
	}
	return count;
}

/* return the number of items for the intersection of two arrays */
int sum_intersection(int arr1[], int count1, int arr2[], int count2) {
	int n = 0;
	for (int i = 0; i < count1; i++) {
		if (array_contains(arr2, count2, arr1[i])) {
			n++;
		}
	}
	return n;
}

/* return the number of items for the union of two arrays */
int sum_union(int arr1[], int count1, int arr2[], int count2) {
	int n = count1;
	for (int i = 0; i < count2; i++) {
		if (!array_contains(arr1, count1, arr2[i])) {
			n++;
		}
	}
	return n;
}

/* compute the strength of connection for a given user */
float compute_soc(int user1_id, int user2_id, int user_count, int frn_mtx[][MAX_USERS]) {
	/* get the friends of two users */
	int friends1[user_count];
	int friends2[user_count];
	int f1_count = get_friends(user1_id, user_count, frn_mtx, friends1);
	int f2_count = get_friends(user2_id, user_count, frn_mtx, friends2);

	/* find the union and intersections */
	int intersect_count = sum_intersection(friends1, f1_count, friends2, f2_count);
	int union_count = sum_union(friends1, f1_count, friends2, f2_count);

	/* calculate the strength of connection */
	if (frn_mtx[user1_id][user2_id]) {
		return (float) intersect_count / (float) union_count;
	}

	return 0;
}

/* wrapping function to uniquely insert a list of tags */
list_t *insert_tags(list_t *tags, user_t *user) {
	for (int i = 0; i < user->tag_count; i++) {
		tags = insert_unique_in_order(tags, user->tags[i]);
	}
	return tags;
}

/* stage 1: read user profiles */
void 
stage_one(user_t users[], int *user_count) {
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);
	user_t *max_tag_user;
	int max_tag_count = 0;

	int id, year;
	while (scanf("u%d %d", &id, &year)) {
		/* initialise user */
		user_t *user = &users[id];
		user->id = id;
		user->year = year;
		user->tag_count = 0;
		(*user_count)++;

		/* read tags */
		char tag[MAX_TAG_LENGTH];
		while (scanf(" #%s", tag)) {
			strcpy(user->tags[user->tag_count], tag);
			user->tag_count++;
		}

		/* find the user with most hashtags */
		if (user->tag_count > max_tag_count) {
			max_tag_user = user;
			max_tag_count = user->tag_count;
		}
	}

	printf("Number of users: %d\n", *user_count);
	printf("u%d has the largest number of hashtags:\n", max_tag_user->id);
	print_tags(max_tag_user);
	printf("\n");
}

/* stage 2: compute the strength of connection between u0 and u1 */
void 
stage_two(user_t users[], int user_count, int frn_mtx[][MAX_USERS]) {
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	/* read the entire matrix */
	read_matrix(frn_mtx, user_count);

	/* compute the strength of connection */
	float strength = compute_soc(0, 1, user_count, frn_mtx);
	
	printf("Strength of connection between u0 and u1: %4.2f\n", strength);
	printf("\n");
}

/* stage 3: compute the strength of connection for all user pairs */
void 
stage_three(int user_count, int frn_mtx[][MAX_USERS], float soc_mtx[][MAX_USERS]) {
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);

	/* compute te strength of connection for every pair of users */
	for (int i = 0; i < user_count; i++) {
		for (int j = 0; j < user_count; j++) {
			float strength = compute_soc(i, j, user_count, frn_mtx);
			soc_mtx[i][j] = strength;
			printf("%4.2f", strength);
			if (j < user_count - 1) {
				printf(" ");
			}
		}
		printf("\n");
	}

	printf("\n");
}

/* stage 4: detect communities and topics of interest */
void 
stage_four(user_t users[], int user_count, int frn_mtx[][MAX_USERS], float soc_mtx[][MAX_USERS]) {
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

	float ths = 0;
	int thc = 0;
	scanf("%f", &ths);
	scanf("%d", &thc);

	for (int i = 0; i<user_count; i++) {
		int friends[user_count];
		int f_count = get_friends(i, user_count, frn_mtx, friends);
		int cls_friends[user_count];
		int cls_friend_count = 0;

		/* find close friends */
		for (int j = 0; j < f_count; j++) {
			if (soc_mtx[i][friends[j]] > ths) {
				cls_friends[cls_friend_count] = friends[j];
				cls_friend_count++;
			}
		}

		if (cls_friend_count > thc) { /* check if user is a core user */
			printf("Stage 4.1. Core user: u%d; ", i);
			printf("close friends:");

			list_t *tags = make_empty_list();
			user_t *user = &users[i];

			/* insert tags from the core user */
			tags = insert_tags(tags, user);

			for (int k = 0; k < cls_friend_count; k++) {
				int id = cls_friends[k];
				printf(" u%d", id);

				/* insert tags from the close friends */
				user = &users[id];
				tags = insert_tags(tags, user);
			}

			printf("\nStage 4.2. Hashtags:\n");
			print_list(tags);
			free_list(tags);
		}
	}
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/****************************************************************/
/* linked list implementation below, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/listops.c 
*/

/* create an empty list */
list_t
*make_empty_list(void) {
	list_t *list;

	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;

	return list;
}

/* free the memory allocated for a list (and its nodes) */
void
free_list(list_t *list) {
	node_t *curr, *prev;

	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}

	free(list);
}

/* insert a new data element into a linked list, keeping the
	data elements in the list unique and in alphabetical order
*/
list_t
*insert_unique_in_order(list_t *list, data_t value) {
	node_t *new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	strcpy(new->data, value);
	new->next = NULL;

	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		node_t *prev = NULL;
		node_t *curr = list->head;
		while (curr) {
			int cmp = strcmp(value, curr->data);
			if (cmp < 0) {
				/* insert between two nodes */
				if (prev) {
					prev->next = new;
				} else {
					list->head = new;
				}
				new->next = curr;
				break;
			} else if (cmp == 0) {
				/* abort if value already exists */
				free(new);
				new = NULL;
				break;
			}
			prev = curr;
			curr = curr->next;
		}

		/* insert at foot if the value is bigger than the values of all other nodes */
		if (new && !new->next) {
			list->foot->next = new;
			list->foot = new;
		}
	}

	return list;
}

/* print the data contents of a list */
void
print_list(list_t *list) {
	node_t *curr = list->head;
	int count = 0;
	while (curr) {
		printf("#%s", curr->data);

		/* print whitespace */
		count++;
		if (!curr->next || count == 5) {
			printf("\n");
			count = 0;
		} else {
			printf(" ");
		}
		curr = curr->next;
	}
}

/****************************************************************/
/*
	Write your time complexity analysis below for Stage 4.2, 
	assuming U users, C core users, H hashtags per user, 
	and a maximum length of T characters per hashtag:

	Worst case assumptions:
		* 	Every user will have U - 1 close friends,
			which can be approximated to U. This means every user is a core user,
			so C = U
		*	Every tag is unique, so the linked list will have a size of U * H
		* 	Every tag is at the maximum character length
		*  strcmp() has the complexity O(T)
	
	1. Make an empty linked list:
		Empty linked lists do not have any nodes, so it only takes O(1) to create.

	2. Insert tags for a single user: 
		First we need to iterate through the list of tags, which takes O(H) time. 

		For each tag, we need to iterate through the unique linked list and check of existing
		values, which takes a maximum of O(TUH) steps, assuming every tag is unique.

		The combined complexity is:
			O(TUH * H)
		=	O(TUH^2)

	3. Insert tags for the core user:
		We perform step 2 once.

	4. Insert tags for every close friend:
		First we need to iterate through the core user's list of close friends,
		which takes linear time of O(U).

		For each friend, we repeat step 2 once.

		The total complexity is:
			O(U * TUH^2)
		= 	O(T(UH)^2)

	5. Print list:
		To print the list, we iterate through the linked list and print each hashtag which takes O(UHT)

	6. Free list:
		To free the linked list, we free the nodes one-by-one. This takes O(UH)

	Combining everything, the worst case complexity for a single community is:
		O(1 + TUH^2 + T(UH)^2 + UHT + UH)
	=	O(T(UH)^2)

	Multiply by the number of communities (C, which equals to U):
	The combined worst case complexity is:
		O(C * T(UH)^2)
	=	O(U * T(UH)^2)
	=	O(TU^3 * H^2)

	Given constant values of T and H, we can simplify the worst case complexity to:
	=	O(U^3)
*/
