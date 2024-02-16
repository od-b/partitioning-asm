#include <stdio.h>
#include <stdlib.h> // included for rand()
#include <limits.h>

/* 
 * Variant of main with debugging prints, assertions, etc.
 * obviously not meant for profiling
*/

/* n arrays to generate and sort */
#define N_RUNS ( 100 )

/* parameters for array randomization */
#define ARR_SIZE ( 100 )
#define ARR_VAL_LIM ( 100 )

/* excessive/detailed printing options */
#define PASS_PRINT ( 0 )
#define MISC_PRINT ( 0 )

/* format out-color */
#define F_REG(ansi_col) fprintf(stderr, "\e[0;%s", ansi_col)
/* format out-color, bold */
#define F_BLD(ansi_col) fprintf(stderr, "\e[1;%s", ansi_col)
/* reset out-color */
#define F_RES fprintf(stderr, "\e[0m")
/* msg src. formatted to redirect on cmd+click in terminal */
#define P_SRC(pref) fprintf(stderr, "[%c][%s:%d] ", pref, __FILE__, __LINE__)

/* info, no line, magenta */
#define PASS do { fprintf(stderr, "\e[0;32m OK \e[0m\n"); } while(0)
#define TELL(...) do { F_REG("37m"); fprintf(stderr, __VA_ARGS__); F_RES; } while(0)
/* info, yellow */
#define SHOUT(...) do { F_BLD("93m"); P_SRC('I'); F_RES; fprintf(stderr, __VA_ARGS__); } while(0)
/* error, red */
#define SCREAM(...) do { F_BLD("31m"); P_SRC('E'); F_RES; fprintf(stderr, __VA_ARGS__); } while(0)
/* error && exit */
#define PANIC(...) do { SCREAM(__VA_ARGS__); exit(1); } while(0)


extern int asm_partition(int *arr, int low, int high);

/* assign randomized values to array elements */
void populate_array(int *arr) {
    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = (rand() % ARR_VAL_LIM);
    }
}

/* given an array with n elements, prints it */
void print_arr(int *arr, int n) {
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%d, ", arr[i]);
    }
    fprintf(stderr, "\n");
}

/* returns whether an array with n elements is sorted */
int is_sorted(int *arr, int n) {
    for (int i = 0; i < n-1; i++) {
        if (arr[i] > arr[i+1]) {
            return 0;
        }
    }
    return 1;
}

/* debug: asserts whether an array is sorted */
void assert_sorted(int *arr, int n) {
    /* for all elements, assert that element to the right is equal or greater */
    if (is_sorted(arr, n)) {
        if (PASS_PRINT) {
            TELL("PASS: assert_sorted\n");
        }
    } else {
        print_arr(arr, (int)ARR_SIZE);
        PANIC("FAIL: assert_sorted\n");
    }
}

void assert_partitioned(int *arr, int n, int pivot_i) {
    for (int i = 0; i < n; i++) {
        if (i < pivot_i) {
            if (arr[i] > arr[pivot_i]) {
                print_arr(arr, ARR_SIZE);
                PANIC("FAIL: partitioning, pivot_i=%d @ arr[%d]\n\n", arr[pivot_i], pivot_i);
            }
        } else if (i > pivot_i) {
            if (arr[i] < arr[pivot_i]) {
                print_arr(arr, ARR_SIZE);
                PANIC("FAIL: partitioning, pivot_i=%d @ arr[%d]\n\n", arr[pivot_i], pivot_i);
            }
        }
    }
    if (PASS_PRINT) {
        print_arr(arr, ARR_SIZE);
        TELL("PASS: (new index of) pivot => arr[%d]=$%d\n\n", pivot_i, arr[pivot_i]);
    }
}

/* assert: run partition func once per array for $N_RUNS randomized arrays */
void assert_partition_func() {
    int arr[(size_t)ARR_SIZE];

    int low = 0;
    int high = ARR_SIZE-1;
    for (int i = 0; i < N_RUNS; i++) {
        populate_array(arr);
        if (MISC_PRINT) {
            print_arr(arr, ARR_SIZE);
        }
        int pivot_i = asm_partition(arr, low, high);
        assert_partitioned(arr, ARR_SIZE, pivot_i);
    }

    if (PASS_PRINT) {
        SHOUT("PASS: partition_func -- runs=%d (separate); arr_len=%d\n",
            (int)N_RUNS, (int)ARR_SIZE);
    }
}

/* 
 * partitions array|slice using value at high index as pivot
 * low and high can be considered start/stop indices
 * returns the 'new' position of value previously at "high" index
 */
int partition(int *arr, int low, int high) {
    int i = low;    // continuous index [--->]
    int j = low-1;  // low moving index [->]

    while (i < high) {
        /* if current element[i] is smaller than pivot, elems 'i' and 'j' */
        if (arr[i] < arr[high]) {
            j++;
            low = arr[j];   // use low as temp since it's free
            arr[j] = arr[i];
            arr[i] = low;
        }
        i++;
    }

    /* swap pivot with leftmost larger element */
    j++;
    low = arr[j];
    arr[j] = arr[high];
    arr[high] = low;

    /* return new index of pivot */
    return j;
}

/* canonical, recursive quicksort */
void quicksort(int *arr, int low, int high) {
    if (low < high) {
        if (MISC_PRINT) {
            TELL("\ncall: quicksort(%d, %d)\n", low, high);
        }
        int pivot_i = asm_partition(arr, low, high);

        // print_arr(arr, ARR_SIZE);
        assert_partitioned(arr, ARR_SIZE, pivot_i);

        quicksort(arr, low, pivot_i-1);     // sort left partition
        quicksort(arr, pivot_i+1, high);    // sort right partition
    }
}


/* naive test controlling that asm_partition does not alter array */
void assert_no_mutation() {
    int LEN = 100;
    int arr[(size_t)LEN];
    int count[(size_t)LEN];

    for (int i = 0; i < LEN; i++) {
        count[i] = 0;
    }

    for (int r = 0; r < N_RUNS; r++) {
        for (int i = 0; i < LEN; i++) {
            int num = (rand() % LEN);
            arr[i] = num;
            count[num]++;
        }

        quicksort(arr, 0, LEN-1);

        /* assert that each value still has the same count */
        for (int i = 0; i < LEN; i++) {
            int num = arr[i];
            count[num]--;
            if (count[num] < 0) {
                PANIC("array was mutated by the sort\n");
            }
        }

        /* pidgeonhole principle, if no count is less than 0, 
        * the content of the array is identical and all counts == 0
        */
    }
}

/* bench: run sort(s) using a large array containing arbitrary integers */
void assert_sorting_func() {
    int arr[(size_t)ARR_SIZE];

    for (int i = 0; i < N_RUNS; i++) {
        populate_array(arr);
        if (MISC_PRINT) {
            SHOUT("generated array: ");
            print_arr(arr, (int)ARR_SIZE);
        }
        quicksort(arr, 0, ((int)ARR_SIZE)-1);
        assert_sorted(arr, (int)ARR_SIZE);
    }

    // printf("visual verification of sorted arr: \n");
    // print_arr(arr, (int)ARR_SIZE);
}

void assert_pre_sorted() {
    int arr[(size_t)ARR_SIZE];

    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = i;
    }
    assert_sorted(arr, (int)ARR_SIZE);
    quicksort(arr, 0, ((int)ARR_SIZE)-1);
    assert_sorted(arr, (int)ARR_SIZE);
}

void run_assertions() {
    SHOUT("Running a series of tests and assertions:\n");
    TELL("* verifying partition_func separate from sort ...");
    assert_partition_func();
    PASS;
    TELL("* verifying the sorting implementation ...");
    assert_sorting_func();
    PASS;
    TELL("* verifying sort on array that is already sorted ...");
    assert_pre_sorted();
    PASS;
    TELL("* verifying that sort does not mutate arrays ...");
    assert_no_mutation();
    PASS;
    SHOUT("PASS: all assertions succesful\n");
}

int main(int argc, char **argv) {
    if (abs(ARR_VAL_LIM) >= RAND_MAX) {
        PANIC("ARR_VAL_LIM (%d) should be less than RAND_MAX (%d). LIM=0 => RAND_MAX",
            ARR_VAL_LIM, RAND_MAX);
    }
    srand(1);
    run_assertions();
    return 0;
}
