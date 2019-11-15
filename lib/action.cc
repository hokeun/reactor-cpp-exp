/*
 * Copyright (C) 2019 TU Dresden
 * All rights reserved.
 *
 * Authors:
 *   Christian Menard
 */

#include "reactor-cpp/action.hh"

#include "reactor-cpp/assert.hh"
#include "reactor-cpp/environment.hh"
#include "reactor-cpp/reaction.hh"

namespace reactor {

void BaseAction::register_trigger(Reaction* reaction) {
  ASSERT(reaction != nullptr);
  ASSERT(this->environment() == reaction->environment());
  ASSERT(this->environment()->phase() == Environment::Phase::Assembly);
  // the reaction must belong to the same reactor as this action
  ASSERT(this->container() == reaction->container());
  auto r = _triggers.insert(reaction);
  ASSERT(r.second);
}

void BaseAction::register_scheduler(Reaction* reaction) {
  ASSERT(reaction != nullptr);
  ASSERT(is_logical());
  ASSERT(this->environment() == reaction->environment());
  ASSERT(this->environment()->phase() == Environment::Phase::Assembly);
  // the reaction must belong to the same reactor as this action
  ASSERT(this->container() == reaction->container());
  auto r = _schedulers.insert(reaction);
  ASSERT(r.second);
}

void Action<void>::schedule(time_t delay) {
  auto scheduler = environment()->scheduler();
  auto setup = [this]() { this->present = true; };
  if (is_logical()) {
    auto tag = Tag::from_logical_time(scheduler->logical_time()).delay(delay);
    scheduler->schedule(tag, this, setup);
  } else {
    // physical action
    scheduler->lock();
    auto tag = Tag::from_physical_time(get_physical_timepoint() + delay);
    scheduler->schedule(tag, this, setup);
    scheduler->unlock();
  }
}

void Timer::startup() {
  Tag t0 = Tag::from_physical_time(environment()->start_time());
  if (_offset != 0) {
    environment()->scheduler()->schedule(t0.delay(_offset), this, nullptr);
  } else {
    environment()->scheduler()->schedule(t0, this, nullptr);
  }
}

void Timer::cleanup() {
  // schedule the timer again
  if (_period != 0) {
    Tag now = Tag::from_logical_time(environment()->logical_time());
    Tag next = now.delay(_period);
    environment()->scheduler()->schedule(next, this, nullptr);
  }
}

void ShutdownAction::shutdown() {
  Tag t = Tag::from_logical_time(environment()->logical_time()).delay();
  environment()->scheduler()->schedule(t, this, nullptr);
}

}  // namespace reactor
