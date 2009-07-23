/*
 * This is INLINE version of sun_atomic_increment / sun_atomic_decrement methods
 * for SUN UltraSparc
 * Library version is in the src library (sparc_atomic.s)
 */
.inline _sparc_atomic_increment,4
1:
	        ld      [%o0], %o2              ! set the current
	        add             %o2, 0x1, %o3                   ! Increment and store current
!	        swap     [%o0], %o3         ! Do the compare and swap
	        cas     [%o0], %o2, %o3 
	        cmp     %o3, %o2                ! Check whether successful
	        bne     1b                                         ! Retry if we failed.
	        membar  #LoadLoad | #LoadStore  ! Ensure the cas finishes before
	        add             %o2, 0x1, %o0   ! Calculate new value once again
	        nop				! Return value in %0
.end

.inline _sparc_atomic_decrement,4
		nop
2:
	        lduw    [%o0], %o2              ! set the current
	        sub     %o2, 0x1, %o3                   ! decrement and store current
!	        swap    [%o0], %o3         ! Do the compare and swap
	        cas     [%o0], %o2, %o3 
	        cmp     %o3, %o2                ! Check whether successful
	        bne     2b                                         ! Retry if we failed.
	        membar  #LoadLoad | #LoadStore  ! Ensure the cas finishes before
		sub     %o2, 0x1, %o0		! Calculate new value once again
	        nop				! return value in %0
.end
