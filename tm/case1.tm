#Q = {INITIAL_STATE, PROCESS_A, PROCESS_B, RESET_HEADS_FOR_COPY, COPY_B_TO_TAPE0, MOVE_HEAD2_BACK, ACCEPT_STATE, REJECT_ILLEGAL_INPUT, REJECT_EMPTY_INPUT, INVALID_INPUT_HANDLER, WRITE_I, WRITE_L1, WRITE_L2, WRITE_E, WRITE_G, WRITE_A, WRITE_L3, WRITE_UNDERSCORE, WRITE_I2, WRITE_N, WRITE_P, WRITE_U, WRITE_T, RESET_INVALID_HANDLER}

#S = {a, b}

#G = {a, b, c, _, X, i, l, e, g, n, p, u, t}

#q0 = INITIAL_STATE

#B = _

#F = {ACCEPT_STATE}

#N = 3

;0: Start move a to Tape 1 and move b to Tape 2
INITIAL_STATE   a__ Xa_ rr* PROCESS_A   ; move a to tape 1
PROCESS_A       a__ _a_ rr* PROCESS_A
PROCESS_A       b__ __b r*r PROCESS_B   ; move b to tape 2
PROCESS_B       b__ __b r*r PROCESS_B
PROCESS_B       ___ ___ lll RESET_HEADS_FOR_COPY ; clear tape 0
RESET_HEADS_FOR_COPY _** _** l** RESET_HEADS_FOR_COPY
RESET_HEADS_FOR_COPY Xa* Xa* *l* RESET_HEADS_FOR_COPY
RESET_HEADS_FOR_COPY X_b X_b **l RESET_HEADS_FOR_COPY
RESET_HEADS_FOR_COPY X__ ___ *rr COPY_B_TO_TAPE0 ; cpy: start to cpy b for a times

;illegal input
INITIAL_STATE   b__ X__ *** INVALID_INPUT_HANDLER ; no a
INITIAL_STATE   ___ ___ *** REJECT_EMPTY_INPUT ; empty input
PROCESS_A       ___ ___ *** INVALID_INPUT_HANDLER ; no b
PROCESS_B       a__ a__ *** INVALID_INPUT_HANDLER ; a between b


;1: Start copy b for a times to tape 0
COPY_B_TO_TAPE0 _ab cab r*r COPY_B_TO_TAPE0
COPY_B_TO_TAPE0 _a_ _a_ **l MOVE_HEAD2_BACK
MOVE_HEAD2_BACK _ab _ab **l MOVE_HEAD2_BACK
MOVE_HEAD2_BACK _a_ _a_ *rr COPY_B_TO_TAPE0
COPY_B_TO_TAPE0 __b __b *** ACCEPT_STATE

;2: write Illegal_Input
REJECT_EMPTY_INPUT      ___ i__ r** WRITE_I
WRITE_I             ___ l__ r** WRITE_L1
WRITE_L1            ___ l__ r** WRITE_L2
WRITE_L2            ___ e__ r** WRITE_E
WRITE_E             ___ g__ r** WRITE_G
WRITE_G             ___ a__ r** WRITE_A
WRITE_A             ___ l__ r** WRITE_L3
WRITE_L3            ___ ___ r** WRITE_UNDERSCORE
WRITE_UNDERSCORE      ___ i__ r** WRITE_I2
WRITE_I2            ___ n__ r** WRITE_N
WRITE_N             ___ p__ r** WRITE_P
WRITE_P             ___ u__ r** WRITE_U
WRITE_U             ___ t__ r** WRITE_T
WRITE_T             ___ ___ *** REJECT_ILLEGAL_INPUT

INVALID_INPUT_HANDLER *__ *__ r** INVALID_INPUT_HANDLER
INVALID_INPUT_HANDLER ___ ___ l** RESET_INVALID_HANDLER
RESET_INVALID_HANDLER   ___ ___ l** RESET_INVALID_HANDLER
RESET_INVALID_HANDLER   a__ ___ l** RESET_INVALID_HANDLER
RESET_INVALID_HANDLER   b__ ___ l** RESET_INVALID_HANDLER
RESET_INVALID_HANDLER   X__ ___ *** REJECT_EMPTY_INPUT  ;