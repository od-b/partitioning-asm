#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// struct opcalls;
typedef struct opcalls {
    unsigned int mov;
    unsigned int lea;
    unsigned int add;
    unsigned int cmp;
    unsigned int jcc;
    unsigned int ret;
} opcalls_t;

/* asm_partition, fairly accurate instruction count.
 * .nonlooping (pre/post loop):
 *     lea: 3
 *     add: 2 (from leaq indirect register, line 1/2)
 *     mov: 5
 *     ret: 1
 * .while:
 *     add: 1
 *     cmp: 1
 *     jcc: 1
 * .if:
 *     cmp: 1
 *     jcc: 1
 * .swap:
 *     add: 2
 *     mov: 4
 *     jcc: 1
 */
int partition(int *arr, int low, int high, opcalls_t *ops) {
    int i = low;
    int j = low-1;

    // nonlooping
    ops->lea += 3;
    ops->add += 2;
    ops->mov += 5;
    ops->ret += 1;

    while (1) {
        // while; put break condition inside scope to count checks
        ops->add += 1;
        ops->cmp += 1;
        ops->jcc += 1;  // goto end
        if (i == high) break;

        // if:
        ops->cmp += 1;
        ops->jcc += 1;  // goto while
        if (arr[i] < arr[high]) {
            // swap:
            ops->add += 2;
            ops->mov += 4;
            ops->jcc += 1;  // goto while

            j++;
            low = arr[j];
            arr[j] = arr[i];
            arr[i] = low;
        }
        i++;
    }

    j++;
    low = arr[j];
    arr[j] = arr[high];
    arr[high] = low;

    return j;
}

void quicksort(int *arr, int low, int high, opcalls_t *ops) {
    if (low < high) {
        int pivot_i = partition(arr, low, high, ops);
        quicksort(arr, low, pivot_i-1, ops);
        quicksort(arr, pivot_i+1, high, ops);
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    opcalls_t *ops = malloc(sizeof(opcalls_t));
    ops->mov = ops->lea = ops->add = ops->cmp = ops->jcc = ops->ret = 0;

    const size_t arr_size = 100;

    int arr[arr_size];
    for (int i = 0; i < arr_size; i++) {
        arr[i] = rand();
    }

    quicksort(arr, 0, arr_size-1, ops);

    // sum of all instruction calls
    unsigned int call_total = ops->mov + ops->lea + ops->add + ops->cmp + ops->jcc + ops->ret;

    /* 
     * AMD K7, rough reciprocal throughput values, based off of agner fog data 
     * assumes `opc r/i, r/i`, direct register/immediate integer values
     * https://www.agner.org/optimize/ -- heading 5
     */
    double rt_MOV = 1.0/3.0;
    double rt_LEA = 1.0/3.0;
    double rt_ADD = 1.0/3.0;
    double rt_CMP = 1.0/3.0;
    double rt_JCC = 1.163;  // [1/3 - 2]. 2 if jump. adjusted to assume jump for 50% of jcc calls
    double rt_RET = 3.0;

    // cumulative theoretical reciprocal throughput, per instruction
    double t_MOV = ((double)ops->mov) * rt_MOV;
    double t_LEA = ((double)ops->lea) * rt_LEA;
    double t_ADD = ((double)ops->add) * rt_ADD;
    double t_CMP = ((double)ops->cmp) * rt_CMP;
    double t_JCC = ((double)ops->jcc) * rt_JCC;
    double t_RET = ((double)ops->ret) * rt_RET;

    // total cycles, calls
    double cycl_total = t_MOV + t_LEA + t_ADD + t_CMP + t_JCC + t_RET;

    // making sure stuff adds up, idk:
    double t_p = (
        (t_MOV/cycl_total) + (t_LEA/cycl_total) + (t_ADD/cycl_total)
        + (t_CMP/cycl_total) + (t_JCC/cycl_total) + (t_RET/cycl_total)
        ) * 100;

    printf("\n__OPC___|__No. Calls__|__reciprocal_tp.__|___cycles___|___cost_%%__\n");
    printf("  mov   |  %-10d |      %.3f       |  %-8.1f  |  %-4.2f\n", ops->mov, rt_MOV, t_MOV, (t_MOV/cycl_total)*100.0);
    printf("  lea   |  %-10d |      %.3f       |  %-8.1f  |  %-4.2f\n", ops->lea, rt_LEA, t_LEA, (t_LEA/cycl_total)*100.0);
    printf("  add   |  %-10d |      %.3f       |  %-8.1f  |  %-4.2f\n", ops->add, rt_ADD, t_ADD, (t_ADD/cycl_total)*100.0);
    printf("  cmp   |  %-10d |      %.3f       |  %-8.1f  |  %-4.2f\n", ops->cmp, rt_CMP, t_CMP, (t_CMP/cycl_total)*100.0);
    printf("  jcc   |  %-10d |      %.3f       |  %-8.1f  |  %-4.2f\n", ops->jcc, rt_JCC, t_JCC, (t_JCC/cycl_total)*100.0);
    printf("  ret   |  %-10d |      %.3f       |  %-8.1f  |  %-4.2f\n", ops->ret, rt_RET, t_RET, (t_RET/cycl_total)*100.0);
    printf(" \e[0;37m SUM   |  %-10d |      n/a         |  %-8.1f  |  %-4.1f\e[0m\n\n", call_total, cycl_total, t_p);

    return 0;
}
