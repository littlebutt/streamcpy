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

static PyObject*
Stream_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Stream* self;
    self = (Stream*)type->tp_alloc(type, 0);
    if (self)
    {
        Pipeline* head = (Pipeline*)PyObject_New(Pipeline, Pipeline_type);
        if (!head)
        {
            return NULL;
        }
        head->op_type = OP_TYPE_SKIP;
        head->op_method = Py_None;
        head->next = Py_None;
        self->head = head;

        self->spliterator = Py_None;
        return (PyObject*)self;
    }
    return NULL;
}

// static method
static PyObject*
Stream_of(PyObject* self, PyObject* args)
{
    Stream* st = (Stream*)PyObject_New(Stream, Stream_type);
    if (!st)
    {
        return NULL;
    }

    PyObject* list_arg;
    if (!PyArg_ParseTuple(args, "O!", PyList_Type, &list_arg))
    {
        return NULL;
    }
    Py_INCREF(list_arg);
    st->spliterator = list_arg;
    return (PyObject*)st;
}

static PyObject*
Stream_map(Stream* self, PyObject* args, PyObject* kwargs)
{
    static char* kwlist[] = {"op_method", NULL};
    PyObject* op_method;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &op_method))
    {
        return NULL;
    }
    Py_INCREF(op_method);
    Pipeline_append(self->head, OP_TYPE_MAP, op_method);
    return (PyObject*)self;
}

#ifdef __cplusplus
}
#endif

#endif // STREAM_H_