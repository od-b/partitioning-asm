.text                               # Code segment
.global asm_partition               # Make this label available in the global symbol table
.type asm_partition, @function      # Declare this label type: function
## FUNCTION
##         asm_partition
## SYNOPSIS
##         extern int asm_partition(int *arr, int low, int high);
## DESCRIPTION
##         partitions array | slice around value of arr[high]. 
## MODIFIES
##         * di. si and dx are NOT modified :^)
##         * r8-r11
##         * di, cx
## RETURNS
##         the new index of value that was previously at arr[high]
##
asm_partition:                      # (&arr: rdi, low: esi, high: edx)
    leaq    (%rdi, %rdx, 4), %r11   # r11  :: &arr[high] (&pivot)
    leaq    (%rdi, %rsi, 4), %r10   # &arr += (low * sizeof(int)). increment &arr by $low
    leaq    -4(%r10), %r9           # r9   :: &arr_j-4
    movl    (%r11), %edi            # edi  :: arr[high] ($pivot)
    movl    %esi, %eax              # eax  :: j (index)
    cmpq    %r10, %r11              # cmp(&pivot - &arr_i)
    jz      .loop_exit              # &arr_i == &pivot ? end
    cmpl    (%r10), %edi            # cmp($pivot - $arr_i)
    jg      .swap                   # jump to swap if $arr_i > $pivot
.while_cond:
    addq    $4, %r10                # &arr_i++
    cmpq    %r10, %r11              # cmp(&pivot - &arr_i)
    // neck 1
    jz      .loop_exit              # &arr_i == &pivot ? end
.if_cond:
    cmpl    (%r10), %edi            # cmp($pivot - $arr_i)
    // neck 2
    jng     .while_cond
.swap:
    movl    (%r10), %ecx            # deref $arr_i to ecx
    addq    $4, %r10                # &arr_i++
    addq    $4, %r9                 # &arr_j++
    addl    $1, %eax                # j++, to return new pivot index
    cmpq    %r10, %r11              # cmp(&pivot - &arr_i) (while-condition)
    # --- swap $arr_i, $arr_j ---
    movl    (%r9), %r8d             # r8d = *arr_j
    movl    %ecx, (%r9)             # *arr_j = *arr_i
    movl    %r8d, -4(%r10)          # offset -4 as r10 was incremented early
    jnz     .if_cond                # !(&arr_i == &pivot) ? goto if_cond : end
.loop_exit:
    # --- swap $arr_j, $pivot ---
    movl    4(%r9), %r8d            # r9 is at rightmost lower [than pivot] element, so move to its right
    movl    %edi, 4(%r9)
    movl    %r8d, (%r11)
    ret
