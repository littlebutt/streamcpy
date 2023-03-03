#ifndef OP_TYPES_H
#define OP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

enum op_type {
    OP_TYPE_SKIP = 0,
    OP_TYPE_MAP,
    OP_TYPE_FOR_EACH, // TODO: more
};

static enum op_type get_op_type(int i) {
    return enum op_type(i);
}


#ifdef __cplusplus
}
#endif

#endif // OP_TYPES_H