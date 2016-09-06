/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <list>
#include <vector>

class DexString;
class DexType;
class DexField;
class DexMethod;

class Gatherable {
 protected:
  Gatherable() {}

  virtual ~Gatherable() {};

 public:
  virtual void gather_strings(std::vector<DexString*>& lstring) {}
  virtual void gather_types(std::vector<DexType*>& ltype) {}
  virtual void gather_fields(std::vector<DexField*>& lfield) {}
  virtual void gather_methods(std::vector<DexMethod*>& lmethod) {}
};
