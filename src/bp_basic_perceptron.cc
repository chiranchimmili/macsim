#include <cstdlib>

#include "bp.h"
#include "bp_basic_perceptron.h"
#include "utils.h"
#include "debug_macros.h"
#include "uop.h"

#include "all_knobs.h"

/** Design: 59-bit branch history length, 512 perceptrons with 60 weights each
 * 
 */

#define PHT_INIT_VALUE \
  ((0x1 << *KNOB(KNOB_PHT_CTR_BITS)) - 1) /* weakly taken */

///////////////////////////////////////////////////////////////////////////////////////////

// Constructor
bp_basic_perceptron_c::bp_basic_perceptron_c(macsim_c *simBase) : bp_dir_base_c(simBase) {
  // Initialize pattern history table/branch history table (same as gshare)
  m_pht = (uns8 *)malloc(sizeof(uns8) * (0x1 << *KNOB(KNOB_BP_HIST_LENGTH)));
  for (int ii = 0; ii < (0x1 << *KNOB(KNOB_BP_HIST_LENGTH)); ++ii) {
    m_pht[ii] = PHT_INIT_VALUE;
  }
  // Initialize perceptrons table, with all weights set to value 0
  std::vector<int8_t> zeroVect(60, 0);
  for (int i = 0; i < 512; i++) {
    m_pctt.push_back(zeroVect);
  }
  
}

uns8 bp_basic_perceptron_c::pred(uop_c *uop) {

}