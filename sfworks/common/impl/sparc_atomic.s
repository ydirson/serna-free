/*
 * This code was compiled from some examples derived from sparc forum, and
 * verified with atomic operations derived from STL port.
 * It is places here as a back-up; current version use InterlockedIncrement() / InterlockedDecrement
 * from RW library which is integrated part of FORTE software.
 * All this solutions need -xtarget=ultra option (and higher).
 */

.section        ".text",#alloc,#execinstr
	.align 8
	.skip 16

/*****************************************************************************/
	.type   _sparc_atomic_increment,#function
	.global _sparc_atomic_increment
	.align  8


_sparc_atomic_increment:
1:
	        ld      [%o0], %o2                     ! set the current
	        add             %o2, 0x1, %o3          ! Increment and store current
!	        swap    [%o0], %o3                     ! Do the compare and swap
	        cas     [%o0], %o2, %o3
	        cmp     %o3, %o2                       ! Check whether successful
	        bne     1b                             ! Retry if we failed.
	        membar  #LoadLoad | #LoadStore         ! Ensure the cas finishes before
	        add             %o2, 0x1, %o0          ! Calculate new value once more
	        nop
		retl				       ! Return value in %o0
		nop

	        .size   _sparc_atomic_increment,(.-_sparc_atomic_increment)
/*****************************************************************************/
	.type   _sparc_atomic_decrement,#function
	.global _sparc_atomic_decrement
	.align  8
_sparc_atomic_decrement:
		nop
2:
	        ld    [%o0], %o2                        ! set the current
	        sub     %o2, 0x1, %o3                   ! decrement and store current
!	        swap    [%o0], %o3                      ! Do the compare and swap
	        cas     [%o0], %o2, %o3
	        cmp     %o3, %o2                        ! Check whether successful
	        bne     2b                                         ! Retry if we failed.
	        membar  #LoadLoad | #LoadStore          ! Ensure the cas finishes before
		sub     %o2, 0x1, %o0			! Calculate new value once more
	        nop
		retl					!Return value in %o0
		nop
	.size   _sparc_atomic_decrement,(.-_sparc_atomic_decrement)
		
