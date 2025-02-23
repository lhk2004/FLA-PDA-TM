#Q = {START_INIT, CHECK_SQUARE_CONDITION, INCREMENT_COUNTER_A, INCREMENT_COUNTER_B, CLEAN_TAPE_FOR_TRUE, CLEAN_TAPE_FOR_FALSE, HALT_ACCEPT, WRITE_TRUE_1, WRITE_TRUE_2, WRITE_TRUE_3, WRITE_TRUE_4, WRITE_FALSE_1, WRITE_FALSE_2, WRITE_FALSE_3, WRITE_FALSE_4}
; State names:
; START_INIT:           Initial state, starts the process
; CHECK_SQUARE_CONDITION: Core logic to check if input count is a square
; INCREMENT_COUNTER_A:  Helper state for incrementing counter on tape1 (part A)
; INCREMENT_COUNTER_B:  Helper state for incrementing counter on tape1 (part B and head reset)
; CLEAN_TAPE_FOR_TRUE:  Cleans tape0 before writing "true"
; CLEAN_TAPE_FOR_FALSE: Cleans tape0 before writing "false"
; HALT_ACCEPT:          Accept state, program halts and accepts
; WRITE_TRUE_*:         States to write "true" onto tape0
; WRITE_FALSE_*:        States to write "false" onto tape0

#S = {1}
#G = {0,1,_,t,r,u,e,f,a,l,s}
#q0 = START_INIT
#B = _
#F = {HALT_ACCEPT}
#N = 2

; Transition functions:

; For input '1', initialize tape1 with '1' and tape0 with '0', then go to CHECK_SQUARE_CONDITION
START_INIT 1_ 01 rr CHECK_SQUARE_CONDITION

; Check if both tapes are blank, if so, it's a square (for input "1"), go to CLEAN_TAPE_FOR_TRUE
CHECK_SQUARE_CONDITION __ __ ll CLEAN_TAPE_FOR_TRUE

; If tape0 has '1' and tape1 is blank, increment tape1 and move tape1 head right, go to INCREMENT_COUNTER_A
CHECK_SQUARE_CONDITION 1_ 11 *r INCREMENT_COUNTER_A

; Helper state A for incrementing: If tape0 has '1' and tape1 is blank, increment tape1, no head move, go to INCREMENT_COUNTER_B
INCREMENT_COUNTER_A 1_ 11 ** INCREMENT_COUNTER_B

; Helper state B for incrementing:
; If tape0 has '1' and tape1 has '1', keep both as '1', move tape0 head left, loop in INCREMENT_COUNTER_B to move left
INCREMENT_COUNTER_B 11 11 *l INCREMENT_COUNTER_B
; If tape0 has '1' and tape1 is blank, keep tape0 as '1', keep tape1 as blank, move tape0 head right, go back to CHECK_SQUARE_CONDITION
INCREMENT_COUNTER_B 1_ 1_ *r CHECK_SQUARE_CONDITION

; If tape0 has '1' and tape1 has '1', decrement tape0 to '0', increment tape1, move both heads right, loop in CHECK_SQUARE_CONDITION
CHECK_SQUARE_CONDITION 11 01 rr CHECK_SQUARE_CONDITION
; If tape0 is blank and tape1 has '1', input is not a square, go to CLEAN_TAPE_FOR_FALSE
CHECK_SQUARE_CONDITION _1 _1 ll CLEAN_TAPE_FOR_FALSE


; If tape0 has '0' and tape1 has '1', clean tape0 by writing '_', keep tape1 as '1', move tape0 head left, loop in CLEAN_TAPE_FOR_TRUE
CLEAN_TAPE_FOR_TRUE 01 _1 l* CLEAN_TAPE_FOR_TRUE
; If tape0 is blank and tape1 has '1', move tape0 head right and go to WRITE_TRUE_1 to start writing "true"
CLEAN_TAPE_FOR_TRUE _1 _1 r* WRITE_TRUE_1

; Write 't' of "true"
WRITE_TRUE_1 _1 t1 r* WRITE_TRUE_2
; Write 'r' of "true"
WRITE_TRUE_2 _1 r1 r* WRITE_TRUE_3
; Write 'u' of "true"
WRITE_TRUE_3 _1 u1 r* WRITE_TRUE_4
; Write 'e' of "true" and HALT_ACCEPT
WRITE_TRUE_4 _1 e1 ** HALT_ACCEPT


; If tape0 has '0' and tape1 has '1', clean tape0 by writing '_', keep tape1 as '1', move tape0 head left, loop in CLEAN_TAPE_FOR_FALSE
CLEAN_TAPE_FOR_FALSE 01 _1 l* CLEAN_TAPE_FOR_FALSE
; If tape0 is blank and tape1 has '1', move tape0 head right and go to WRITE_FALSE_1 to start writing "false"
CLEAN_TAPE_FOR_FALSE _1 _1 r* WRITE_FALSE_1

; Write 'f' of "false"
WRITE_FALSE_1 _1 f1 r* WRITE_FALSE_2
; Write 'a' of "false"
WRITE_FALSE_2 _1 a1 r* WRITE_FALSE_3
; Write 'l' of "false"
WRITE_FALSE_3 _1 l1 r* WRITE_FALSE_4
; Write 's' of "false"
WRITE_FALSE_4 _1 s1 r* WRITE_TRUE_4 ; Note: Originally pointed to 'we', corrected to WRITE_TRUE_4 to reach HALT_ACCEPT after writing "false", and to be consistent with original program's behavior to end in state 'e'

; HALT_ACCEPT state is defined in #F