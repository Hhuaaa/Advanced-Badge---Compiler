.data
.text
.globl main

main:
li $t0, 3
li $t2, 4
li $t1, 0
LOOP_START:
	bgt $t1, 3, LOOP_END
	beq $t0, $t2, IF_TRUE0
	j IF_FALSE0
IF_TRUE0:
	add $t0, $t0, $t1
	addi $t0, $t0, 2
	j END_IF0
IF_FALSE0:
END_IF0:
	bne $t0, $t2, IF_TRUE1
	j IF_FALSE1
IF_TRUE1:
	add $t0, $t0, 1
	j END_IF1
IF_FALSE1:
END_IF1:
	addi $t1, $t1, 1
	j LOOP_START
LOOP_END:
    li $v0, 1
    move $a0, $t0
    syscall
    
    li $v0, 10
    syscall
