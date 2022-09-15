#ifndef BP_BASIC_PERCEPTRON_H_INCLUDED
#define BP_BASIC_PERCEPTRON_H_INCLUDED

#include <vector>
#include "global_defs.h"
#include "bp.h"

class bp_basic_perceptron_c : public bp_dir_base_c
{
public:
  /**
   * Gshare BP constructor
   */
  bp_basic_perceptron_c(macsim_c* simBase);

  /**
   * Gshare BP destructor
   */
  ~bp_basic_perceptron_c(void) {
  }

  /**
   * Predict a branch instruction
   */
  uns8 pred(uop_c* uop);

  /**
   * Update branch predictor when a branch instruction is resolved.
   */
  void update(uop_c* uop);

  /**
   * Called to recover the bp when a misprediction is realized
   */
  void recover(recovery_info_c* recovery_info);

  uint64_t hash(Addr addr);

private:
  /**
   * Private constructor
   * Do not implement
   */
  bp_basic_perceptron_c(const bp_basic_perceptron_c& rhs);

  /**
   * Overridden operator =
   */
  const bp_basic_perceptron_c& operator=(const bp_basic_perceptron_c& rhs);
};



#endif // BP_BASIC_PERCEPTRON_H_INCLUDED