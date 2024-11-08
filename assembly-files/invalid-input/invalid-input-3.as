macr hello_world
   lea STR, r6
 inc r6
 mov *r6, L3
 sub r1, r4
 cmp r3, #-6
 bne END
 add r7, *r6
 clr K
 sub L3, L3
 endmacr 5

hello_world
