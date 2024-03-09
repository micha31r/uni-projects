/* A simplified learned index implementation:
 *
 * Skeleton code written by Jianzhong Qi, April 2023
 * Edited by: Michael Ren, April 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STAGE_NUM_ONE 1						  /* stage numbers */ 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_HEADER "Stage %d\n==========\n" /* stage header format string */

#define DATASET_SIZE 100					  /* number of input integers */
#define DATA_OUTPUT_SIZE 10					  /* output size for stage 1 */

#define BS_NOT_FOUND (-1)					  /* used by binary search */
#define BS_FOUND 0

typedef int data_t; 				  		  /* data type */

/* custom structure to store data domains (task 3.3.4) */
struct Map {
	int a;
	int b;
	data_t max;
};

/* create a new type for the structure */
typedef struct Map map_t; 

/****************************************************************/

/* function prototypes */
void print_stage_header(int stage_num);
int cmp(data_t *x1, data_t *x2);
void swap_data(data_t *x1, data_t *x2);
void partition(data_t dataset[], int n, data_t *pivot, int *first_eq, int *first_gt);
void quick_sort(data_t dataset[], int n);

/* adapted from the binary search function skeleton */
int search_mappings(map_t mappings[], int lo, int hi, data_t *key, int *locn);
int search_key(data_t dataset[], int lo, int hi, data_t *key, int *locn);

/* Stages */
void stage_one(data_t dataset[]);
void stage_two(data_t dataset[]);
void stage_three(data_t dataset[], map_t mappings[], int *mps_len, int *max_err);
void stage_four(data_t dataset[], map_t mappings[], int mps_len, int max_err);

/* add your own function prototypes here */
void compute_ab(data_t dataset[], int y0, int y1, int *a, int *b);
float compute_f_key(data_t key, float a, float b);
int compute_err(data_t dataset[], int index, int a, int b);
int max(int a, int b);
int min(int a, int b);

/****************************************************************/

/* main function controls all the action */
int main(int argc, char *argv[]) {
	/* to hold all input data */
	data_t dataset[DATASET_SIZE];
	int max_err;

	/* to hold the mapping functions */
	map_t mappings[DATASET_SIZE];
	int mps_len = 0; /* length of mappings array */

	/* stage 1: read and sort the input */
	stage_one(dataset); 
	
	/* stage 2: compute the first mapping function */
	stage_two(dataset);
	
	/* stage 3: compute more mapping functions */ 
	stage_three(dataset, mappings, &mps_len, &max_err);
	
	/* stage 4: perform exact-match queries */
	stage_four(dataset, mappings, mps_len, max_err);
	
	/* all done; take some rest */
	return 0;
}

/****************************************************************/

/* compute a and b from two elements in the dataset array */
void compute_ab(data_t dataset[], int y0, int y1, int *a, int *b) {
	/* y0, y1 are the index positions */
	/* x0, x1 are the element values */
	data_t x0 = dataset[y0];
	data_t x1 = dataset[y1];
	if (x0 == x1) {
		*a = y0;
		*b = 0;
	} else {
		*a = y0 * x1 - y1 * x0;
		*b = x1 - x0;
	}
}

/* compute f(key) value */
float compute_f_key(data_t key, float a, float b) {
	return b == 0 
		? a
		: ((float) key + a) / b;
}

/* compute the prediction error */
int compute_err(data_t dataset[], int index, int a, int b) {
	int f_key = b == 0 
		? a 
		: ceil(compute_f_key(dataset[index], a, b));
	return abs(f_key - index);
}

/* return the bigger of two integers */
int max(int a, int b) {
	return a > b 
		? a 
		: b;
}

/* return the smaller of two integers */
int min(int a, int b) {
	return a < b 
		? a 
		: b;
}

/* stage 1: read and sort the input */
void stage_one(data_t dataset[]) {
	/* print stage header */
	print_stage_header(STAGE_NUM_ONE);

	/* read input numbers */
	int num = 0;
	for (int i = 0; i < DATASET_SIZE; i++) {
		scanf("%d", &num);
		dataset[i] = num;
	}

	/* sort the dataset */
	quick_sort(dataset, DATASET_SIZE);
	
	/* print sorted items */
	printf("First %d numbers:", DATA_OUTPUT_SIZE);
	for (int i = 0; i < DATA_OUTPUT_SIZE; i++) {
		printf(" %d", dataset[i]);
	}

	printf("\n\n");
}

/* stage 2: compute the first mapping function */
void stage_two(data_t dataset[]) {
	/* add code for stage 2 */
	/* print stage header */
	print_stage_header(STAGE_NUM_TWO);

	int a, b;
	compute_ab(dataset, 0, 1, &a, &b);

	/* compute the maximum prediction error */
	int biggest_err = 0;
	int biggest_index = 0; /* index position of the corresponding dataset element */
	for (int i = 0; i < DATASET_SIZE; i++) {
		int err = compute_err(dataset, i, a, b);
		if (err > biggest_err) {
			biggest_err = err;
			biggest_index = i;
		}
	}

	/* print the maximum prediction error and the corresponding dataset element */
	printf("Maximum prediction error: %d\n", biggest_err);
	printf("For key: %d\n", dataset[biggest_index]);
	printf("At position: %d\n", biggest_index);
	printf("\n");
}

/* stage 3: compute more mapping functions */ 
void stage_three(data_t dataset[], map_t mappings[], int *mps_len, int *max_err) {
	/* print stage header */
	print_stage_header(STAGE_NUM_THREE);

	/* read input */
	scanf("%d", max_err);
	printf("Target maximum prediction error: %d\n", *max_err);

	int a, b;
	compute_ab(dataset, 0, 1, &a, &b);

	for (int i = 2; i < DATASET_SIZE; i++) {
		int err = compute_err(dataset, i, a, b);
		/* instead of using an auxiliary variable:
		 * set the default value to -1 to represent negative state
		 * since all inputs are positive integers */
		data_t max_elem = -1;

		if (err > *max_err) {
			max_elem = dataset[i - 1];
		} else if (i == DATASET_SIZE - 1) { /* last element is always covered */
			max_elem = dataset[i];
		}

		/* print and store the maximum element covered */
		if (max_elem >= 0) {
			printf("Function %2d: a = %4d, b = %3d, max element = %3d\n", *mps_len, a, b, max_elem);
			mappings[*mps_len].a = a;
			mappings[*mps_len].b = b;
			mappings[*mps_len].max = max_elem;
			(*mps_len)++;
		}

		/* re-calculate a and b (after the values have been stored) */
		if (err > *max_err) {
			if (i == DATASET_SIZE - 2) {
				/* special case when there is only 1 element left to process */
				a = DATASET_SIZE - 1;
				b = 0;
			} else {
				compute_ab(dataset, i, i + 1, &a, &b);
			}
		}
	}

	printf("\n");
}

/* stage 4: perform exact-match queries */
/* algorithms are fun */
void stage_four(data_t dataset[], map_t mappings[], int mps_len, int max_err) {
	/* print stage header */
	print_stage_header(STAGE_NUM_FOUR);

	data_t key = 0;
	while (scanf("%d", &key) == 1) { /* read remaining inputs */
		printf("Searching for %d:\n", key);

		/* check if key is within the dataset's range */
		printf("Step 1: ");
		if (key < dataset[0] || key > dataset[DATASET_SIZE - 1]) {
			printf("not found!\n");
			continue;
		}
		printf("search key in data domain.\n");

		/* find the data domain that is valid for key */
		printf("Step 2:");
		int map_index = 0;
		search_mappings(mappings, 0, mps_len, &key, &map_index);
		printf("\n");

		/* find the index of key in dataset */
		printf("Step 3:");
		int key_index = 0;
		int f_key = ceil(compute_f_key(key, mappings[map_index].a, mappings[map_index].b));
		int found = search_key(
			dataset,
			max(0, f_key - max_err),
			min(DATASET_SIZE-1, f_key + max_err) + 1,
			&key,
			&key_index
		);
		if (found == 0) {
			printf(" @ dataset[%d]!\n", key_index);
		} else {
			printf(" not found!\n");
		}
	}
	
	printf("\n");
}

/****************************************************************/
/* functions provided, adapt them as appropriate */

/* print stage header given stage number */
void print_stage_header(int stage_num) {
	printf(STAGE_HEADER, stage_num);
}

/* data swap function used by quick sort, adpated from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/sortscaffold.c
*/
void swap_data(data_t *x1, data_t *x2) {
	data_t t;
	t = *x1;
	*x1 = *x2;
	*x2 = t;
}

/* partition function used by quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c 
*/
void partition(data_t dataset[], int n, data_t *pivot, int *first_eq, int *first_gt) {
	int next=0, fe=0, fg=n, outcome;
	
	while (next<fg) {
		if ((outcome = cmp(dataset+next, pivot)) < 0) {
			swap_data(dataset+fe, dataset+next);
			fe += 1;
			next += 1;
		} else if (outcome > 0) {
			fg -= 1;
			swap_data(dataset+next, dataset+fg);
		} else {
			next += 1;
		}
	}
	
	*first_eq = fe;
	*first_gt = fg;
	return;
}

/* quick sort function, adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/quicksort.c 
*/
void quick_sort(data_t dataset[], int n) {
	data_t pivot;
	int first_eq, first_gt;
	if (n<=1) {
		return;
	}
	/* array section is non-trivial */
	pivot = dataset[n/2]; // take the middle element as the pivot
	partition(dataset, n, &pivot, &first_eq, &first_gt);
	quick_sort(dataset, first_eq);
	quick_sort(dataset + first_gt, n - first_gt);
}

/* comparison function used by binary search and quick sort, from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c 
*/
int cmp(data_t *x1, data_t *x2) {
	return (*x1-*x2);
}

/* binary search between dataset[lo] and dataset[hi-1], adapted from
   https://people.eng.unimelb.edu.au/ammoffat/ppsaa/c/binarysearch.c 
*/

/* binary search addaped to find the mapping function */
int search_mappings(map_t mappings[], int lo, int hi, data_t *key, int *locn) {
	int mid = (lo+hi)/2, outcome;

	/* must accept the last mismatch as our answer because
	 * the key may not be equal to the data domain's max value.
	 * for example: 18 within the data domain 0 where the max is 64, but 8 doesn't equal to 64 */
	if (lo>=hi) {
		*locn = mid;
		return BS_FOUND;
	}

	/* print the value that key is being compared to */
	printf(" %d", mappings[mid].max);
	
	if ((outcome = cmp(key, &(mappings+mid)->max)) < 0) {
		return search_mappings(mappings, lo, mid, key, locn);
	} else if (outcome > 0) {
		return search_mappings(mappings, mid+1, hi, key, locn);
	} else {
		*locn = mid;
		return BS_FOUND;
	}
}

/* binary search adapted to find element in dataset */
int search_key(data_t dataset[], int lo, int hi, data_t *key, int *locn) {
	int mid = (lo+hi)/2, outcome;

	/* if key is in dataset, it is between dataset[lo] and dataset[hi-1] */
	if (lo>=hi) {
		return BS_NOT_FOUND;
	}

	/* print the value that key is being compared to */
	printf(" %d", dataset[mid]);
	
	if ((outcome = cmp(key, dataset+mid)) < 0) {
		return search_key(dataset, lo, mid, key, locn);
	} else if (outcome > 0) {
		return search_key(dataset, mid+1, hi, key, locn);
	} else {
		*locn = mid;
		return BS_FOUND;
	}
}
