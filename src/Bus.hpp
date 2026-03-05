#pragma once
#ifndef BUS_H
#define BUS_H

#include <vector>
#include <assert.h>

#include "common.hpp"
#include "Device.hpp"

struct MemoryRegion {
  u32 base_addr;
  u32 size;
  Device & device;

  MemoryRegion(u32 base_addr, u32 size, Device & device) 
    : base_addr(base_addr), size(size), device(device) {

  }
};

struct MemoryMapper {
  std::vector<MemoryRegion> regions;

  void addRegion(u32 base_addr, u32 size, Device & device) {
    regions.emplace_back(base_addr, size, device);
  }

  Device & lookup(u32 addr) {
    for (auto & region : regions) {
      if (addr >= region.base_addr && addr < region.base_addr + region.size) {
        return region.device;
      }
    }
    assert(false && "Address not mapped to any device");
  }
};

struct BusEvent {
  enum Type { READ, WRITE, INTERRUPT } type;
  u32 addr;
  u32 data;
};

struct Bus {
  MemoryMapper mapper;

  void addDevice(u32 base_addr, u32 size, Device & device) {
    mapper.addRegion(base_addr, size, device);
  }

  u32 handleEvent(BusEvent event) {
    Device & device = mapper.lookup(event.addr);
    if (event.type == BusEvent::READ) {
      return device.read(event.addr);
    } else if (event.type == BusEvent::WRITE) {
      device.write(event.addr, event.data);
      return 0;
    } else if (event.type == BusEvent::INTERRUPT) {
      return 0;
    } else {
      assert(false && "Unknown bus event type");
    }
  }
};

#endif
