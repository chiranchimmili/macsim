#!/usr/bin/env python3

from tqdm import tqdm

MEMORY_START = 0x0
MEMORY_SIZE = 32 ## size in GB
MEMORY_END = 32*(2**30)

print("Memory Start: ", hex(MEMORY_START))
print("Memory Size: ", MEMORY_SIZE)
print("Memory End :", hex(MEMORY_END))

MATRIX_A_M = 10
MATRIX_A_N = 10  ### This and the next need to be the same
MATRIX_B_M = 10  ### This and the above need to be the same
MATRIX_B_N = 1
RESULT_M = MATRIX_A_M
RESULT_N = MATRIX_B_N

MATRIX_A_START = MEMORY_START
MATRIX_A_END = MATRIX_A_START + MATRIX_A_M*MATRIX_A_N*4 - 4
MATRIX_B_START = MATRIX_A_END + 1
MATRIX_B_END = MATRIX_B_START + MATRIX_B_M*MATRIX_B_N*4 - 1
RESULT_START = MATRIX_B_END + 1 
RESULT_END = RESULT_START + RESULT_M*RESULT_N*4 - 1

outputFile = open('output.txt','w')

for i in tqdm(range(MATRIX_A_M)):
    for j in range(MATRIX_B_N):
        print("####################################",file=outputFile)
        result_address = RESULT_START + i*RESULT_N*4 + j*4
        #print("LOAD    rcx, ",result_address,file=outputFile )
        print("MOV rcx , 0",file=outputFile )
        for k in range(MATRIX_A_N):
            matrix_a_address = MATRIX_A_START + i*MATRIX_A_N*4 + k*4
            matrix_b_address = MATRIX_B_START + k*MATRIX_B_N*4 + j*4           
            print("LOAD     rax , ",hex(matrix_a_address),file=outputFile)
            print("LOAD     rbx , ",hex(matrix_b_address),file=outputFile)
            print("MUL      rdx , rax, rbx",file=outputFile)
            print("ADD      rcx , rcx, rdx",file=outputFile)
        print("STORE    rcx , ",hex(result_address) ,file=outputFile)
        print("####################################",file=outputFile)

outputFile.close()