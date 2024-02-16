#include <stdio.h>
#include <stdlib.h> // included for rand()

extern int asm_partition(int *arr, int low, int high);

/* 
 * partitions array|slice using value at high index as pivot
 * low and high can be considered start/stop indices
 * returns the new index of selected pivot
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
        int pivot_i = partition(arr, low, high);
        quicksort(arr, low, pivot_i-1);     // sort left partition
        quicksort(arr, pivot_i+1, high);    // sort right partition
    }
}

/* version with asm_partition */
void asm_quicksort(int *arr, int low, int high) {
    if (low < high) {
        int pivot_i = asm_partition(arr, low, high);
        asm_quicksort(arr, low, pivot_i-1);     // sort left partition
        asm_quicksort(arr, pivot_i+1, high);    // sort right partition
    }
}

inline void populate_array(int *arr, const size_t count) {
    for (size_t i = 0; i < count; i++) {
        arr[i] = rand();
    }
}

inline void populate_array_lim(int *arr, const size_t count, const int val_lim) {
    for (size_t i = 0; i < count; i++) {
        arr[i] = (rand() % val_lim);
    }
}

void asm_run_sort(int *arr, const size_t count, const int val_lim, const int n_runs) {
    /* branch: with or without rand limit */
    if (val_lim > 0) {
        for (int i = 0; i < n_runs; i++) {
            populate_array_lim(arr, count, val_lim);
            asm_quicksort(arr, 0, count-1);
        }
    } else {
        for (int i = 0; i < n_runs; i++) {
            populate_array(arr, count);
            asm_quicksort(arr, 0, count-1);
        }
    }
}

void run_sort(int *arr, const size_t count, const int val_lim, const int n_runs) {
    /* branch: with or without rand limit */
    if (val_lim > 0) {
        for (int i = 0; i < n_runs; i++) {
            populate_array_lim(arr, count, val_lim);
            quicksort(arr, 0, count-1);
        }
    } else {
        for (int i = 0; i < n_runs; i++) {
            populate_array(arr, count);
            quicksort(arr, 0, count-1);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 6) {
        printf("\nUSAGE: main_prof.c <arr_count> <val_lim> <n_runs> <seed> <run_asm: bool>\n");
        return 1;
    }

    const size_t count = atoi(argv[1]);
    const int val_lim = atoi(argv[2]);
    const int n_runs = atoi(argv[3]);
    const int seed = atoi(argv[4]);
    const int run_asm = atoi(argv[5]);

    srand(seed);  // seed for equal terms between implementations

    int *arr = calloc(count, sizeof(int));
    /* assume ok. if calloc failed we crash and burn soon enough anyways */

    /* branch: asm or c */
    if (run_asm) {
        asm_run_sort(arr, count, val_lim, n_runs);
    } else {
        run_sort(arr, count, val_lim, n_runs);
    }

    return 0;
}
