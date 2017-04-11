#include "pool.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

Pool::Pool() {
  _chain = new PoolChainNode(-1,-1); // dummy head
}

Pool::~Pool() {
  PoolChainNode *p = _chain, *t;
  while (p->next) {
    t = p->next;
    p->next = p->next->next;
    delete t;
  }
  delete p;
  delete [] _pool;
}

void Pool::create(int size) {
  if (_pool) throw std::runtime_error("A pool has been created.");
  
  int* temp;
  try {
    temp = new int[size];
  } catch (...) {
    std::cout << "Cannot create the pool." << std::endl;
  }
  
  _poolLength = size;
  _pool = temp;
}

void Pool::destroy() {
  if (!_pool) return;
  delete [] _pool;
  _pool = nullptr;
  
  PoolChainNode *p = _chain, *t;
  while (p->next) {
    t = p->next;
    p->next = p->next->next;
    delete t;
  }
}

int* Pool::poolFront() {
  return _pool;
}

int* Pool::alloc(int size) {
  if (size <= 0) throw std::invalid_argument("Cannot allocate.");
  
  PoolChainNode *current = _chain;
  while (current->next)  {
    if (current->next->begin - current->end - 1 >= size) {
      PoolChainNode *newNode = new PoolChainNode(current->end + 1,
                                                 current->end + size);
      newNode->next = current->next;
      current->next = newNode;
      return _pool + newNode->begin;
    }
    current = current->next;
  }
  if (_poolLength - current->end - 1 >= size) {
    PoolChainNode *newNode = new PoolChainNode(current->end + 1,
                                               current->end + size);
    current->next = newNode;
    return _pool + current->end + 1;
  } else {
    return nullptr;
  }
}

int* Pool::realloc(int *existing, int newSize) {
  if (existing < _pool || existing >= _pool + _poolLength)
    throw std::invalid_argument("Not a valid existing pointer.");
  
  int begin = existing - _pool;
  
  PoolChainNode *current = _chain;
  while (current->next && current->next->begin < begin) {
    current = current->next;
  }
  
  if (!current->next || current->next->begin > begin)
    throw std::invalid_argument("Not a valid existing pointer.");
  
  PoolChainNode *prev = current;
  current = current->next;
  
  // Check if can avoid moving
  int oldSize = current->end - current->begin + 1;
  if (oldSize >= newSize) {
    return _pool + current->begin;
  } else if ((current->next == nullptr && current->begin + newSize <= _poolLength) ||
             (current->next && current->next->begin - current->begin >= newSize)) {
    current->end = current->begin + newSize - 1;
    return _pool + current->begin;
  }
  
  // Check if space available after existing block
  PoolChainNode *search = current->next;
  while (search) {
    if ((!search->next && _poolLength - search->end - 1 >= newSize) ||
        (search->next && search->next->begin - search->end - 1 >= newSize)) {
      PoolChainNode *newNode = new PoolChainNode(search->end + 1,
                                                 search->end + newSize);
      newNode->next = search->next;
      search->next = newNode;
      std::memcpy(&_pool + newNode->begin,
                  &_pool + current->begin,
                  oldSize);
      prev->next = current->next;
      delete current;
      return _pool + newNode->begin;
    }
    search = search->next;
  }
  
  // Check if space available before existing block
  search = _chain;
  while (search && search != current) {
    if ((!search->next && _poolLength - search->end - 1 >= newSize) ||
        (search->next && search->next->begin - search->end - 1 >= newSize)) {
      PoolChainNode *newNode = new PoolChainNode(search->end + 1,
                                                 search->end + newSize);
      newNode->next = search->next;
      search->next = newNode;
      std::memcpy(&_pool + newNode->begin,
                  &_pool + current->begin,
                  oldSize);
      prev->next = current->next;
      delete current;
      return _pool + newNode->begin;
    }
    search = search->next;
  }
  
  return NULL;
}

void Pool::free(int *existing) {
  int begin = existing - _pool;
  
  PoolChainNode *current = _chain;
  while (current->next && current->next->begin < begin) {
    current = current->next;
  }
  
  if (!current->next || current->next->begin > begin) {
    throw std::invalid_argument("Not a valid existing pointer.");
  }
  
  PoolChainNode *toDelete = current->next;
  current->next = current->next->next;
  delete toDelete;
}

std::ostream &operator<<(std::ostream &out, const Pool& pool) {
  if (pool._pool == nullptr) {
    out << "Pool not initialized.";
    return out;
  }
  
  std::ostringstream oss;
  
  out << "Pool size: " << pool._poolLength << std::endl;
  
  Pool::PoolChainNode *current = pool._chain;
  while (current->next) {
    if (current->begin != -1) {
      for (int i = current->begin; i <= current->end; ++i) {
        oss << '*';
      }
    }
    
    for (int i = current->end + 1; i < current->next->begin; ++i) {
      oss << '_';
    }
    current = current->next;
  }
  
  if (current->begin != -1) {
    for (int i = current->begin; i <= current->end; ++i) {
      oss << '*';
    }
  }
  
  for (int i = current->end + 1; i < pool._poolLength; ++i) {
    oss << '_';
  }
  
  out << oss.str();
  return out;
}

