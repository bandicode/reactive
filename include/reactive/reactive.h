// Copyright (C) 2020 Vincent Chambrin
// This file is part of the reactive library
// For conditions of distribution and use, see copyright notice in LICENSE

#ifndef REACTIVE_REACTIVE_H
#define REACTIVE_REACTIVE_H

#include <functional>
#include <unordered_map>
#include <vector>

#if (defined(WIN32) || defined(_WIN32)) && !defined(REACTIVE_STATIC_LINKING)
#if defined(REACTIVE_COMPILE_LIBRARY)
#  define REACTIVE_API __declspec(dllexport)
#else
#  define REACTIVE_API __declspec(dllimport)
#endif
#else
#define REACTIVE_API
#endif

namespace react
{

class REACTIVE_API Engine
{
public:
  bool evaluating = false;
  std::vector<void*> dependencies;
  std::vector<std::function<void(void)>> callbacks;
  std::unordered_map<void*, size_t> bindings;
  std::unordered_map<void*, std::vector<size_t>> subscribers;
};

REACTIVE_API Engine& engine();

template<typename T>
T& dep(T& x)
{
  Engine& e = engine();
  if (!e.evaluating)
    e.dependencies.push_back(reinterpret_cast<void*>(&x));
  return x;
}

template<typename T, typename F>
void bind(T& x, F&& func)
{
  Engine& e = engine();

  x = func();

  e.callbacks.push_back([&x, func]() {
    x = func();
    });

  for (void* d : e.dependencies)
  {
    e.subscribers[d].push_back(e.callbacks.size() - 1);
  }

  e.bindings[reinterpret_cast<void*>(&x)] = e.callbacks.size() - 1;

  e.dependencies.clear();
}

template<typename T, typename F>
void hook(T& x, F&& func)
{
  Engine& e = engine();

  e.callbacks.push_back(std::forward<F>(func));
  e.subscribers[reinterpret_cast<void*>(&x)].push_back(e.callbacks.size() - 1);

  e.dependencies.clear();
}

template<typename T>
void changed(T& x)
{
  Engine& e = engine();

  auto it = e.subscribers.find(reinterpret_cast<void*>(&x));

  if (it != e.subscribers.end())
  {
    e.evaluating = true;

    for (size_t index : it->second)
    {
      e.callbacks[index]();
    }

    e.evaluating = false;
  }
}

template<typename T>
void destroyed(T& x)
{
  Engine& e = engine();

  auto it = e.subscribers.find(reinterpret_cast<void*>(&x));

  if (it != e.subscribers.end())
  {
    for (size_t index : it->second)
    {
      e.callbacks[index] = []() {};
    }

    e.subscribers.erase(it);
  }
}

template<typename T>
class property
{
private:
  T m_value;

public:
  property(T v = {})
    : m_value(v)
  {
    
  }

  ~property()
  {
    react::destroyed(m_value);
  }

  const T& get() const
  {
    return m_value;
  }

  operator const T& () const
  {
    react::dep(const_cast<T&>(m_value));
    return m_value;
  }

  property<T>& operator=(const T& val)
  {
    m_value = val;
    react::changed(m_value);
    return *this;
  }

  property<T>& operator=(std::function<T(void)> expr)
  {
    react::bind(m_value, expr);
    return *this;
  }
};

} // namespace react

#endif // REACTIVE_REACTIVE_H
