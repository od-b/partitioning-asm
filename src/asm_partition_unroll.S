.text                               # Code segment
.global asm_partition               # Make this label available in the global symbol table
.type asm_partition, @function      # Declare this label type: function
asm_partition:                      # (&arr: rdi, low: esi, high: edx)
    leaq    (%rdi, %rdx, 4), %r11   # r11  :: &arr[high] (&pivot)
    movl    (%r11), %r10d           # r10d :: arr[high] ($pivot)
    leaq    (%rdi, %rsi, 4), %rdi   # &arr += (low * sizeof(int)), move &arr to arr[low]
    leaq    -4(%rdi), %r9               # r9   :: -4&arr_j
    movl    %esi, %eax              # eax  :: j (index)
# .loop_entry_0:
    cmpq    %rdi, %r11              # cmp(&pivot - &arr_i)
    jz      .loop_exit               # if pivot is less than i, end
    cmpl    (%rdi), %r10d           # cmp($pivot - $arr_i)
    jng     .loop_entry_1             # if $pivot is less than $arr_i: (jna faster if unsigned)
.swap:
# Perform the next loop_entry test simultaneously as the swap to better fill pipeline and reduce jumps
    movl    (%rdi), %ecx            # for comparison in L3, and potential swap deref
    addq    $4, %rdi
    addq    $4, %r9
    addl    $1, %eax                # j++, to return new pivot index
    cmpq    %rdi, %r11              # cmp(&pivot - &arr_i) (while-condition)
    # perform swap of $arr[i], $arr[j]
    movl    (%r9), %r8d             # r8d = *arr_j
    movl    %ecx, (%r9)             # *arr_j = *arr_i
    movl    %r8d, -4(%rdi)          # offset -4 as rdi was incremented early
    # perform comparison as in .loop_entry, except with direct exit if comparison fails
    jz      .loop_exit              # if &pivot is less than &arr_i, end.
.loop_re_1:
    cmpl    (%rdi), %r10d           # cmp($pivot - $arr_i)
    jg      .swap
.loop_entry_1:
    addq    $4, %rdi                # &arr_i += sizeof(int)
    cmpq    %rdi, %r11              # cmp(&pivot - &arr_i)
    jz      .loop_exit              # if pivot is less than i, end
    cmpl    (%rdi), %r10d           # cmp($pivot - $arr_i)
    jg      .swap                   # if $pivot is less than $arr_i: (jna faster if unsigned)
.loop_entry_2:
    addq    $4, %rdi
    cmpq    %rdi, %r11
    jz      .loop_exit
    cmpl    (%rdi), %r10d
    jg      .swap
.loop_entry_3:
    addq    $4, %rdi
    cmpq    %rdi, %r11
    jz      .loop_exit
    cmpl    (%rdi), %r10d
    jg      .swap
.loop_entry_4:
    addq    $4, %rdi
    cmpq    %rdi, %r11
    jz      .loop_exit
    cmpl    (%rdi), %r10d
    jg      .swap
.loop_entry_5:
    addq    $4, %rdi
    cmpq    %rdi, %r11
    jnz     .loop_re_1
.loop_exit:
    # --- swap $arr_j, $pivot ---
    movl    4(%r9), %r8d            # r9 is at rightmost lower [than pivot] element, so move to its right
    movl    %r10d, 4(%r9)
    movl    %r8d, (%r11)
    ret
