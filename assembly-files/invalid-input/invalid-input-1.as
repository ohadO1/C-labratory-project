MAIN: add r3, ,LIST
.entry INVALIDENTRY  
.extern
EMPTYLABEL:
LOOP: prn #48
macr m_macr
 cmp r3, #-6, r5
 bne 
endmacr
 lea STR, r5, r6 ,,
 inc r6, r10
 mov *r6,K, 35
 sub r1  ,r10, mov
 m_macr
 dec K, #25
 mov 2, stop 
 jmp , abc ,,
END: stop f5
STR: .string "abc3d"
LIST: .data 6, -9 15 44
.data -100
 K: .data 31

