  # labwork.S
  # Just for the enable_interrupts.
  # Consider moving this to vectors.S?
  #

.global enable_interrupt
	.data
	.align 2
	.text

enable_interrupt:
	ei
	jr $ra
	nop
