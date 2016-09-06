/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <algorithm>
#include <gtest/gtest.h>

#include "VerifyUtil.h"

DexClass* find_class_named(const DexClasses& classes, const char* name) {
  auto it = std::find_if(classes.begin(), classes.end(), [&name](DexClass* cls){
    return !strcmp(name, cls->get_name()->c_str());
  });
  return it == classes.end() ? nullptr : *it;
}

DexMethod* find_vmethod_named(const DexClass& cls, const char* name) {
  auto vmethods = cls.get_vmethods();
  auto it = std::find_if(vmethods.begin(), vmethods.end(), [&name](DexMethod* m){
    return !strcmp(name, m->get_name()->c_str());
  });
  return it == vmethods.end() ? nullptr : *it;
}

DexOpcodeMethod* find_invoke(const DexMethod* m, uint32_t opcode,
    const char* target_mname) {
  auto insns = m->get_code()->get_instructions();
  return find_invoke(insns.begin(), insns.end(), opcode, target_mname);
}

DexOpcodeMethod* find_invoke(
    std::vector<DexInstruction*>::iterator begin,
    std::vector<DexInstruction*>::iterator end,
    uint32_t opcode,
    const char* target_mname) {
  auto it = std::find_if(begin, end,
    [opcode, target_mname](DexInstruction* insn) {
      if (insn->opcode() != opcode) {
        return false;
      }
      auto mname =
        static_cast<DexOpcodeMethod*>(insn)->get_method()->get_name();
      return mname == DexString::get_string(target_mname);
    });
  return it == end ? nullptr : static_cast<DexOpcodeMethod*>(*it);
}
