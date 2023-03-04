#ifndef PIPELINE_H_
#define PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "op_types.h"

typedef struct _Pipeline {

    int op_type;      // op_type

    PyObject* op_method;    // op_method

    struct _Pipeline* next; // next

} Pipeline;

static int
Pipeline_append(Pipeline* pl, const int op_type, PyObject* op_method)
{
    Pipeline* last = (Pipeline*)malloc(sizeof(Pipeline));
    if (last == NULL)
    {
        return -1;
    }
    last->next = NULL;
    last->op_type = op_type;
    last->op_method = op_method;

    Pipeline* ptr = pl;
    while (ptr->next)
    {
        ptr = ptr->next;
    }
    ptr->next = last;
    
    return 0;
}



#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H_