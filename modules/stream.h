#ifndef STREAM_H_
#define STREAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#include "pipeline.h"

typedef struct {
    PyObject_HEAD

    PyObject* spliterator;
    
    Pipeline* head;
} Stream;

#ifdef __cplusplus
}
#endif

#endif // STREAM_H_