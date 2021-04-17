/**
 * Simple re-implementation of some LSM stuff in C to use in the compute process
 */

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

#include "lsm.h"

const int TOMBSTONE = INT_MIN;
const int V_TOMBSTONE = INT_MIN;

bool isEqual(struct KVPair K1, struct KVPair K2) {
  return (K1.key == K2.key && K1.value == K2.value);
}

bool isNotEqual(struct KVPair K1, struct KVPair K2) {
  return (K2.key != K1.key != K2.value != K1.value);
}

bool lessThan(struct KVPair K1, struct KVPair K2) {
  return K1.key < K2.key;
}
bool greaterThan(struct KVPair K1, struct KVPair K2) {
  return K1.key > K2.key;
}

bool isNotEqualPair(KVIntPair_t K1, KVIntPair_t K2) {
  return !(isEqual(K1.first, K2.first) && (K1.second == K2.second));
}

bool lessThanPair(KVIntPair_t K1, KVIntPair_t K2) {
  return lessThan(K1.first, K2.first) ||
         (!(lessThan(K2.first, K1.first)) && K1.second < K2.second);
}

/******************************************************************************/

#define LEFTCHILD(x) 2 * x + 1
#define RIGHTCHILD(x) 2 * x + 2
#define PARENT(x) (x - 1) / 2

const KVPair_t KVPAIRMAX = {INT_MAX, 0};
const KVIntPair_t KVINTPAIRMAX = { {INT_MAX, 0}, -1 };

void initStaticHeap(StaticHeap *H, unsigned sz, KVIntPair_t mx) {
  H->size = 0;
  H->arr = (KVIntPair_t *)malloc(sz * sizeof(KVIntPair_t));
  H->max = mx;
  for (int i = 0; i < sz; i++) {
    H->arr[i] = mx;
  }
}

void push(StaticHeap *H, KVIntPair_t blob) {
  unsigned i = H->size++;
  while (i && lessThanPair(blob, H->arr[PARENT(i)])) {
    H->arr[i] = H->arr[PARENT(i)];
    i = PARENT(i);
  }
  H->arr[i] = blob;
}

void heapify(StaticHeap *H, int i) {
  int smallest =
      (LEFTCHILD(i) < H->size && lessThanPair(H->arr[LEFTCHILD(i)], H->arr[i]))
          ? LEFTCHILD(i)
          : i;
  if (RIGHTCHILD(i) < H->size &&
      lessThanPair(H->arr[RIGHTCHILD(i)], H->arr[smallest])) {
    smallest = RIGHTCHILD(i);
  }
  if (smallest != i) {
    KVIntPair_t temp = H->arr[i];
    H->arr[i] = H->arr[smallest];
    H->arr[smallest] = temp;
    heapify(H, smallest);
  }
}

KVIntPair_t pop(StaticHeap *H) {
  KVIntPair_t ret = H->arr[0];
  H->arr[0] = H->arr[--H->size];
  heapify(H, 0);
  return ret;
}

#undef LEFTCHILD
#undef RIGHTCHILD
#undef PARENT