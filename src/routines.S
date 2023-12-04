.text
.align 4

.global quicksleep
quicksleep:
    beq     $a0,$zero,stopsleep
    addi    $a0,$a0,-1
    j       quicksleep
stopsleep:
    j       $ra