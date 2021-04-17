/**
 * Simple re-implementation of some LSM stuff in C to use in the compute process
 */

#ifndef __LSM_H__
#define __LSM_H__

#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

extern const int TOMBSTONE;
extern const int V_TOMBSTONE;

typedef struct KVPair {
  int32_t key;
  int32_t value;
} KVPair_t;

typedef struct KVIntPair {
  struct KVPair first;
  int second;
} KVIntPair_t;

bool isEqual(struct KVPair K1, struct KVPair K2);
bool isNotEqual(struct KVPair K1, struct KVPair K2);
bool lessThan(struct KVPair K1, struct KVPair K2);
bool greaterThan(struct KVPair K1, struct KVPair K2);
bool isNotEqualPair(KVIntPair_t K1, KVIntPair_t K2);
bool lessThanPair(KVIntPair_t K1, KVIntPair_t K2);

/******************************************************************************/

extern const KVPair_t KVPAIRMAX;
extern const KVIntPair_t KVINTPAIRMAX;

typedef struct StaticHeap {
  int size;
  KVIntPair_t *arr;
  KVIntPair_t max;
  int i;
} StaticHeap;

void initStaticHeap(StaticHeap *H, unsigned sz, KVIntPair_t mx);
void push(StaticHeap *H, KVIntPair_t blob);
void heapify(StaticHeap *H, int i);
KVIntPair_t pop(StaticHeap *H);

#endif // __LSM_H__
