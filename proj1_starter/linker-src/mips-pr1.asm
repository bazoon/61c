.include "parsetools.s"
.include "file_utils.s"

.data

file1: 	.asciiz "/Users/vn/projects/61c/proj1_starter/link-in/linker1.out" 
outfile1: .asciiz "/Users/vn/projects/61c/proj1_starter/lout" 
fnames: .word file1 


.text
main:
	la $a0, fnames
	li $a1, 1
	jal open_files
	lw $a0, 0($v0)
	jal readline


	li $v0, 10
	syscall


	
write_machine_code:
	# You may need to save additional items onto the stack. Feel free to
	# change this part.
	addiu $sp, $sp, -24
	sw $s0, 20($sp)
	sw $s1, 16($sp)
	sw $s2, 12($sp)
	sw $s3, 8($sp)
	sw $s4, 4($sp)
	sw $ra, 0($sp)
	# We'll save the arguments since we are making function calls.
	move $s0, $a0			# $s0 = output file ptr
	move $s1, $a1			# $s1 = input file ptr
	move $s2, $a2			# $s2 = symbol table
	move $s3, $a3			# $s3 = relocation table
	# We find the start of the .text section by reading each line and 
	# checking to see if we find ".text".
write_machine_code_find_text:	
	move $a0, $s1
	jal readline
	blt $v0, $0, write_machine_code_error	# Invalid line reached
	move $a0, $v1
	la $a1, textLabel
	jal streq
	bne $v0, $0, write_machine_code_find_text
	# Now, we are at the start of the text section (ie. next time we call
	# readline(), we will read in the first instruction). Fill in the
	# blanks below with the correct instruction(s).
	
	# 1. Initialize the byte offset to zero. We will need this for any instructions
	# that require relocation:
	# YOUR_INSTRUCTIONS_HERE
	add $s4, $zero, 0
write_machine_code_next_inst:
	# 2. Call readline() while passing in the correct arguments:
	# YOUR_INSTRUCTIONS_HERE
	jal readline
	# Check whether readline() returned an error.
	blt $v0, $0, write_machine_code_error
	# Store whether end-of-file was reached into $s4. We will check this later.
	move $s4, $v0			
	# We check whether there are any characters besides the NUL terminator below.
	lbu $t0, 0($v1)
	beq $t0, $0, write_machine_code_done
	
	# 3. Looks like there is another instruction. Call parse_int() with base=16
	# to convert the instruction into a number, and store it into a register:
	# YOUR_INSTRUCTIONS_HERE
	move $a0, $v1
	li $a1, 16
	jal parse_int # v0 = instruction
	move $a0, $v0
	jal inst_needs_relocation
	beq $v0, $zero, write_machine_code_to_file
	# 4. Check if the instruction needs relocation. If it does not, branch to
	# the label write_machine_code_to_file:
	# YOUR_INSTRUCTIONS_HERE
	
	# 5. Here we handle relocation. Call relocate_inst() with the appropriate
	# arguments, and store the relocated instruction in the appropriate register:
	# YOUR_INSTRUCTIONS_HERE
	move $a1, $s4
	move $a2, $s2
	move $a3, $s3
	jal relocate_inst

write_machine_code_to_file:
	# 6. Write the instruction into a string buffer via hex_to_str():
	# YOUR_INSTRUCTIONS_HERE
	move $a0, $v0
	la $a1, hex_buffer
	jal hex_to_str
	# 7. Increment the byte offset by the appropriate amount:
	# YOUR_INSTRUCTIONS_HERE
	addiu $t4, $t4, 4

	# Here, we use the write to file syscall. WE specify the output file as $a0.
	move $a0, $s0
	# Set $a1 = the buffer that we will write.
	la $a1, hex_buffer
	# Set $a2 = number of bytes to write. 8 digits + newline = 9 bytes
	li $a2, 9			
	li $v0, 15
	syscall
	# Finally, we check whether end-of-file was reached. If not, keep going.
	bne $s4, $0, write_machine_code_next_inst
write_machine_code_done:
	li $v0, 0
	j write_machine_code_end
write_machine_code_error:
	li $v0, -1
write_machine_code_end:
	# Don't forget to change this part if you saved more items onto the stack!
	lw $s0, 20($sp)
	lw $s1, 16($sp)
	lw $s2, 12($sp)
	lw $s3, 8($sp)
	lw $s4, 4($sp)
	lw $ra, 0($sp)
	addiu $sp, $sp, 24
	jr $ra	
			
				


	
	
	
	
	
	
	
	
	
	
	
	
	
