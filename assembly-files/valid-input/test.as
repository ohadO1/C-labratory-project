.entry HELLO
.extern TEST

HELLO: clr r2

A: .data 2, 3, 4, 5,  5

WORD: .string "helloWorld"

macr test_macr
add A, r0
sub #3, r1
cmp A, WORD
endmacr
cmp #25, r4
inc r6
red r5
lea TEST, r3
lea  WORD, r7
test_macr
stop
