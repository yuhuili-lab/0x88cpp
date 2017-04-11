#ifndef POOL_HPP
#define POOL_HPP

#include <iostream>

class Pool {
public:
  Pool();
  ~Pool();
  void create(int size);
  void destroy();
  int* alloc(int size);
  int* realloc(int *existing, int newSize);
  void free(int *existing);
  int* poolFront();
  
private:
  struct PoolChainNode {
    int begin;
    int end;
    PoolChainNode *next;
    PoolChainNode(int begin, int end) : begin{begin}, end{end}, next{nullptr} {}
  };
  int _poolLength;
  int* _pool = nullptr;
  PoolChainNode *_chain;
  
  friend std::ostream &operator<<(std::ostream &out, const Pool& pool);
};

std::ostream &operator<<(std::ostream &out, const Pool& pool);

#endif
