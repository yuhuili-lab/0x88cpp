#include "pool.hpp"

#include <iostream>
#include <string>
using std::cin;
using std::cout;
using std::endl;

int main() {
  Pool pool;
  std::string s;
  int i, k;
  int* poolFront = nullptr;
  
  while (cin >> s) {
    if (s == "quit") {
      break;
    } else if (s == "c" || s == "create") {
      cin >> i;
      pool.create(i);
      cout << pool << endl;
      poolFront = pool.poolFront();
    } else if (s == "d" || s == "destroy") {
      pool.destroy();
    } else if (s == "a" || s == "alloc") {
      cin >> i;
      int* t = pool.alloc(i);
      if (t == nullptr) {
        cout << "Alloc: nullptr" << endl;
      } else {
        cout << "Alloc at " << t - poolFront << endl;
      }
      cout << pool << endl;
    } else if (s == "r" || s == "realloc") {
      cin >> k >> i;
      int* t = pool.realloc(poolFront + k, i);
      if (t == nullptr) {
        cout << "Realloc: nullptr" << endl;
      } else {
        cout << "Realloc at " << t - poolFront << endl;
      }
      cout << pool << endl;
    } else if (s == "f" || s == "free") {
      cin >> k;
      pool.free(poolFront + k);
      cout << pool << endl;
    } else if (s == "p") {
      cout << pool << endl;
    }
  }
}
