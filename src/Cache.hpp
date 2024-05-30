#pragma once
#ifndef CACHE_H
#define CACHE_H

#define L1_CACHE_SIZE 512*KB
#define L2_CACHE_SIZE 1*MB
#define L3_CACHE_SIZE 2*MB

#define I_CACHE_SIZE  1*KB

#define BLOCK_SIZE    64

struct ICache {
 
};

struct DCache {

};

struct Cache {
  ICache icache;
  DCache dcache;
};

#endif
