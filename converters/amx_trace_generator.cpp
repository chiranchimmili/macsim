#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <zlib.h>

#include "amx_mm_converter.h"

#define TILE_ROWS 16        
#define TILE_COLS 16        

#define MATRIX_C_ROWS MATRIX_A_ROWS
#define MATRIX_C_COLS MATRIX_B_COLS

#define MATRIX_A_ADDRESS 0x10000
#define MATRIX_B_ADDRESS 0x20000
#define MATRIX_C_ADDRESS 0x30000

typedef struct {
    int matrix_a_rows;
    int matrix_a_cols;
    int matrix_b_rows;
    int matrix_b_cols;
} MatrixDimensions;

void print_trace_info(TraceInfo* t_info, FILE* file);
void write_trace_info_to_gz(TraceInfo* t_info, gzFile gzfile);
void generate_amx_trace(FILE* outputFile, gzFile gz_outputFile, MatrixDimensions dims);

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <MATRIX_A_ROWS> <MATRIX_A_COLS> <MATRIX_B_ROWS> <MATRIX_B_COLS>\n", argv[0]);
        return 1;
    }

    MatrixDimensions dims;
    dims.matrix_a_rows = atoi(argv[1]);
    dims.matrix_a_cols = atoi(argv[2]);
    dims.matrix_b_rows = atoi(argv[3]);
    dims.matrix_b_cols = atoi(argv[4]);

    gzFile gz_outputFile = gzopen("output.1_0.raw", "wb");
    FILE* outputFile = fopen("output.1.txt", "w");

    generate_amx_trace(outputFile, gz_outputFile, dims);

    fclose(outputFile);
    gzclose(gz_outputFile);

    return 0;
}

void print_trace_info(TraceInfo* t_info, FILE* file) {
    if (!t_info || !file) {
        fprintf(stderr, "Error: t_info or file is NULL\n");
        return;
    }

    fprintf(file, "*** begin of the data structure ***\n");
    fprintf(file, "t_info->uop_opcode_num %d\n", t_info->uop_opcode_num);
    fprintf(file, "t_info->uop_opcode %s\n", t_info->uop_opcode ? t_info->uop_opcode : "NULL");
    fprintf(file, "t_info->num_read_regs: %d\n", t_info->num_read_regs);
    fprintf(file, "t_info->num_dest_regs: %d\n", t_info->num_dest_regs);

    if (t_info->num_read_regs >= 1) {
        fprintf(file, "t_info->src0: %d : %s\n", t_info->src0, (t_info->src0 >= 0 && t_info->src0 < sizeof(regs)/sizeof(regs[0])) ? regs[t_info->src0] : "INVALID INDEX");
        if (t_info->num_read_regs > 1) {
            fprintf(file, "t_info->src1: %d : %s\n", t_info->src1, (t_info->src1 >= 0 && t_info->src1 < sizeof(regs)/sizeof(regs[0])) ? regs[t_info->src1] : "INVALID INDEX");
        }
    }

    if (t_info->num_dest_regs >= 1) {
        fprintf(file, "t_info->dst0: %d : %s\n", t_info->dst0, (t_info->dst0 >= 0 && t_info->dst0 < sizeof(regs)/sizeof(regs[0])) ? regs[t_info->dst0] : "INVALID INDEX");
        if (t_info->num_dest_regs > 1) {
            fprintf(file, "t_info->dst1: %d : %s\n", t_info->dst1, (t_info->dst1 >= 0 && t_info->dst1 < sizeof(regs)/sizeof(regs[0])) ? regs[t_info->dst1] : "INVALID INDEX");
        }
    }

    fprintf(file, "t_info->cf_type: %s\n", t_info->cf_type ? t_info->cf_type : "NULL");
    fprintf(file, "t_info->has_immediate: %d\n", t_info->has_immediate);
    fprintf(file, "t_info->r_dir: %d\n", t_info->r_dir);
    fprintf(file, "t_info->has_st: %d\n", t_info->has_st);
    fprintf(file, "t_info->num_ld: %d\n", t_info->num_ld);
    fprintf(file, "t_info->mem_read_size: %lu\n", t_info->mem_read_size);
    fprintf(file, "t_info->mem_write_size: %lu\n", t_info->mem_write_size);
    fprintf(file, "t_info->is_fp: %d\n", t_info->is_fp);
    fprintf(file, "t_info->ld_vaddr1: %lx\n", t_info->ld_vaddr1);
    fprintf(file, "t_info->ld_vaddr2: %lx\n", t_info->ld_vaddr2);
    fprintf(file, "t_info->st_vaddr: %lx\n", t_info->st_vaddr);
    fprintf(file, "t_info->instruction_addr: %lx\n", t_info->instruction_addr);
    fprintf(file, "t_info->branch_target: %lx\n", t_info->branch_target);
    fprintf(file, "t_info->actually_taken: %d\n", t_info->actually_taken);
    fprintf(file, "t_info->write_flg: %d\n", t_info->write_flg);
    fprintf(file, "*** end of the data structure ***\n\n");
}

void write_trace_info_to_gz(TraceInfo* t_info, gzFile gzfile) {
    Inst_info info = {
        .num_read_regs = t_info->num_read_regs,
        .num_dest_regs = t_info->num_dest_regs,
        .cf_type = 0,
        .has_immediate = t_info->has_immediate,
        .opcode = t_info->uop_opcode_num,
        .has_st = t_info->has_st,
        .is_fp = t_info->is_fp,
        .write_flg = t_info->write_flg,
        .num_ld = t_info->num_ld,
        .size = 8,
        .ld_vaddr1 = t_info->ld_vaddr1,
        .ld_vaddr2 = t_info->ld_vaddr2,
        .st_vaddr = t_info->st_vaddr,
        .instruction_addr = t_info->instruction_addr,
        .branch_target = t_info->branch_target,
        .mem_read_size = t_info->mem_read_size,
        .mem_write_size = t_info->mem_write_size,
        .rep_dir = t_info->r_dir,
        .actually_taken = t_info->actually_taken
    };

    if (t_info->num_read_regs >= 1) {
        info.src[0] = t_info->src0;
        if (t_info->num_read_regs > 1) {
            info.src[1] = t_info->src1;
        }
    }
    if (t_info->num_dest_regs >= 1) {
        info.dst[0] = t_info->dst0;
        if (t_info->num_dest_regs > 1) {
            info.dst[1] = t_info->dst1;
        }
    }

    gzwrite(gzfile, &info, sizeof(Inst_info));
}

void generate_amx_trace(FILE* outputFile, gzFile gz_outputFile, MatrixDimensions dims) {
    TraceInfo t_info = {
        .cf_type = "NOT_CF",
        .has_immediate = 0,
        .r_dir = 1,
        .has_st = 0,
        .num_ld = 0,
        .mem_read_size = 0,
        .mem_write_size = 0,
        .num_dest_regs = 0,
        .is_fp = 1,
        .ld_vaddr1 = 0,
        .ld_vaddr2 = 0,
        .st_vaddr = 0,
        .dst0 = 0,
        .dst1 = 0,
        .instruction_addr = 0x400000,
        .branch_target = 0,
        .actually_taken = 0,
        .write_flg = 0
    };

    for (int i = 0; i < dims.matrix_a_rows; i += TILE_ROWS) {
        for (int j = 0; j < dims.matrix_b_cols; j += TILE_COLS) {
            for (int k = 0; k < dims.matrix_a_cols; k += TILE_COLS) {
                uint64_t a_tile_addr = MATRIX_A_ADDRESS + (i * dims.matrix_a_cols + k) * sizeof(float);
                uint64_t b_tile_addr = MATRIX_B_ADDRESS + (k * dims.matrix_b_cols + j) * sizeof(float);
                uint64_t c_tile_addr = MATRIX_C_ADDRESS + (i * dims.matrix_b_cols + j) * sizeof(float);

                t_info.uop_opcode_num = 4;
                t_info.uop_opcode = "TILELOADD";
                t_info.num_ld = 16;
                t_info.dst0 = 320;
                t_info.mem_read_size = 64;
                t_info.ld_vaddr1 = a_tile_addr;
                t_info.instruction_addr++;

                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);

                t_info.ld_vaddr1 = b_tile_addr;
                t_info.dst0 = 321;
                t_info.instruction_addr++;

                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);

                t_info.uop_opcode = "TDPBF16PS";
                t_info.num_read_regs = 2;
                t_info.num_dest_regs = 1;
                t_info.src0 = 320;
                t_info.src1 = 321;
                t_info.dst0 = 322;
                t_info.num_ld = 0;
                t_info.mem_read_size = 0;
                t_info.instruction_addr++;

                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);

                t_info.uop_opcode = "TILESTORED";
                t_info.st_vaddr = c_tile_addr;
                t_info.num_read_regs = 1;
                t_info.num_dest_regs = 0;
                t_info.has_st = 1;
                t_info.src0 = 322;
                t_info.mem_write_size = 64;
                t_info.instruction_addr++;

                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);
            }
        }
    }

    t_info.uop_opcode = "NOP";
    t_info.uop_opcode_num = 51;
    t_info.instruction_addr += 4;

    print_trace_info(&t_info, outputFile);
    write_trace_info_to_gz(&t_info, gz_outputFile);
}
