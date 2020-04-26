// Copyright (C) 2020 Vincent Chambrin
// This file is part of the reactive library
// For conditions of distribution and use, see copyright notice in LICENSE

#include "reactive/reactive.h"

#include <iostream>

int main()
{
  {
    react::property<int> a = 5;
    react::property<int> b = 2;
    react::property<int> c = 0;

    c = [&]() { return a + b; };

    std::cout << c.get() << std::endl;

    a = 10;

    std::cout << c.get() << std::endl;
  }

  {
    std::string str = "Hello";

    react::hook(str, [&]() {
      std::cout << str << std::endl;
      });
   
    str = "Hello World!";
    react::changed(str);

    react::destroyed(str);
    react::changed(str);
  }

  return 0;
}

