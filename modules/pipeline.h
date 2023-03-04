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

static PyObject*
Pipeline_execute(Pipeline* pl, PyListObject* init_data)
{
    PyListObject* data = init_data;
    Pipeline* ptr = pl;
    while (ptr)
    {
        if (ptr->op_type == OP_TYPE_SKIP)
        {
            ptr = ptr->next;
            continue;
        }
        switch (ptr->op_type)
        {
            case OP_TYPE_MAP:
            {
                PyListObject* new_data = (PyListObject*)PyList_New(0);
                if (!new_data)
                {
                    goto FAILURE;
                }
                for (Py_ssize_t i = 0; i<PyList_Size(data); ++i)
                {
                    PyObject* res = PyObject_CallFunction(ptr->op_method, "O", PyList_GetItem(data, i));
                    if (!res)
                    {
                        goto FAILURE;
                    }
                    if(PyList_Append(new_data, res) == -1)
                    {
                        goto FAILURE;
                    }
                }
                data = new_data;
                break;
            }
            case OP_TYPE_FOR_EACH:
            {
                for (Py_ssize_t i = 0; i<PyList_Size(data); ++i)
                {
                    if(!PyObject_CallFunction(ptr->op_method, "O", PyList_GetItem(data, i)))
                    {
                        goto FAILURE;
                    }
                }
                break;
            }
            default: return Py_None;
        }
    }
    
    return Py_None;

    FAILURE:
        return Py_None;
}

#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H_