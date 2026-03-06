#pragma once
#include <cassert>
#ifndef PROGRAMLOADER_H
#define PROGRAMLOADER_H

#include <string>
#include <vector>
#include <fstream>

#include "common.hpp"
#include "Memory.hpp"

struct ProgramLoader {
  static void loadProgram(const std::string& binaryPath, Memory& memory, u32 entryPoint) {
    std::ifstream file(binaryPath, std::ios::binary);
    assert(file.is_open() && "Failed to open binary file");

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    assert(size > 0 && "Binary file is empty");

    std::vector<u8> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
      assert(false && "Failed to read binary file");
    }

    int padding = (4 - (buffer.size() % 4)) % 4;
    buffer.insert(buffer.end(), padding, 0);

    u32 addr = entryPoint;
    for (size_t i = 0; i < buffer.size(); i += 4) {
      u32 word = static_cast<u32>(buffer[i]) |
                  (static_cast<u32>(buffer[i + 1]) << 8) |
                  (static_cast<u32>(buffer[i + 2]) << 16) |
                  (static_cast<u32>(buffer[i + 3]) << 24);
      memory.write(addr, word);
      addr += 4;
    }
  } 
};

#endif
