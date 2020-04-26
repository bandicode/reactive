// Copyright (C) 2020 Vincent Chambrin
// This file is part of the reactive library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "reactive/reactive.h"

namespace react
{

Engine& engine()
{
  static Engine e = {};
  return e;
}

} // namespace react
