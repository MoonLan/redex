/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include "DexInstruction.h"

#include "Debug.h"
#include "DexIdx.h"
#include "DexOutput.h"
#include "Warning.h"

DexOpcodeFormat opcode_format(DexOpcode opcode) {
  switch (opcode) {
#define OP(op, code, fmt) \
  case code:              \
    return FMT_##fmt;
    OPS
#undef OP
  case FOPCODE_PACKED_SWITCH : return FMT_fopcode;
  case FOPCODE_SPARSE_SWITCH:
    return FMT_fopcode;
  case FOPCODE_FILLED_ARRAY:
    return FMT_fopcode;
  }
  always_assert_log(false, "Unexpected opcode 0x%x", opcode);
}

unsigned DexInstruction::count_from_opcode() const {
  static int args[] = {
      0, /* FMT_f00x   */
      0, /* FMT_f10x   */
      0, /* FMT_f12x   */
      0, /* FMT_f12x_2 */
      0, /* FMT_f11n   */
      0, /* FMT_f11x_d */
      0, /* FMT_f11x_s */
      0, /* FMT_f10t   */
      1, /* FMT_f20t   */
      0, /* FMT_f20bc  */
      1, /* FMT_f22x   */
      1, /* FMT_f21t   */
      1, /* FMT_f21s   */
      1, /* FMT_f21h   */
      0, /* FMT_f21c_d */
      0, /* FMT_f21c_s */
      1, /* FMT_f23x_d */
      1, /* FMT_f23x_s */
      1, /* FMT_f22b   */
      1, /* FMT_f22t   */
      1, /* FMT_f22s   */
      0, /* FMT_f22c_d */
      0, /* FMT_f22c_s */
      0, /* FMT_f22cs  */
      2, /* FMT_f30t   */
      2, /* FMT_f32x   */
      2, /* FMT_f31i   */
      2, /* FMT_f31t   */
      1, /* FMT_f31c   */
      1, /* FMT_f35c   */
      2, /* FMT_f35ms  */
      2, /* FMT_f35mi  */
      1, /* FMT_f3rc   */
      2, /* FMT_f3rms  */
      2, /* FMT_f3rmi  */
      4, /* FMT_f51l   */
      1, /* FMT_f41c_d */
      1, /* FMT_f41c_s */
      2, /* FMT_f52c_d */
      2, /* FMT_f52c_s */
      2, /* FMT_f5rc */
      2, /* FMT_f57c */
      0, /* FMT_fopcode   */
  };
  return args[opcode_format(opcode())];
};

DexOpcode DexInstruction::opcode() const {
  auto opcode = m_opcode & 0xff;
  if (opcode == OPCODE_NOP) {
    // Get the full opcode for pseudo-ops.
    return static_cast<DexOpcode>(m_opcode);
  }
  return static_cast<DexOpcode>(opcode);
}

DexInstruction* DexInstruction::set_opcode(DexOpcode op) {
  if (op >= FOPCODE_PACKED_SWITCH) {
    m_opcode = op;
  } else {
    m_opcode = (m_opcode & 0xff00) | op;
  }
  return this;
}

unsigned DexInstruction::dests_size() const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f00x:
  case FMT_f10x:
  case FMT_f11x_s:
  case FMT_f10t:
  case FMT_f20t:
  case FMT_f21t:
  case FMT_f21c_s:
  case FMT_f23x_s:
  case FMT_f22t:
  case FMT_f22c_s:
  case FMT_f30t:
  case FMT_f31t:
  case FMT_f35c:
  case FMT_f3rc:
  case FMT_f41c_s:
  case FMT_f52c_s:
  case FMT_f5rc:
  case FMT_f57c:
  case FMT_fopcode:
    return 0;
  case FMT_f12x:
  case FMT_f12x_2:
  case FMT_f11n:
  case FMT_f11x_d:
  case FMT_f22x:
  case FMT_f21s:
  case FMT_f21h:
  case FMT_f21c_d:
  case FMT_f23x_d:
  case FMT_f22b:
  case FMT_f22s:
  case FMT_f22c_d:
  case FMT_f32x:
  case FMT_f31i:
  case FMT_f31c:
  case FMT_f51l:
  case FMT_f41c_d:
  case FMT_f52c_d:
    return 1;
  case FMT_f20bc:
  case FMT_f22cs:
  case FMT_f35ms:
  case FMT_f35mi:
  case FMT_f3rms:
  case FMT_f3rmi:
    always_assert_log(false, "Unimplemented opcode `%s'", SHOW(this));
  }
  not_reached();
}

unsigned DexInstruction::srcs_size() const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f00x:
  case FMT_f10x:
  case FMT_f11n:
  case FMT_f11x_d:
  case FMT_f10t:
  case FMT_f20t:
  case FMT_f21s:
  case FMT_f21h:
  case FMT_f21c_d:
  case FMT_f30t:
  case FMT_f31i:
  case FMT_f31c:
  case FMT_f51l:
  case FMT_f41c_d:
  case FMT_fopcode:
    return 0;
  case FMT_f12x:
  case FMT_f11x_s:
  case FMT_f22x:
  case FMT_f21t:
  case FMT_f21c_s:
  case FMT_f22b:
  case FMT_f22s:
  case FMT_f22c_d:
  case FMT_f32x:
  case FMT_f31t:
  case FMT_f3rc:
  case FMT_f41c_s:
  case FMT_f52c_d:
  case FMT_f5rc:
    return 1;
  case FMT_f12x_2:
  case FMT_f23x_d:
  case FMT_f22t:
  case FMT_f22c_s:
  case FMT_f52c_s:
    return 2;
  case FMT_f23x_s:
    return 3;
  case FMT_f35c:
  case FMT_f57c:
    return arg_word_count();
  case FMT_f20bc:
  case FMT_f22cs:
  case FMT_f35ms:
  case FMT_f35mi:
  case FMT_f3rms:
  case FMT_f3rmi:
    always_assert_log(false, "Unimplemented opcode `%s'", SHOW(this));
  }
  not_reached();
}

bool DexInstruction::dest_is_src() const {
  auto format = opcode_format(opcode());
  return format == FMT_f12x_2;
}

bool DexInstruction::src_is_wide(int i) const {
  switch (opcode()) {
  case OPCODE_MOVE_WIDE:
  case OPCODE_MOVE_WIDE_FROM16:
  case OPCODE_MOVE_WIDE_16:
  case OPCODE_RETURN_WIDE:
    return i == 0;

  case OPCODE_CMPL_DOUBLE:
  case OPCODE_CMPG_DOUBLE:
  case OPCODE_CMP_LONG:
    return i == 0 || i == 1;

  case OPCODE_APUT_WIDE:
  case OPCODE_IPUT_WIDE:
  case OPCODE_SPUT_WIDE:
  case OPCODE_IPUT_WIDE_JUMBO:
  case OPCODE_SPUT_WIDE_JUMBO:
    return i == 0;

  case OPCODE_NEG_LONG:
  case OPCODE_NOT_LONG:
  case OPCODE_NEG_DOUBLE:
  case OPCODE_LONG_TO_INT:
  case OPCODE_LONG_TO_FLOAT:
  case OPCODE_LONG_TO_DOUBLE:
  case OPCODE_DOUBLE_TO_INT:
  case OPCODE_DOUBLE_TO_LONG:
  case OPCODE_DOUBLE_TO_FLOAT:
    return i == 0;

  case OPCODE_ADD_LONG:
  case OPCODE_SUB_LONG:
  case OPCODE_MUL_LONG:
  case OPCODE_DIV_LONG:
  case OPCODE_REM_LONG:
  case OPCODE_AND_LONG:
  case OPCODE_OR_LONG:
  case OPCODE_XOR_LONG:
  case OPCODE_ADD_DOUBLE:
  case OPCODE_SUB_DOUBLE:
  case OPCODE_MUL_DOUBLE:
  case OPCODE_DIV_DOUBLE:
  case OPCODE_REM_DOUBLE:
  case OPCODE_ADD_LONG_2ADDR:
  case OPCODE_SUB_LONG_2ADDR:
  case OPCODE_MUL_LONG_2ADDR:
  case OPCODE_DIV_LONG_2ADDR:
  case OPCODE_REM_LONG_2ADDR:
  case OPCODE_AND_LONG_2ADDR:
  case OPCODE_OR_LONG_2ADDR:
  case OPCODE_XOR_LONG_2ADDR:
  case OPCODE_ADD_DOUBLE_2ADDR:
  case OPCODE_SUB_DOUBLE_2ADDR:
  case OPCODE_MUL_DOUBLE_2ADDR:
  case OPCODE_DIV_DOUBLE_2ADDR:
  case OPCODE_REM_DOUBLE_2ADDR:
    return i == 0 || i == 1;

  case OPCODE_SHL_LONG:
  case OPCODE_SHR_LONG:
  case OPCODE_USHR_LONG:
  case OPCODE_SHL_LONG_2ADDR:
  case OPCODE_SHR_LONG_2ADDR:
  case OPCODE_USHR_LONG_2ADDR:
    return i == 0;

  default:
    return false;
  }
}

bool DexInstruction::is_wide() const {
  return src_is_wide(0) || src_is_wide(1) || dest_is_wide();
}

bool DexInstruction::dest_is_wide() const {
  switch (opcode()) {
  case OPCODE_MOVE_WIDE:
  case OPCODE_MOVE_WIDE_FROM16:
  case OPCODE_MOVE_WIDE_16:
  case OPCODE_MOVE_RESULT_WIDE:
    return true;

  case OPCODE_CONST_WIDE_16:
  case OPCODE_CONST_WIDE_32:
  case OPCODE_CONST_WIDE:
  case OPCODE_CONST_WIDE_HIGH16:
    return true;

  case OPCODE_AGET_WIDE:
  case OPCODE_IGET_WIDE:
  case OPCODE_SGET_WIDE:
  case OPCODE_IGET_WIDE_JUMBO:
  case OPCODE_SGET_WIDE_JUMBO:
    return true;

  case OPCODE_NEG_LONG:
  case OPCODE_NOT_LONG:
  case OPCODE_NEG_DOUBLE:
  case OPCODE_INT_TO_LONG:
  case OPCODE_INT_TO_DOUBLE:
  case OPCODE_LONG_TO_DOUBLE:
  case OPCODE_FLOAT_TO_LONG:
  case OPCODE_FLOAT_TO_DOUBLE:
  case OPCODE_DOUBLE_TO_LONG:
    return true;

  case OPCODE_ADD_LONG:
  case OPCODE_SUB_LONG:
  case OPCODE_MUL_LONG:
  case OPCODE_DIV_LONG:
  case OPCODE_REM_LONG:
  case OPCODE_AND_LONG:
  case OPCODE_OR_LONG:
  case OPCODE_XOR_LONG:
  case OPCODE_SHL_LONG:
  case OPCODE_SHR_LONG:
  case OPCODE_USHR_LONG:
  case OPCODE_ADD_DOUBLE:
  case OPCODE_SUB_DOUBLE:
  case OPCODE_MUL_DOUBLE:
  case OPCODE_DIV_DOUBLE:
  case OPCODE_REM_DOUBLE:
  case OPCODE_ADD_LONG_2ADDR:
  case OPCODE_SUB_LONG_2ADDR:
  case OPCODE_MUL_LONG_2ADDR:
  case OPCODE_DIV_LONG_2ADDR:
  case OPCODE_REM_LONG_2ADDR:
  case OPCODE_AND_LONG_2ADDR:
  case OPCODE_OR_LONG_2ADDR:
  case OPCODE_XOR_LONG_2ADDR:
  case OPCODE_SHL_LONG_2ADDR:
  case OPCODE_SHR_LONG_2ADDR:
  case OPCODE_USHR_LONG_2ADDR:
  case OPCODE_ADD_DOUBLE_2ADDR:
  case OPCODE_SUB_DOUBLE_2ADDR:
  case OPCODE_MUL_DOUBLE_2ADDR:
  case OPCODE_DIV_DOUBLE_2ADDR:
  case OPCODE_REM_DOUBLE_2ADDR:
    return true;

  default:
    return false;
  }
}

int DexInstruction::src_bit_width(int i) const {
  switch (opcode_format(opcode())) {
  case FMT_f00x:    assert(false);
  case FMT_f10x:    assert(false);
  case FMT_f12x:    assert(i == 0); return 4;
  case FMT_f12x_2:  assert(i <= 1); return 4;
  case FMT_f11n:    assert(false);
  case FMT_f11x_d:  assert(false);
  case FMT_f11x_s:  assert(i == 0); return 8;
  case FMT_f10t:    assert(false);
  case FMT_f20t:    assert(false);
  case FMT_f20bc:   assert(false);
  case FMT_f22x:    assert(i == 0); return 16;
  case FMT_f21t:    assert(i == 0); return 8;
  case FMT_f21s:    assert(false);
  case FMT_f21h:    assert(false);
  case FMT_f21c_d:  assert(false);
  case FMT_f21c_s:  assert(i == 0); return 8;
  case FMT_f23x_d:  assert(i <= 1); return 8;
  case FMT_f23x_s:  assert(i <= 2); return 8;
  case FMT_f22b:    assert(i == 0); return 8;
  case FMT_f22t:    assert(i <= 1); return 4;
  case FMT_f22s:    assert(i == 0); return 4;
  case FMT_f22c_d:  assert(i == 0); return 4;
  case FMT_f22c_s:  assert(i <= 1); return 4;
  case FMT_f22cs:   assert(false);
  case FMT_f30t:    assert(false);
  case FMT_f32x:    assert(i == 0); return 16;
  case FMT_f31i:    assert(false);
  case FMT_f31t:    assert(i == 0); return 8;
  case FMT_f31c:    assert(false);
  case FMT_f35c:    assert(i <= 4); return 4;
  case FMT_f3rc:    assert(i == 0); return 16;
  case FMT_f41c_d:  assert(false);
  case FMT_f41c_s:  assert(i == 0);  return 16;
  case FMT_f52c_d:  assert(i == 0);  return 16;
  case FMT_f52c_s:  assert(i <= 1);  return 16;
  case FMT_f5rc:    assert(i == 0);  return 16;
  case FMT_f57c:    assert(i <= 6);  return 4;
  case FMT_f35ms:
  case FMT_f35mi:
  case FMT_f3rms:
  case FMT_f3rmi:
  case FMT_f51l:
  case FMT_fopcode:
  default:          assert(false);
  }
  not_reached();
}

int DexInstruction::dest_bit_width() const {
  switch (opcode_format(opcode())) {
  case FMT_f00x:    assert(false);
  case FMT_f10x:    assert(false);
  case FMT_f12x:    return 4;
  case FMT_f12x_2:  return 4;
  case FMT_f11n:    return 4;
  case FMT_f11x_d:  return 8;
  case FMT_f11x_s:  assert(false);
  case FMT_f10t:    assert(false);
  case FMT_f20t:    assert(false);
  case FMT_f20bc:   assert(false);
  case FMT_f22x:    return 8;
  case FMT_f21t:    assert(false);
  case FMT_f21s:    return 8;
  case FMT_f21h:    return 8;
  case FMT_f21c_d:  return 8;
  case FMT_f21c_s:  assert(false);
  case FMT_f23x_d:  return 8;
  case FMT_f23x_s:  assert(false);
  case FMT_f22b:    return 8;
  case FMT_f22t:    assert(false);
  case FMT_f22s:    return 4;
  case FMT_f22c_d:  return 4;
  case FMT_f22c_s:  assert(false);
  case FMT_f22cs:   assert(false);
  case FMT_f30t:    assert(false);
  case FMT_f32x:    return 16;
  case FMT_f31i:    return 8;
  case FMT_f31t:    assert(false);
  case FMT_f31c:    return 8;
  case FMT_f35c:    assert(false);
  case FMT_f35ms:
  case FMT_f35mi:
  case FMT_f3rc:
  case FMT_f3rms:
  case FMT_f3rmi:   assert(false);
  case FMT_f51l:    return 8;
  case FMT_f41c_d:  return 16;
  case FMT_f41c_s:  assert(false);
  case FMT_f52c_d:  return 16;
  case FMT_f52c_s:  assert(false);
  case FMT_f5rc:    assert(false);
  case FMT_f57c:    assert(false);
  case FMT_fopcode:
  default:          assert(false);
  }
  not_reached();
}

uint16_t DexInstruction::dest() const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f12x:
  case FMT_f12x_2:
  case FMT_f11n:
  case FMT_f22s:
  case FMT_f22c_d:
  case FMT_f22cs:
    return (m_opcode >> 8) & 0xf;
  case FMT_f11x_d:
  case FMT_f22x:
  case FMT_f21s:
  case FMT_f21h:
  case FMT_f21c_d:
  case FMT_f23x_d:
  case FMT_f22b:
  case FMT_f31i:
  case FMT_f31c:
  case FMT_f51l:
    return (m_opcode >> 8) & 0xff;
  case FMT_f32x:
    return m_arg[0];
  case FMT_f41c_d:
  case FMT_f52c_d:
    return m_arg[0];
  default:
    // All other formats do not define a destination register.
    always_assert_log(false, "Unhandled opcode: %s", SHOW(this));
  }
  not_reached();
}

DexInstruction* DexInstruction::set_dest(uint16_t vreg) {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f12x:
  case FMT_f12x_2:
  case FMT_f11n:
  case FMT_f22s:
  case FMT_f22c_d:
  case FMT_f22cs:
    assert((vreg & 0xf) == vreg);
    m_opcode = (m_opcode & 0xf0ff) | (vreg << 8);
    return this;
  case FMT_f11x_d:
  case FMT_f22x:
  case FMT_f21s:
  case FMT_f21h:
  case FMT_f21c_d:
  case FMT_f23x_d:
  case FMT_f22b:
  case FMT_f31i:
  case FMT_f31c:
  case FMT_f51l:
    assert((vreg & 0xff) == vreg);
    m_opcode = (m_opcode & 0x00ff) | (vreg << 8);
    return this;
  case FMT_f32x:
    m_arg[0] = vreg;
    return this;
  case FMT_f41c_d:
  case FMT_f52c_d:
    m_arg[0] = vreg;
    return this;
  default:
    // All other formats do not define a destination register.
    always_assert_log(false, "Unhandled opcode: %s", SHOW(this));
  }
  not_reached();
}

uint16_t DexInstruction::src(int i) const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f11x_s:
  case FMT_f21t:
  case FMT_f21c_s:
  case FMT_f31t:
    assert(i == 0);
    return (m_opcode >> 8) & 0xff;
  case FMT_f12x:
  case FMT_f22s:
  case FMT_f22c_d:
    assert(i == 0);
    return (m_opcode >> 12) & 0xf;
  case FMT_f12x_2:
    assert(i < 2);
    if (i == 0) return (m_opcode >> 8) & 0xf;
    return (m_opcode >> 12) & 0xf;
  case FMT_f22x:
  case FMT_f3rc:
    assert(i == 0);
    return m_arg[0];
  case FMT_f23x_d:
    assert(i < 2);
    if (i == 0) return m_arg[0] & 0xff;
    return (m_arg[0] >> 8) & 0xff;
  case FMT_f23x_s:
    assert(i < 3);
    if (i == 0) return (m_opcode >> 8) & 0xff;
    if (i == 1) return m_arg[0] & 0xff;
    return (m_arg[0] >> 8) & 0xff;
  case FMT_f22b:
    assert(i == 0);
    return m_arg[0] & 0xff;
  case FMT_f22t:
  case FMT_f22c_s:
    assert(i < 2);
    if (i == 0) return (m_opcode >> 8) & 0xf;
    if (i == 1) return (m_opcode >> 12) & 0xf;
  case FMT_f32x:
    assert(i == 0);
    return m_arg[1];
  case FMT_f35c:
    assert(i < 5);
    switch (i) {
    case 0:
      return m_arg[0] & 0xf;
    case 1:
      return (m_arg[0] >> 4) & 0xf;
    case 2:
      return (m_arg[0] >> 8) & 0xf;
    case 3:
      return (m_arg[0] >> 12) & 0xf;
    case 4:
      return (m_opcode >> 8) & 0xf;
    }
  case FMT_f41c_s:
    assert(i == 0);
    return m_arg[0];
  case FMT_f52c_d:
    assert(i == 0);
    return m_arg[1];
  case FMT_f52c_s:
    assert(i <= 1);
    return m_arg[i];
  case FMT_f5rc:
    assert(i == 0);
    return m_arg[1];
  case FMT_f57c:
    assert(i <= 6);
    switch(i) {
    case 0:
      return (m_arg[0] >> 4) & 0xf;
    case 1:
      return (m_arg[0] >> 8) & 0xf;
    case 2:
      return (m_arg[0] >> 12) & 0xf;
    case 3:
      return m_arg[1] & 0xf;
    case 4:
      return (m_arg[1] >> 4) & 0xf;
    case 5:
      return (m_arg[1] >> 8) & 0xf;
    case 6:
      return (m_arg[1] >> 12) & 0xf;
    }
  default:
    // All other formats do not define source registers.
    always_assert_log(false, "Unhandled opcode: %s", SHOW(this));
  }
  not_reached();
}

DexInstruction* DexInstruction::set_src(int i, uint16_t vreg) {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f11x_s:
  case FMT_f21t:
  case FMT_f21c_s:
  case FMT_f31t:
    assert(i == 0);
    assert((vreg & 0xff) == vreg);
    m_opcode = (m_opcode & 0x00ff) | (vreg << 8);
    return this;
  case FMT_f12x:
  case FMT_f22s:
  case FMT_f22c_d:
    assert(i == 0);
    assert((vreg & 0xf) == vreg);
    m_opcode = (m_opcode & 0x0fff) | (vreg << 12);
    return this;
  case FMT_f12x_2:
    assert(i < 2);
    assert((vreg & 0xf) == vreg);
    if (i == 0) {
      m_opcode = (m_opcode & 0xf0ff) | (vreg << 8);
    } else {
      m_opcode = (m_opcode & 0x0fff) | (vreg << 12);
    }
    return this;
  case FMT_f22x:
  case FMT_f3rc:
    assert(i == 0);
    m_arg[0] = vreg;
    return this;
  case FMT_f23x_d:
    assert(i < 2);
    assert((vreg & 0xff) == vreg);
    if (i == 0) {
      m_arg[0] = (m_arg[0] & 0xff00) | vreg;
      return this;
    }
    m_arg[0] = (m_arg[0] & 0x00ff) | (vreg << 8);
    return this;
  case FMT_f23x_s:
    assert(i < 3);
    assert((vreg & 0xff) == vreg);
    if (i == 0) {
      m_opcode = (m_opcode & 0x00ff) | (vreg << 8);
    } else if (i == 1) {
      m_arg[0] = (m_arg[0] & 0xff00) | vreg;
    } else {
      m_arg[0] = (m_arg[0] & 0x00ff) | (vreg << 8);
    }
    return this;
  case FMT_f22b:
    assert(i == 0);
    m_arg[0] = (m_arg[0] & 0xff00) | vreg;
    return this;
  case FMT_f22t:
  case FMT_f22c_s:
    assert(i < 2);
    assert((vreg & 0xf) == vreg);
    if (i == 0) {
      m_opcode = (m_opcode & 0xf0ff) | (vreg << 8);
    } else {
      m_opcode = (m_opcode & 0x0fff) | (vreg << 12);
    }
    return this;
  case FMT_f32x:
    assert(i == 0);
    m_arg[1] = vreg;
    return this;
  case FMT_f35c:
    assert(i < 5);
    assert((vreg & 0xf) == vreg);
    switch (i) {
    case 0:
      m_arg[0] = (m_arg[0] & 0xfff0) | vreg;
      return this;
    case 1:
      m_arg[0] = (m_arg[0] & 0xff0f) | (vreg << 4);
      return this;
    case 2:
      m_arg[0] = (m_arg[0] & 0xf0ff) | (vreg << 8);
      return this;
    case 3:
      m_arg[0] = (m_arg[0] & 0x0fff) | (vreg << 12);
      return this;
    case 4:
      m_opcode = (m_opcode & 0xf0ff) | (vreg << 8);
      return this;
    }
  case FMT_f41c_s:
    assert(i == 0);
    m_arg[0] = vreg;
    return this;
  case FMT_f52c_d:
    assert(i == 0);
    m_arg[1] = vreg;
    return this;
  case FMT_f52c_s:
    assert(i <= 1);
    m_arg[i] = vreg;
    return this;
  case FMT_f5rc:
    assert(i == 0);
    m_arg[1] = vreg;
    return this;
  case FMT_f57c:
    assert(i <= 6);
    assert((vreg & 0xf) == vreg);
    switch (i) {
    case 0:
      m_arg[0] = (m_arg[0] & 0xff0f) | (vreg << 4);
      return this;
    case 1:
      m_arg[0] = (m_arg[0] & 0xf0ff) | (vreg << 8);
      return this;
    case 2:
      m_arg[0] = (m_arg[0] & 0x0fff) | (vreg << 12);
      return this;
    case 3:
      m_arg[1] = (m_arg[1] & 0xfff0) | vreg;
      return this;
    case 4:
      m_arg[0] = (m_arg[1] & 0xff0f) | (vreg << 4);
      return this;
    case 5:
      m_arg[0] = (m_arg[1] & 0xf0ff) | (vreg << 8);
      return this;
    case 6:
      m_arg[0] = (m_arg[1] & 0x0fff) | (vreg << 12);
      return this;
    }
  default:
    // All other formats do not define source registers.
    always_assert_log(false, "Unhandled opcode: %s", SHOW(this));
  }
  not_reached();
}

bool DexInstruction::has_literal() const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f11n:
  case FMT_f21s:
  case FMT_f21h:
  case FMT_f22b:
  case FMT_f22s:
  case FMT_f31i:
  case FMT_f51l:
    return true;
  default:
    return false;
  }
  not_reached();
}

template <int Width>
int64_t signext(uint64_t uv) {
  int shift = 64 - Width;
  return int64_t(uint64_t(uv) << shift) >> shift;
}

int64_t DexInstruction::literal() const {
  assert(has_literal());
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f11n:
    return signext<4>(m_opcode >> 12);
  case FMT_f21s:
    return signext<16>(m_arg[0]);
  case FMT_f21h:
    return signext<16>(m_arg[0]) << 16;
  case FMT_f22b:
    return signext<16>(m_arg[0]) << 48;
  case FMT_f22s:
    return signext<16>(m_arg[0]);
  case FMT_f31i: {
    auto literal = uint32_t(m_arg[0]) | (uint32_t(m_arg[1]) << 16);
    return signext<32>(literal);
  }
  case FMT_f51l: {
    auto literal = uint64_t(m_arg[0]) | (uint64_t(m_arg[1]) << 16) |
                   (uint64_t(m_arg[2]) << 32) | (uint64_t(m_arg[3]) << 48);
    return signext<64>(literal);
  }
  default:
    assert(false);
  }
  not_reached();
}

DexInstruction* DexInstruction::set_literal(int64_t literal) {
  assert(has_literal());
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f11n:
    m_opcode = (m_opcode & 0xfff) | ((literal & 0xf) << 12);
    return this;
  case FMT_f21s:
    m_arg[0] = literal;
    return this;
  case FMT_f21h:
    m_arg[0] = literal >> 16;
    return this;
  case FMT_f22b:
    m_arg[0] = literal >> 48;
    return this;
  case FMT_f22s:
    m_arg[0] = literal;
    return this;
  case FMT_f31i:
    m_arg[0] = literal & 0xffff;
    m_arg[1] = literal >> 16;
    return this;
  case FMT_f51l:
    m_arg[0] = literal;
    m_arg[1] = literal >> 16;
    m_arg[2] = literal >> 32;
    m_arg[3] = literal >> 48;
    return this;
  default:
    assert(false);
  }
  not_reached();
}

bool DexInstruction::has_offset() const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f10t:
  case FMT_f20t:
  case FMT_f21t:
  case FMT_f22t:
  case FMT_f30t:
  case FMT_f31t:
    return true;
  default:
    return false;
  }
  not_reached();
}

int32_t DexInstruction::offset() const {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f10t:
    return (int32_t) signext<8>(m_opcode >> 8);
  case FMT_f20t:
  case FMT_f21t:
  case FMT_f22t:
    return (int32_t) signext<16>(m_arg[0]);
  case FMT_f30t:
  case FMT_f31t: {
    auto offset = uint32_t(m_arg[0]) | (uint32_t(m_arg[1]) << 16);
    return  (int32_t) signext<32>(offset);
  }
  default:
    assert(false);
  }
  not_reached();
}

DexInstruction* DexInstruction::set_offset(int32_t offset) {
  auto format = opcode_format(opcode());
  switch (format) {
  case FMT_f10t:
    always_assert_log((int32_t)(int8_t)(offset & 0xff) == offset,
                      "offset %d too large for %s",
                      offset,
                      SHOW(this));
    m_opcode = (m_opcode & 0xff) | ((offset & 0xff) << 8);
    return this;
  case FMT_f20t:
  case FMT_f21t:
  case FMT_f22t:
    always_assert_log((int32_t)(int16_t)(offset & 0xffff) == offset,
                      "offset %d too large for %s",
                      offset,
                      SHOW(this));
    m_arg[0] = offset;
    return this;
  case FMT_f30t:
  case FMT_f31t:
    m_arg[0] = offset;
    m_arg[1] = offset >> 16;
    return this;
  default:
    assert(false);
  }
  not_reached();
}

bool DexInstruction::has_range() const {
  auto format = opcode_format(opcode());
  if (format == FMT_f3rc || format == FMT_f5rc)
    return true;
  return false;
}

uint16_t DexInstruction::range_base() const {
  auto format = opcode_format(opcode());
  assert(format == FMT_f3rc || format == FMT_f5rc);
  if (format == FMT_f5rc)
    return m_arg[1];
  return m_arg[0];
}

uint16_t DexInstruction::range_size() const {
  auto format = opcode_format(opcode());
  assert(format == FMT_f3rc || format == FMT_f5rc);
  if (format == FMT_f5rc)
    return m_arg[0];
  return (m_opcode >> 8) & 0xff;
}

DexInstruction* DexInstruction::set_range_base(uint16_t base) {
  auto format = opcode_format(opcode());
  assert(format == FMT_f3rc || format == FMT_f5rc);
  if (format == FMT_f5rc) {
    m_arg[1] = base;
  } else {
    m_arg[0] = base;
  }
  return this;
}

DexInstruction* DexInstruction::set_range_size(uint16_t size) {
  auto format = opcode_format(opcode());
  assert(format == FMT_f3rc || format == FMT_f5rc);
  if (format == FMT_f5rc) {
    m_arg[0] = size;
  } else {
    assert(size == (size & 0xff));
    m_opcode = (m_opcode & 0xff) | (size << 8);
  }
  return this;
}

bool DexInstruction::has_arg_word_count() const {
  auto format = opcode_format(opcode());
  if(format == FMT_f35c || format == FMT_f57c)
    return true;
  return false;
}

uint16_t DexInstruction::arg_word_count() const {
  auto format = opcode_format(opcode());
  assert(format == FMT_f35c || format == FMT_f57c);
  if (format == FMT_f57c) {
    return (m_arg[0]) & 0xf;
  }
  return (m_opcode >> 12) & 0xf;
}

DexInstruction* DexInstruction::set_arg_word_count(uint16_t count) {
  auto format = opcode_format(opcode());
  assert(format == FMT_f35c || format == FMT_f57c);
  assert((count & 0xf) == count);
  if (format == FMT_f57c) {
    m_arg[0] = (m_arg[0] & 0xfff0) | count;
  } else {
    m_opcode = (m_opcode & 0x0fff) | (count << 12);
  }
  return this;
}

void DexInstruction::verify_encoding() const {
  auto test = m_count ? new DexInstruction(opcode()) : new DexInstruction(opcode(), 0);
  if (dests_size()) {
    test->set_dest(dest());
  }
  for (unsigned i = 0; i < srcs_size(); i++) {
    test->set_src(i, src(i));
  }
  if (has_range()) {
    test->set_range_base(range_base());
    test->set_range_size(range_size());
  }
  if (has_arg_word_count()) test->set_arg_word_count(arg_word_count());
  if (has_literal()) test->set_literal(literal());
  if (has_offset()) test->set_offset(offset());

  assert_log(m_opcode == test->m_opcode, "%x %x\n", m_opcode, test->m_opcode);
  for (unsigned i = 0; i < m_count; i++) {
    assert_log(m_arg[i] == test->m_arg[i],
               "(%x %x) (%x %x)",
               m_opcode,
               m_arg[i],
               test->m_opcode,
               test->m_arg[i]);
  }

  delete test;
}

void DexOpcodeString::gather_strings(std::vector<DexString*>& lstring) {
  lstring.push_back(m_string);
}

uint16_t DexOpcodeString::size() const { return jumbo() ? 3 : 2; }

void DexOpcodeString::encode(DexOutputIdx* dodx, uint16_t*& insns) {
  encode_opcode(dodx, insns);
  uint32_t sidx = dodx->stringidx(m_string);
  uint16_t idx = (uint16_t)sidx;
  if (!jumbo()) {
    always_assert_log(sidx == idx,
                      "Attempt to encode jumbo string in non-jumbo opcode: %s",
                      m_string->c_str());
    *insns++ = idx;
    return;
  }
  if (sidx == idx) {
    opt_warn(NON_JUMBO_STRING, "%s\n", m_string->c_str());
  }
  *insns++ = idx;
  idx = sidx >> 16;
  *insns++ = idx;
}

uint16_t DexOpcodeType::size() const { return m_count + 2; }

void DexOpcodeType::gather_types(std::vector<DexType*>& ltype) {
  ltype.push_back(m_type);
}

void DexOpcodeType::encode(DexOutputIdx* dodx, uint16_t*& insns) {
  encode_opcode(dodx, insns);
  uint16_t idx = dodx->typeidx(m_type);
  *insns++ = idx;
  encode_args(insns);
}

void DexOpcodeField::gather_fields(std::vector<DexField*>& lfield) {
  lfield.push_back(m_field);
}

uint16_t DexOpcodeField::size() const { return 2; }

void DexOpcodeField::encode(DexOutputIdx* dodx, uint16_t*& insns) {
  encode_opcode(dodx, insns);
  uint16_t idx = dodx->fieldidx(m_field);
  *insns++ = idx;
}

void DexOpcodeMethod::gather_methods(std::vector<DexMethod*>& lmethod) {
  lmethod.push_back(m_method);
}

uint16_t DexOpcodeMethod::size() const { return 3; }

void DexOpcodeMethod::encode(DexOutputIdx* dodx, uint16_t*& insns) {
  encode_opcode(dodx, insns);
  uint16_t idx = dodx->methodidx(m_method);
  *insns++ = idx;
  encode_args(insns);
}

uint16_t DexOpcodeData::size() const { return m_data_count + 1; }

void DexOpcodeData::encode(DexOutputIdx* dodx, uint16_t*& insns) {
  encode_opcode(dodx, insns);
  memcpy(insns, m_data, m_data_count * sizeof(uint16_t));
  insns += m_data_count;
}

void DexInstruction::encode(DexOutputIdx* dodx, uint16_t*& insns) {
  encode_opcode(dodx, insns);
  encode_args(insns);
}

uint16_t DexInstruction::size() const { return m_count + 1; }

DexInstruction* DexInstruction::make_instruction(DexIdx* idx, const uint16_t*& insns) {
  uint16_t fopcode = *insns++;
  uint8_t opcode = (fopcode & 0xff);
  switch (opcode) {
  case OPCODE_NOP: {
    if (fopcode == FOPCODE_PACKED_SWITCH) {
      int count = (*insns--) * 2 + 4;
      insns += count;
      return new DexOpcodeData(insns - count, count - 1);
    } else if (fopcode == FOPCODE_SPARSE_SWITCH) {
      int count = (*insns--) * 4 + 2;
      insns += count;
      return new DexOpcodeData(insns - count, count - 1);
    } else if (fopcode == FOPCODE_FILLED_ARRAY) {
      uint16_t ewidth = *insns++;
      uint32_t size = *((uint32_t*)insns);
      int count = (ewidth * size + 1) / 2 + 4;
      insns += count - 2;
      return new DexOpcodeData(insns - count, count - 1);
    }
  }
  /* Format 10, fall through for NOP */
  case OPCODE_MOVE:
  case OPCODE_MOVE_WIDE:
  case OPCODE_MOVE_OBJECT:
  case OPCODE_MOVE_RESULT:
  case OPCODE_MOVE_RESULT_WIDE:
  case OPCODE_MOVE_RESULT_OBJECT:
  case OPCODE_MOVE_EXCEPTION:
  case OPCODE_RETURN_VOID:
  case OPCODE_RETURN:
  case OPCODE_RETURN_WIDE:
  case OPCODE_RETURN_OBJECT:
  case OPCODE_CONST_4:
  case OPCODE_MONITOR_ENTER:
  case OPCODE_MONITOR_EXIT:
  case OPCODE_THROW:
  case OPCODE_GOTO:
  case OPCODE_NEG_INT:
  case OPCODE_NOT_INT:
  case OPCODE_NEG_LONG:
  case OPCODE_NOT_LONG:
  case OPCODE_NEG_FLOAT:
  case OPCODE_NEG_DOUBLE:
  case OPCODE_INT_TO_LONG:
  case OPCODE_INT_TO_FLOAT:
  case OPCODE_INT_TO_DOUBLE:
  case OPCODE_LONG_TO_INT:
  case OPCODE_LONG_TO_FLOAT:
  case OPCODE_LONG_TO_DOUBLE:
  case OPCODE_FLOAT_TO_INT:
  case OPCODE_FLOAT_TO_LONG:
  case OPCODE_FLOAT_TO_DOUBLE:
  case OPCODE_DOUBLE_TO_INT:
  case OPCODE_DOUBLE_TO_LONG:
  case OPCODE_DOUBLE_TO_FLOAT:
  case OPCODE_INT_TO_BYTE:
  case OPCODE_INT_TO_CHAR:
  case OPCODE_INT_TO_SHORT:
  case OPCODE_ADD_INT_2ADDR:
  case OPCODE_SUB_INT_2ADDR:
  case OPCODE_MUL_INT_2ADDR:
  case OPCODE_DIV_INT_2ADDR:
  case OPCODE_REM_INT_2ADDR:
  case OPCODE_AND_INT_2ADDR:
  case OPCODE_OR_INT_2ADDR:
  case OPCODE_XOR_INT_2ADDR:
  case OPCODE_SHL_INT_2ADDR:
  case OPCODE_SHR_INT_2ADDR:
  case OPCODE_USHR_INT_2ADDR:
  case OPCODE_ADD_LONG_2ADDR:
  case OPCODE_SUB_LONG_2ADDR:
  case OPCODE_MUL_LONG_2ADDR:
  case OPCODE_DIV_LONG_2ADDR:
  case OPCODE_REM_LONG_2ADDR:
  case OPCODE_AND_LONG_2ADDR:
  case OPCODE_OR_LONG_2ADDR:
  case OPCODE_XOR_LONG_2ADDR:
  case OPCODE_SHL_LONG_2ADDR:
  case OPCODE_SHR_LONG_2ADDR:
  case OPCODE_USHR_LONG_2ADDR:
  case OPCODE_ADD_FLOAT_2ADDR:
  case OPCODE_SUB_FLOAT_2ADDR:
  case OPCODE_MUL_FLOAT_2ADDR:
  case OPCODE_DIV_FLOAT_2ADDR:
  case OPCODE_REM_FLOAT_2ADDR:
  case OPCODE_ADD_DOUBLE_2ADDR:
  case OPCODE_SUB_DOUBLE_2ADDR:
  case OPCODE_MUL_DOUBLE_2ADDR:
  case OPCODE_DIV_DOUBLE_2ADDR:
  case OPCODE_REM_DOUBLE_2ADDR:
  case OPCODE_ARRAY_LENGTH:
    return new DexInstruction(fopcode);
  /* Format 20 */
  case OPCODE_MOVE_FROM16:
  case OPCODE_MOVE_WIDE_FROM16:
  case OPCODE_MOVE_OBJECT_FROM16:
  case OPCODE_CONST_16:
  case OPCODE_CONST_HIGH16:
  case OPCODE_CONST_WIDE_16:
  case OPCODE_CONST_WIDE_HIGH16:
  case OPCODE_GOTO_16:
  case OPCODE_CMPL_FLOAT:
  case OPCODE_CMPG_FLOAT:
  case OPCODE_CMPL_DOUBLE:
  case OPCODE_CMPG_DOUBLE:
  case OPCODE_CMP_LONG:
  case OPCODE_IF_EQ:
  case OPCODE_IF_NE:
  case OPCODE_IF_LT:
  case OPCODE_IF_GE:
  case OPCODE_IF_GT:
  case OPCODE_IF_LE:
  case OPCODE_IF_EQZ:
  case OPCODE_IF_NEZ:
  case OPCODE_IF_LTZ:
  case OPCODE_IF_GEZ:
  case OPCODE_IF_GTZ:
  case OPCODE_IF_LEZ:
  case OPCODE_AGET:
  case OPCODE_AGET_WIDE:
  case OPCODE_AGET_OBJECT:
  case OPCODE_AGET_BOOLEAN:
  case OPCODE_AGET_BYTE:
  case OPCODE_AGET_CHAR:
  case OPCODE_AGET_SHORT:
  case OPCODE_APUT:
  case OPCODE_APUT_WIDE:
  case OPCODE_APUT_OBJECT:
  case OPCODE_APUT_BOOLEAN:
  case OPCODE_APUT_BYTE:
  case OPCODE_APUT_CHAR:
  case OPCODE_APUT_SHORT:
  case OPCODE_ADD_INT:
  case OPCODE_SUB_INT:
  case OPCODE_MUL_INT:
  case OPCODE_DIV_INT:
  case OPCODE_REM_INT:
  case OPCODE_AND_INT:
  case OPCODE_OR_INT:
  case OPCODE_XOR_INT:
  case OPCODE_SHL_INT:
  case OPCODE_SHR_INT:
  case OPCODE_USHR_INT:
  case OPCODE_ADD_LONG:
  case OPCODE_SUB_LONG:
  case OPCODE_MUL_LONG:
  case OPCODE_DIV_LONG:
  case OPCODE_REM_LONG:
  case OPCODE_AND_LONG:
  case OPCODE_OR_LONG:
  case OPCODE_XOR_LONG:
  case OPCODE_SHL_LONG:
  case OPCODE_SHR_LONG:
  case OPCODE_USHR_LONG:
  case OPCODE_ADD_FLOAT:
  case OPCODE_SUB_FLOAT:
  case OPCODE_MUL_FLOAT:
  case OPCODE_DIV_FLOAT:
  case OPCODE_REM_FLOAT:
  case OPCODE_ADD_DOUBLE:
  case OPCODE_SUB_DOUBLE:
  case OPCODE_MUL_DOUBLE:
  case OPCODE_DIV_DOUBLE:
  case OPCODE_REM_DOUBLE:
  case OPCODE_ADD_INT_LIT16:
  case OPCODE_RSUB_INT:
  case OPCODE_MUL_INT_LIT16:
  case OPCODE_DIV_INT_LIT16:
  case OPCODE_REM_INT_LIT16:
  case OPCODE_AND_INT_LIT16:
  case OPCODE_OR_INT_LIT16:
  case OPCODE_XOR_INT_LIT16:
  case OPCODE_ADD_INT_LIT8:
  case OPCODE_RSUB_INT_LIT8:
  case OPCODE_MUL_INT_LIT8:
  case OPCODE_DIV_INT_LIT8:
  case OPCODE_REM_INT_LIT8:
  case OPCODE_AND_INT_LIT8:
  case OPCODE_OR_INT_LIT8:
  case OPCODE_XOR_INT_LIT8:
  case OPCODE_SHL_INT_LIT8:
  case OPCODE_SHR_INT_LIT8:
  case OPCODE_USHR_INT_LIT8: {
    uint16_t arg = *insns++;
    return new DexInstruction(fopcode, arg);
  }

  /* Format 30 */
  case OPCODE_MOVE_16:
  case OPCODE_MOVE_WIDE_16:
  case OPCODE_MOVE_OBJECT_16:
  case OPCODE_CONST:
  case OPCODE_CONST_WIDE_32:
  case OPCODE_FILL_ARRAY_DATA:
  case OPCODE_GOTO_32:
  case OPCODE_PACKED_SWITCH:
  case OPCODE_SPARSE_SWITCH: {
    insns += 2;
    return new DexInstruction(insns - 3, 2);
  }
  /* Format 50 */
  case OPCODE_CONST_WIDE: {
    insns += 4;
    return new DexInstruction(insns - 5, 4);
  }
  /* Field ref: */
  case OPCODE_IGET:
  case OPCODE_IGET_WIDE:
  case OPCODE_IGET_OBJECT:
  case OPCODE_IGET_BOOLEAN:
  case OPCODE_IGET_BYTE:
  case OPCODE_IGET_CHAR:
  case OPCODE_IGET_SHORT:
  case OPCODE_IPUT:
  case OPCODE_IPUT_WIDE:
  case OPCODE_IPUT_OBJECT:
  case OPCODE_IPUT_BOOLEAN:
  case OPCODE_IPUT_BYTE:
  case OPCODE_IPUT_CHAR:
  case OPCODE_IPUT_SHORT:
  case OPCODE_SGET:
  case OPCODE_SGET_WIDE:
  case OPCODE_SGET_OBJECT:
  case OPCODE_SGET_BOOLEAN:
  case OPCODE_SGET_BYTE:
  case OPCODE_SGET_CHAR:
  case OPCODE_SGET_SHORT:
  case OPCODE_SPUT:
  case OPCODE_SPUT_WIDE:
  case OPCODE_SPUT_OBJECT:
  case OPCODE_SPUT_BOOLEAN:
  case OPCODE_SPUT_BYTE:
  case OPCODE_SPUT_CHAR:
  case OPCODE_SPUT_SHORT: {
    uint16_t fidx = *insns++;
    DexField* field = idx->get_fieldidx(fidx);
    return new DexOpcodeField(fopcode, field);
  }
  /* MethodRef: */
  case OPCODE_INVOKE_VIRTUAL:
  case OPCODE_INVOKE_SUPER:
  case OPCODE_INVOKE_DIRECT:
  case OPCODE_INVOKE_STATIC:
  case OPCODE_INVOKE_INTERFACE:
  case OPCODE_INVOKE_VIRTUAL_RANGE:
  case OPCODE_INVOKE_SUPER_RANGE:
  case OPCODE_INVOKE_DIRECT_RANGE:
  case OPCODE_INVOKE_STATIC_RANGE:
  case OPCODE_INVOKE_INTERFACE_RANGE: {
    uint16_t midx = *insns++;
    uint16_t arg = *insns++;
    DexMethod* meth = idx->get_methodidx(midx);
    return new DexOpcodeMethod(fopcode, meth, arg);
  }
  /* StringRef: */
  case OPCODE_CONST_STRING: {
    uint16_t sidx = *insns++;
    DexString* str = idx->get_stringidx(sidx);
    return new DexOpcodeString(fopcode, str);
  }
  case OPCODE_CONST_STRING_JUMBO: {
    uint32_t sidx = *insns++;
    sidx |= (*insns++) << 16;
    DexString* str = idx->get_stringidx(sidx);
    return new DexOpcodeString(fopcode, str);
  }
  case OPCODE_CONST_CLASS:
  case OPCODE_CHECK_CAST:
  case OPCODE_INSTANCE_OF:
  case OPCODE_NEW_INSTANCE:
  case OPCODE_NEW_ARRAY: {
    uint16_t tidx = *insns++;
    DexType* type = idx->get_typeidx(tidx);
    return new DexOpcodeType(fopcode, type);
  }
  case OPCODE_FILLED_NEW_ARRAY:
  case OPCODE_FILLED_NEW_ARRAY_RANGE: {
    uint16_t tidx = *insns++;
    uint16_t arg = *insns++;
    DexType* type = idx->get_typeidx(tidx);
    return new DexOpcodeType(fopcode, type, arg);
  }
  default:
    fprintf(stderr, "Unknown opcode %02x\n", opcode);
    return nullptr;
  }
}
