#pragma once
#ifndef DEVICE_H
#define DEVICE_H

#include "common.hpp"

struct Device {
  virtual u32 read(u32 addr) = 0;
  virtual void write(u32 addr, u32 data) = 0;
};

#endif
