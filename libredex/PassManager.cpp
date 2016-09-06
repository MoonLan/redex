/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include "PassManager.h"

#include <cstdio>
#include <chrono>

#include "Debug.h"
#include "DexClass.h"
#include "DexLoader.h"
#include "DexOutput.h"
#include "DexUtil.h"
#include "ConfigFiles.h"
#include "ReachableClasses.h"
#include "Transform.h"

PassManager::PassManager(
    const std::vector<Pass*>& passes,
    const std::vector<KeepRule>& rules,
    const Json::Value& config)
  : m_config(config),
    m_registered_passes(passes),
    m_proguard_rules(rules),
    m_current_pass_metrics(nullptr) {
  if (config["redex"].isMember("passes")) {
    auto passes = config["redex"]["passes"];
    for (auto& pass : passes) {
      activate_pass(pass.asString().c_str(), config);
    }
  } else {
    // If config isn't set up, run all registered passes.
    m_activated_passes = m_registered_passes;
  }
}

void PassManager::run_passes(DexStoresVector& stores, ConfigFiles& cfg) {
  DexStoreClassesIterator it(stores);
  Scope scope = build_class_scope(it);
  init_reachable_classes(scope, m_config,
      m_proguard_rules, cfg.get_no_optimizations_annos());
  for (auto pass : m_activated_passes) {
    using namespace std::chrono;
    TRACE(PM, 1, "Running %s...\n", pass->name().c_str());
    m_current_pass_metrics = &m_pass_metrics[pass->name()];
    auto start = high_resolution_clock::now();
    if (pass->assumes_sync()) {
      MethodTransform::sync_all();
    }
    pass->run_pass(stores, cfg, *this);
    auto end = high_resolution_clock::now();
    TRACE(PM, 1, "Pass %s completed in %.1lf seconds\n",
          pass->name().c_str(), duration<double>(end - start).count());
    m_current_pass_metrics = nullptr;
  }

  MethodTransform::sync_all();
}

void PassManager::activate_pass(const char* name, const Json::Value& cfg) {
  for (auto pass : m_registered_passes) {
    if (name == pass->name()) {
      m_activated_passes.push_back(pass);
      pass->configure_pass(PassConfig(cfg[pass->name()]));
      return;
    }
  }
  always_assert_log(false, "No pass named %s!", name);
}

void PassManager::incr_metric(const std::string& key, int value) {
  always_assert_log(m_current_pass_metrics != nullptr, "No current pass!");
  (*m_current_pass_metrics)[key] += value;
}

std::map<std::string, std::map<std::string, int>>
PassManager::get_metrics() const {
  return m_pass_metrics;
}
