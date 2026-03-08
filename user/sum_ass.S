.data
    fmt_sum: .asciz "%d\n"
    msg_overflow: .asciz "Invalid input: buffer overflow\n"
    msg_invalid: .asciz "Invalid input: Invalid symbols\n"
    msg_no_first: .asciz "Invalid input: No first number\n"
    msg_no_space: .asciz "Invalid input: No space\n"
    msg_no_second: .asciz "Invalid input: No second number\n"
    msg_gets_err: .asciz "read error\n"

.bss
    buf: .space 128

.text
.globl main

main:
    addi sp, sp, -64
    sd ra, 56(sp)
    sd s0, 48(sp)
    sd s1, 40(sp)
    sd s2, 32(sp)
    sd s3, 24(sp)
    sd s4, 16(sp)
    sd s5, 8(sp)

    # s0 = base pointer to buf
    la s0, buf

    # gets(buf)
    mv a0, s0
    li a1, 128
    call gets
    beqz a0, .Lgets_error   # gets returned NULL

    # first_pos = -1, second_pos = -1, was_space = 0, pos = 0
    li s1, -1 # first_pos
    li s2, -1 # second_pos
    li s3, 0  # was_space
    li s4, 0  # pos

.Lloop:
    # _if (pos >= 127) overflow
    li t0, 127
    bge s4, t0, .Loverflow

    # ch = buf[pos]
    add t1, s0, s4
    lbu t2, 0(t1)

    # _if ch == '\0' -> end scan
    beqz t2, .Lend_scan

    # _if ch == '\n' -> end scan
    li t3, '\n'
    beq t2, t3, .Lend_scan

    # _if (ch == ' ' && first_pos == -1) { pos++; continue; }
    li t3, ' '
    bne t2, t3, .Lnot_leading_space
    li t4, -1
    bne s1, t4, .Lspace_after_or_between
    addi s4, s4, 1
    j .Lloop

.Lnot_leading_space:
    # _if ('0' <= ch && ch <= '9') ...
    li t3, '0'
    blt t2, t3, .Lnot_digit
    li t4, '9'
    bgt t2, t4, .Lnot_digit

    # digit:
    # _if (first_pos == -1) first_pos = pos;
    li t5, -1
    bne s1, t5, .Lmaybe_second
    mv s1, s4
    addi s4, s4, 1
    j .Lloop

.Lmaybe_second:
    # _else if (was_space == 1 && second_pos == -1) second_pos = pos;
    li t0, 1
    bne s3, t0, .Ldigit_advance
    li t1, -1
    bne s2, t1, .Ldigit_advance
    mv s2, s4

.Ldigit_advance:
    addi s4, s4, 1
    j .Lloop

.Lnot_digit:
    # _if ch == ' ':
    li t3, ' '
    bne t2, t3, .Linvalid

.Lspace_after_or_between:
    # was_space = 1; pos++; continue;
    li s3, 1
    addi s4, s4, 1
    j .Lloop

.Linvalid:
    la a1, msg_invalid
    j .Lprint_ret

.Loverflow:
    la a1, msg_overflow
    j .Lprint_ret

.Lgets_error:
    la a1, msg_gets_err
    j .Lprint_ret

.Lend_scan:
    # _if (first_pos == -1) error
    li  t0, -1
    bne s1, t0, .Lcheck_space
    la a1, msg_no_first
    j .Lprint_ret

.Lcheck_space:
    # _if (was_space == 0) error
    bnez s3, .Lcheck_second
    la a1, msg_no_space
    j .Lprint_ret

.Lcheck_second:
    # _if (second_pos == -1) error
    li t0, -1
    bne s2, t0, .Lcompute
    la a1, msg_no_second
    j .Lprint_ret

.Lprint_ret:
    li a1, 2
    call fprintf
    li a0, 1
    j .Lret

.Lcompute:
    # a = atoi(buf + first_pos)
    add a0, s0, s1
    call atoi
    mv s5, a0

    # b = atoi(buf + second_pos)
    add a0, s0, s2
    call atoi

    # c = a + b
    add a1, s5, a0

    # printf("%d\n", c)
    la a0, fmt_sum
    call printf

    li a0, 0

.Lret:
    ld ra, 56(sp)
    ld s0, 48(sp)
    ld s1, 40(sp)
    ld s2, 32(sp)
    ld s3, 24(sp)
    ld s4, 16(sp)
    ld s5, 8(sp)
    addi sp, sp, 64
    ret
