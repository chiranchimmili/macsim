#include <cstdlib>

#include "bp.h"
#include "bp_basic_perceptron.h"
#include "utils.h"
#include "debug_macros.h"
#include "uop.h"

#include "all_knobs.h"

/** Design: 
 * 59-bit branch history length (optimal)
 * 512 perceptrons with 60 weights each
 * 
 */

#define PHT_INIT_VALUE \
  ((0x1 << *KNOB(KNOB_PHT_CTR_BITS)) - 1) /* weakly taken */

///////////////////////////////////////////////////////////////////////////////////////////

// Constructor
bp_basic_perceptron_c::bp_basic_perceptron_c(macsim_c *simBase) : bp_dir_base_c(simBase) {
  // Initialize perceptrons table, with all weights set to value 0
  std::vector<int8_t> zeroVect(60, 0);
  for (int i = 0; i < 512; i++) {
    m_pctt.push_back(zeroVect);
  }
  // 59-bit global history shift register is already initialized in bp.cc
}

uns8 bp_basic_perceptron_c::pred(uop_c *uop) {
  Addr addr = uop->m_pc;
  uint64_t index = hash(addr);
  // get selected perceptron from perceptron table
  std::vector<int8_t> perceptron = m_pctt[index];
  int64_t prediction = 0;
  uns8 branch_taken = 0;

  // input vector first bit is always 1 (biased input) so we can just add weights[0]
  prediction += perceptron[0];
  for (int i = 1; i < 60; i++) {
    if (m_global_hist_59[59 - i - 1] == 1) {
        prediction += perceptron[i];
    } else {
        prediction -= perceptron[i];
    }
  }
  if (prediction >= 0) {
    branch_taken = 1;
  } else {
    branch_taken = 0;
  }
  uop->m_uop_info.m_pred_global_hist_59 = m_global_hist_59;
  // Unsure about this part
  uop->m_recovery_info.m_global_hist_59 = this->m_global_hist_59 | std::bitset<59> (uop->m_dir << 31);

  // Update global history shift register's rightmost bit to prediction
  m_global_hist_59 <<= 1;
  if (branch_taken) {
    m_global_hist_59.set(0, 1);
  } else {
    m_global_hist_59.set(0, 0);
  }

  return branch_taken;
}

void bp_basic_perceptron_c::update(uop_c *uop) {
  
  



}


void bp_basic_perceptron_c::recover(recovery_info_c *recovery_info) {
  m_global_hist_59 = recovery_info->m_global_hist_59;
}


// Hashing function for addresses to index into perceptron table
uint64_t bp_basic_perceptron_c::hash(Addr addr) {
    addr = ((addr >> 16) ^ addr) * 0x45d9f3b;
    addr = ((addr >> 16) ^ addr) * 0x45d9f3b;
    addr = (addr >> 16) ^ addr;
    return addr % 512;
}