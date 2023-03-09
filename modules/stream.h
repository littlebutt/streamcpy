#ifndef STREAM_H_
#define STREAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#include "pipeline.h"

static PyTypeObject Stream_type;

typedef struct {
    PyObject_HEAD

    PyObject* spliterator;

    Pipeline* head;
} Stream;

static void
Stream_dealloc(Stream* st)
{
    Py_XDECREF(st->head);
    Py_XDECREF(st->spliterator);
    Py_TYPE(st)->tp_free(st);
}

static PyObject*
Stream_repr(Stream* st)
{
    PyObject* retval;
    PyObject* sp_repr = PyObject_Repr(st->spliterator);
    if (!sp_repr)
    {
        return NULL;
    }
    const char* sp_repr_str = PyBytes_AsString(sp_repr);
    PyObject* h_repr = PyObject_Repr(st->head);
    if (!h_repr)
    {
        return NULL;
    }
    const char* h_repr_str = PyBytes_AsString(h_repr);
    retval = PyUnicode_FromFormat("Stream(%s, %s)", sp_repr_str, h_repr_str);
    return retval;
}

#ifdef __cplusplus
}
#endif

#endif // STREAM_H_