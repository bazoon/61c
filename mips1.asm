.data
A: .word 2,4,3,61,1,99
B: .word 1,2,3,4,5,6,7,8,9,10,11,12

.text 


addi $a0, $a0, 15
jal fib
j exit
fib:
addi $sp, $sp, -12
sw $ra, 8($sp)
sw $a0, 4($sp)
sw $s1, 0($sp)

bne $a0, $zero, l1
add $v0, $zero, $zero
j done
l1:
addi $s1, $1, 1
bne $a0, $s1, l2
addi $v0, $zero, 1
j done
l2:
addi $a0, $a0, -1
jal fib
add $s1, $v0, $zero
addi $a0, $a0, -1
jal fib
add $v0, $v0, $s1

done:

lw $s1, 0($sp)
lw $a0, 4($sp)
lw $ra, 8($sp)
addi $sp, $sp, 12
jr $ra

exit: