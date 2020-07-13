  addu		$t0, $t1, $t2		# $t0 = $t1 + $t2
second:
  beq		$t0, $t1, target	# if $t0 == $t1 then target
  addu   $a0, $a0, $zero
  addu   $a0, $a1, $a1
target:
  j second
