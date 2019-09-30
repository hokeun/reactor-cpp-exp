/*
 * Copyright (C) 2019 TU Dresden
 * All rights reserved.
 *
 * Authors:
 *   Christian Menard
 */

#pragma once

#include <set>
#include <string>

#include "dear/reactor.hh"

namespace dear {

class Environment {
 public:
  enum class Phase { Construction, Assembly };

 private:
  std::set<Reactor*> _top_level_reactors;

 public:
  Environment() = default;

  void register_reactor(Reactor* reactor);

  const auto& top_level_reactors() const { return _top_level_reactors; }

  void assemble();
  void export_dependency_graph(const std::string& path);

  Phase phase() const { return _phase; }
};

}  // namespace dear
