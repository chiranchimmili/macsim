#include <stdio.h>
#include <stdint.h>

#include "simple_mm_converter.h"

#define MEMORY_START 0x0
#define MEMORY_SIZE (32LL * (1LL << 30)) // size in GB, the LL denotes long long
#define MEMORY_END (32LL * (1LL << 30))

#define MATRIX_A_M 8
#define MATRIX_A_N 8
#define MATRIX_B_M 8
#define MATRIX_B_N 1

#define RESULT_M MATRIX_A_M
#define RESULT_N MATRIX_B_N

#define MATRIX_A_START MEMORY_START
#define MATRIX_A_END (MATRIX_A_START + MATRIX_A_M * MATRIX_A_N * 4 - 4)
#define MATRIX_B_START (MATRIX_A_END + 1)
#define MATRIX_B_END (MATRIX_B_START + MATRIX_B_M * MATRIX_B_N * 4 - 4)
#define RESULT_START (MATRIX_B_END + 1)
#define RESULT_END (RESULT_START + RESULT_M * RESULT_N * 4 - 4)

void print_trace_info(TraceInfo* t_info, FILE* file) {
    fprintf(file, "*** begin of the data structure ***\n");
    fprintf(file, "t_info->uop_opcode_num %d\n", t_info->uop_opcode_num);
    fprintf(file, "t_info->uop_opcode %s\n", t_info->uop_opcode);
    fprintf(file, "t_info->num_read_regs: %d\n", t_info->num_read_regs);
    fprintf(file, "t_info->num_dest_regs: %d\n", t_info->num_dest_regs);

    if (t_info->num_read_regs >= 1) {
        fprintf(file, "t_info->src0: %d : %s\n", t_info->src0, regs[t_info->src0]);
        if (t_info->num_read_regs > 1) {
            fprintf(file, "t_info->src1: %d : %s\n", t_info->src1, regs[t_info->src1]);
        }
    }
    if (t_info->num_dest_regs >= 1) {
        fprintf(file, "t_info->dst0: %d : %s\n", t_info->dst0, regs[t_info->dst0]);
        if (t_info->num_dest_regs > 1) {
            fprintf(file, "t_info->dst1: %d : %s\n", t_info->dst1, regs[t_info->dst1]);
        }
    }
    fprintf(file, "t_info->cf_type: %s\n", t_info->cf_type);
    fprintf(file, "t_info->has_immediate: %d\n", t_info->has_immediate);
    fprintf(file, "t_info->r_dir:%d\n", t_info->r_dir);
    fprintf(file, "t_info->has_st: %d\n", t_info->has_st);
    fprintf(file, "t_info->num_ld: %d\n", t_info->num_ld);
    fprintf(file, "t_info->mem_read_size: %llu\n", t_info->mem_read_size);
    fprintf(file, "t_info->mem_write_size: %llu\n", t_info->mem_write_size);
    fprintf(file, "t_info->is_fp: %d\n", t_info->is_fp);
    fprintf(file, "t_info->ld_vaddr1: %llx\n", t_info->ld_vaddr1);
    fprintf(file, "t_info->ld_vaddr2: %llx\n", t_info->ld_vaddr2);
    fprintf(file, "t_info->st_vaddr: %llx\n", t_info->st_vaddr);
    fprintf(file, "t_info->instruction_addr: %llx\n", t_info->instruction_addr);
    fprintf(file, "t_info->branch_target: %llx\n", t_info->branch_target);
    fprintf(file, "t_info->actually_taken: %d\n", t_info->actually_taken);
    fprintf(file, "t_info->write_flg: %d\n", t_info->write_flg);
    fprintf(file, "*** end of the data structure ***\n\n");
}

int main() {
    FILE *outputFile = fopen("output.txt", "w");

    printf("Memory Start: %llx\n", (unsigned long long)MEMORY_START);
    printf("Memory Size: %lld\n", MEMORY_SIZE);
    printf("Memory End: %llx\n", (unsigned long long)MEMORY_END);

    TraceInfo t_info;
    t_info.cf_type = "NOT_CF";
    t_info.has_immediate = 0;
    t_info.r_dir = 0;
    t_info.has_st = 0;
    t_info.num_ld = 0;
    t_info.mem_read_size = 8;
    t_info.mem_write_size = 0;
    t_info.is_fp = 0;
    t_info.ld_vaddr1 = 0;
    t_info.ld_vaddr2 = 0;
    t_info.st_vaddr = 0;
    t_info.instruction_addr = 0; // FIX
    t_info.branch_target = 0;
    t_info.actually_taken = 0;
    t_info.write_flg = 0;

    for (int i = 0; i < MATRIX_A_M; i++) {
        for (int j = 0; j < MATRIX_B_N; j++) {
            uint64_t result_address = RESULT_START + i * RESULT_N * 4 + j * 4;

            // MOV Operation
            t_info.uop_opcode_num = 30; 
            t_info.uop_opcode = "DATAXFER";
            t_info.num_read_regs = 0;
            t_info.num_dest_regs = 1;
            t_info.dst0 = 9;
            t_info.has_immediate = 1;
            t_info.mem_read_size = 0;
            t_info.instruction_addr = 0; // FIX
            print_trace_info(&t_info, outputFile);

            for (int k = 0; k < MATRIX_A_N; k++) {
                uint64_t matrix_a_address = MATRIX_A_START + i * MATRIX_A_N * 4 + k * 4;
                uint64_t matrix_b_address = MATRIX_B_START + k * MATRIX_B_N * 4 + j * 4;
                
                // LOAD operation for matrix_a
                t_info.uop_opcode_num = 30;
                t_info.uop_opcode = "DATAXFER";
                t_info.num_read_regs = 0;
                t_info.num_dest_regs = 1;
                t_info.dst0 = 10;
                t_info.mem_read_size = 8;
                t_info.has_immediate = 0;
                t_info.num_ld = 1;
                t_info.ld_vaddr1 = matrix_a_address;
                t_info.mem_write_size = 0;
                t_info.instruction_addr = 0; // FIX

                print_trace_info(&t_info, outputFile);

                // LOAD operation for matrix_b
                t_info.dst0 = 7;
                t_info.ld_vaddr1 = matrix_b_address;
                t_info.instruction_addr = 0; // FIX
                print_trace_info(&t_info, outputFile);

                // MUL operation
                t_info.uop_opcode_num = 95;
                t_info.uop_opcode = "TR_MUL";
                t_info.num_read_regs = 2;
                t_info.num_dest_regs = 2;
                t_info.src0 = 7;
                t_info.src1 = 10;
                t_info.dst0 = 8;
                t_info.dst1 = 25;
                t_info.num_ld = 0;
                t_info.ld_vaddr1 = 0;
                t_info.mem_read_size = 0;
                t_info.instruction_addr = 0; // FIX

                print_trace_info(&t_info, outputFile);

                // ADD operation
                t_info.uop_opcode_num = 13;
                t_info.uop_opcode = "BINARY";
                t_info.src0 = 9;
                t_info.src1 = 8;
                t_info.dst0 = 9;
                t_info.dst1 = 25;
                t_info.instruction_addr = 0; // FIX
                print_trace_info(&t_info, outputFile);
            }

            // STORE operation for result
            t_info.uop_opcode_num = 30; 
            t_info.uop_opcode = "DATAXFER";
            t_info.st_vaddr = result_address;
            t_info.num_read_regs = 1;
            t_info.num_dest_regs = 0;
            t_info.src0 = 9;
            t_info.mem_write_size = 4;
            t_info.has_st = 1; 
            t_info.instruction_addr = 0; // FIX
            print_trace_info(&t_info, outputFile);
        }
    }

    fclose(outputFile);
    return 0;
}
