#ifndef OP_TYPES_H
#define OP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define OP_TYPE_SKIP 0
#define OP_TYPE_MAP 1
#define OP_TYPE_FOR_EACH 2
#define OP_TYPE_FILTER 3
#define OP_TYPE_COLLECT 4
#define OP_TYPE_DISTINCT 5
#define OP_TYPE_LIMIT 6
#define OP_TYPE_REDUCE 7
#define OP_TYPE_SORTED 8
#define OP_TYPE_MAX 9
#define OP_TYPE_MIN 10
#define OP_TYPE_COUNT 11
#define OP_TYPE_ANY_MATCH 12
#define OP_TYPE_ALL_MATCH 13


#ifdef __cplusplus
}
#endif

#endif // OP_TYPES_H