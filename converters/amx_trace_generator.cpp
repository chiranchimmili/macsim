#include <stdio.h>
#include <stdint.h>
#include <zlib.h>

#include "amx_mm_converter.h"

#define TILE_ROWS 16        
#define TILE_COLS 16        

#define MATRIX_A_ROWS 16    
#define MATRIX_A_COLS 64    
#define MATRIX_B_ROWS 64    
#define MATRIX_B_COLS 64
#define MATRIX_C_ROWS MATRIX_A_ROWS
#define MATRIX_C_COLS MATRIX_B_COLS

#define MATRIX_A_ADDRESS 0x10000
#define MATRIX_B_ADDRESS 0x20000
#define MATRIX_C_ADDRESS 0x30000

void print_trace_info(TraceInfo* t_info, FILE* file) {
    if (!t_info) {
        fprintf(stderr, "Error: t_info is NULL\n");
        return;
    }
    if (!file) {
        fprintf(stderr, "Error: file is NULL\n");
        return;
    }

    fprintf(file, "*** begin of the data structure ***\n");

    fprintf(file, "t_info->uop_opcode_num %d\n", t_info->uop_opcode_num);
    fprintf(file, "t_info->uop_opcode %s\n", t_info->uop_opcode ? t_info->uop_opcode : "NULL");
    fprintf(file, "t_info->num_read_regs: %d\n", t_info->num_read_regs);
    fprintf(file, "t_info->num_dest_regs: %d\n", t_info->num_dest_regs);

    if (t_info->num_read_regs >= 1) {
        if (t_info->src0 >= 0 && t_info->src0 < sizeof(regs)/sizeof(regs[0])) {
            fprintf(file, "t_info->src0: %d : %s\n", t_info->src0, regs[t_info->src0]);
        } else {
            fprintf(file, "t_info->src0: %d : INVALID INDEX\n", t_info->src0);
        }
        if (t_info->num_read_regs > 1) {
            if (t_info->src1 >= 0 && t_info->src1 < sizeof(regs)/sizeof(regs[0])) {
                fprintf(file, "t_info->src1: %d : %s\n", t_info->src1, regs[t_info->src1]);
            } else {
                fprintf(file, "t_info->src1: %d : INVALID INDEX\n", t_info->src1);
            }
        }
    }

    if (t_info->num_dest_regs >= 1) {
        if (t_info->dst0 >= 0 && t_info->dst0 < sizeof(regs)/sizeof(regs[0])) {
            fprintf(file, "t_info->dst0: %d : %s\n", t_info->dst0, regs[t_info->dst0]);
        } else {
            fprintf(file, "t_info->dst0: %d : INVALID INDEX\n", t_info->dst0);
        }
        if (t_info->num_dest_regs > 1) {
            if (t_info->dst1 >= 0 && t_info->dst1 < sizeof(regs)/sizeof(regs[0])) {
                fprintf(file, "t_info->dst1: %d : %s\n", t_info->dst1, regs[t_info->dst1]);
            } else {
                fprintf(file, "t_info->dst1: %d : INVALID INDEX\n", t_info->dst1);
            }
        }
    }

    // Print remaining fields
    fprintf(file, "t_info->cf_type: %s\n", t_info->cf_type ? t_info->cf_type : "NULL");
    fprintf(file, "t_info->has_immediate: %d\n", t_info->has_immediate);
    fprintf(file, "t_info->r_dir:%d\n", t_info->r_dir);
    fprintf(file, "t_info->has_st: %d\n", t_info->has_st);
    fprintf(file, "t_info->num_ld: %d\n", t_info->num_ld);
    fprintf(file, "t_info->mem_read_size: %lu\n", t_info->mem_read_size);
    fprintf(file, "t_info->mem_write_size: %lu\n", t_info->mem_write_size);
    fprintf(file, "t_info->is_fp: %d\n", t_info->is_fp);
    fprintf(file, "t_info->ld_vaddr1: %lx\n", t_info->ld_vaddr1);
    fprintf(file, "t_info->ld_vaddr2: %lx\n", t_info->ld_vaddr2);
    fprintf(file, "t_info->st_vaddr: %lx\n", t_info->st_vaddr);
    // fprintf(file, "t_info->row_stride: %ld\n", t_info->row_stride);
    fprintf(file, "t_info->instruction_addr: %lx\n", t_info->instruction_addr);
    fprintf(file, "t_info->branch_target: %lx\n", t_info->branch_target);
    fprintf(file, "t_info->actually_taken: %d\n", t_info->actually_taken);
    fprintf(file, "t_info->write_flg: %d\n", t_info->write_flg);
    fprintf(file, "*** end of the data structure ***\n\n");
}

void write_trace_info_to_gz(TraceInfo* t_info, gzFile gzfile) {
    Inst_info *info = new Inst_info;
    info->num_read_regs = t_info->num_read_regs;
    info->num_dest_regs = t_info->num_dest_regs;
    
    if (t_info->num_read_regs >= 1) {
        info->src[0] = t_info->src0;
        if (t_info->num_read_regs > 1) {
            info->src[1] = t_info->src1;
        }
    }
    if (t_info->num_dest_regs >= 1) {
        info->dst[0] = t_info->dst0;
        if (t_info->num_dest_regs > 1) {
            info->dst[1] = t_info->dst1;
        }
    }

    info->cf_type = 0;
    info->has_immediate = t_info->has_immediate;
    info->opcode = t_info->uop_opcode_num;
    info->has_st = t_info->has_st;
    info->is_fp = t_info->is_fp;
    info->write_flg = t_info->write_flg;
    info->num_ld = t_info->num_ld;
    info->size = 8;
    info->ld_vaddr1 = t_info->ld_vaddr1;
    info->ld_vaddr2 = t_info->ld_vaddr2;
    info->st_vaddr = t_info->st_vaddr;
    info->instruction_addr = t_info->instruction_addr;
    info->branch_target = t_info->branch_target;
    info->mem_read_size = t_info->mem_read_size;
    info->mem_write_size = t_info->mem_write_size;
    //info->row_stride = t_info->row_stride;
    info->rep_dir = t_info->r_dir;
    info->actually_taken = t_info->actually_taken;

    gzwrite(gzfile, info, sizeof(Inst_info));
}

void generate_amx_trace(FILE* outputFile, gzFile gz_outputFile) {
    TraceInfo t_info;

    t_info.cf_type = "NOT_CF";
    t_info.has_immediate = 0;
    t_info.r_dir = 1;
    t_info.has_st = 0;
    t_info.num_ld = 0;
    t_info.mem_read_size = 0;
    t_info.mem_write_size = 0;
    t_info.num_dest_regs = 0;
    t_info.is_fp = 0;
    t_info.ld_vaddr1 = 0;
    t_info.ld_vaddr2 = 0;
    t_info.st_vaddr = 0;
    t_info.dst0 = 0;
    t_info.dst1 = 0;
    // t_info.row_stride = 0;
    t_info.instruction_addr = 0x400000; // FIX
    t_info.branch_target = 0;
    t_info.actually_taken = 0;
    t_info.write_flg = 0;

    for (int i = 0; i < MATRIX_A_ROWS; i += TILE_ROWS) {
        for (int j = 0; j < MATRIX_B_COLS; j += TILE_COLS) {
            for (int k = 0; k < MATRIX_A_COLS; k += TILE_COLS) {
                int a_tile_rows = (i + TILE_ROWS < MATRIX_A_ROWS) ? TILE_ROWS : (MATRIX_A_ROWS - i);
                int a_tile_cols = (k + TILE_COLS < MATRIX_A_COLS) ? TILE_COLS : (MATRIX_A_COLS - k);
                int b_tile_rows = a_tile_cols; 
                int b_tile_cols = (j + TILE_COLS < MATRIX_B_COLS) ? TILE_COLS : (MATRIX_B_COLS - j);
                int c_tile_cols = (j + TILE_COLS < MATRIX_C_COLS) ? TILE_COLS : (MATRIX_C_COLS - j);

                uint64_t a_tile_addr = MATRIX_A_ADDRESS + (i * MATRIX_A_COLS + k) * sizeof(float);
                uint64_t b_tile_addr = MATRIX_B_ADDRESS + (k * MATRIX_B_COLS + j) * sizeof(float);
                uint64_t c_tile_addr = MATRIX_C_ADDRESS + (i * MATRIX_C_COLS + j) * sizeof(float);

                // row strides for Matrix A and Matrix B
                uint64_t row_stride_a = MATRIX_A_COLS * sizeof(float);
                uint64_t row_stride_b = MATRIX_B_COLS * sizeof(float);
                uint64_t row_stride_c = MATRIX_C_COLS * sizeof(float);

                // row_strides for partial tiles
                if (a_tile_cols < TILE_COLS) {
                    row_stride_a = a_tile_cols * sizeof(float);
                }
                if (b_tile_cols < TILE_COLS) {
                    row_stride_b = b_tile_cols * sizeof(float);
                }
                if (c_tile_cols < TILE_COLS) {
                    row_stride_c = c_tile_cols * sizeof(float);
                }

                // t_info.uop_opcode_num = 4;  
                // t_info.uop_opcode = "LDTILECFG";
                // t_info.num_read_regs = 1;
                // t_info.src0 = 328;
                // t_info.num_ld = 1;
                // t_info.mem_read_size = 64;
                // t_info.ld_vaddr1 = 0x5000; // dummy
                // t_info.instruction_addr++;
                
                // set others

                // print_trace_info(&t_info, outputFile);
                // write_trace_info_to_gz(&t_info, gz_outputFile);

                // Load tile from Matrix A
                t_info.uop_opcode_num = 4;
                t_info.uop_opcode = "TILELOADD";
                t_info.num_ld = 16;
                t_info.src0 = 0;
                t_info.dst0 = 320;
                t_info.num_dest_regs = 1;
                t_info.num_read_regs = 0;
                // t_info.mem_read_size = a_tile_rows * row_stride_a;
                t_info.mem_read_size = 64;
                t_info.ld_vaddr1 = a_tile_addr;
                //  t_info.row_stride = row_stride_a;
                t_info.instruction_addr++;
                t_info.is_fp = 1;
        
                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);

                // Load tile from Matrix B
                t_info.ld_vaddr1 = b_tile_addr;
               // t_info.row_stride = row_stride_b;
                t_info.dst0 = 321;
                // t_info.mem_read_size = b_tile_rows * row_stride_b;
                t_info.mem_read_size = 64;
                t_info.instruction_addr++;
                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);

                // Perform dot-product
                t_info.uop_opcode_num = 4;
                t_info.uop_opcode = "TDPBF16PS";
                t_info.num_read_regs = 2;
                t_info.num_dest_regs = 1;
                t_info.src0 = 320;
                t_info.src1 = 321;
                t_info.dst0 = 322;
                t_info.num_ld = 0;
                t_info.mem_read_size = 0;
                t_info.is_fp = 1;
                t_info.ld_vaddr1 = 0;
                //  t_info.row_stride = 0;
                t_info.instruction_addr++;

                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);

                // Store result tile (accumulate in Matrix C)
                t_info.uop_opcode_num = 4;
                t_info.uop_opcode = "TILESTORED";
                t_info.st_vaddr = c_tile_addr;
                t_info.num_read_regs = 1;
                t_info.num_dest_regs = 0;
                t_info.has_st = 1;
                t_info.src0 = 322;
                // t_info.mem_write_size = a_tile_rows * row_stride_c;
                t_info.mem_write_size = 64;
                t_info.instruction_addr++;
               
                print_trace_info(&t_info, outputFile);
                write_trace_info_to_gz(&t_info, gz_outputFile);
            }
        }
    }
        // NOP instruction
    t_info.uop_opcode_num = 51;
    t_info.uop_opcode = "NOP";
    t_info.num_read_regs = 0;
    t_info.num_dest_regs = 0;
    t_info.src0 = 0;
    t_info.src1 = 0;
    t_info.dst0 = 0;
    t_info.dst1 = 0;
    t_info.has_st = 0;
    t_info.st_vaddr = 0;
    t_info.mem_write_size = 0;
    t_info.instruction_addr+=4;
    print_trace_info(&t_info, outputFile);
    write_trace_info_to_gz(&t_info, gz_outputFile);
}


int main() {
    gzFile gz_outputFile = gzopen("output.1_0.raw", "wb");
    FILE* outputFile = fopen("output.1.txt", "w");

    generate_amx_trace(outputFile, gz_outputFile);

    fclose(outputFile);
    gzclose(gz_outputFile);

    return 0;
}
