.text                                # Code segment
.global asm_partition                # Make this label available in the global symbol table
.type asm_partition, @function       # Declare this label type: function

# NV, special   :: RBP, RSP
# non-volatile  :: RBX, RDI, RSI, R12, R13, R14, R15
# args          :: RDI, RSI, RDX, RCX, R8, R9
# volatile      :: RAX, RCX, RDX, R8, R9, R10, R11
asm_partition:                          # (*arr: rdi, low: esi, high: edx)
    # --- init new stack frame ---
    pushq   %rbp
    movq    %rsp, %rbp
    # --- set up registers ---
    movl    (%rdi, %rdx, 4), %r10d      # r10  :: pivot -- this is fine
    movq    %rdi, %r8                   # r8   :: *arr_i
    xorq    %rax, %rax                  # clear ax
    movl    %esi, %eax                  # copy low
    imul    $4, %eax                    # low *= 4     -- TODO: shift instead
    addq    %rax, %r8
    subq    $4, %r8                     # to start looping at *arr-4
    movq    %r8, %r9                    # r9   :: *arr_j
    movl    %esi, %eax                  # eax  :: j
    decl    %esi                        # to start i and j counters at low-1
    # --- loop body ---
.L2:
    incl    %esi                        # i++
    addq    $4, %r8                     # *arr_i++
    cmpl    %esi, %edx                  # cmp(high - i)
    jl     .end                         # if high is not greater than i, end
    cmpl    (%r8), %r10d                # cmp(pivot - $arr_i)
    jng     .L2                         # if pivot is greater than arr[i], reloop
    # ... else, swap arr_i, arr_j ($r8, $r9)
    incl    %eax    # j++
    addq    $4, %r9
    movl    (%r8), %ecx
    movl    (%r9), %r11d
    movl    %ecx, (%r9)
    movl    %r11d, (%r8)
    jmp     .L2
.end:
    # --- swap $arr_j, $arr[high] ---
    addq    $4, %r9                     # *arr_j++
    subq    $4, %r8
    movl    (%r8), %ecx
    movl    (%r9), %r11d
    movl    %ecx, (%r9)
    movl    %r11d, (%r8)
    popq    %rbp
    ret
