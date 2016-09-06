/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <cstdint>
#include <initializer_list>

#include "DexInstruction.h"

/*
 * Mini-DSL for building DexInstructions quickly.
 */

namespace dex_asm {

enum OperandTag {
  VREG,
  OFFSET,
  LITERAL
};

struct Operand {
  OperandTag tag;
  uint64_t v;
};

inline Operand operator "" _v(unsigned long long v) {
  return {VREG, v};
}

inline Operand operator "" _off(unsigned long long v) {
  return {OFFSET, v};
}

inline Operand operator "" _L(unsigned long long v) {
  return {LITERAL, v};
}

DexInstruction* dasm(DexOpcode opcode, std::initializer_list<Operand> = {});

}
