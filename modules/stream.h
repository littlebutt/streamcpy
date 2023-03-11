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

static PyObject*
Stream_for_each(Stream* self, PyObject* args, PyObject* kwargs)
{
    static char* kwlist[] = {"op_method", NULL};
    PyObject* op_method;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &op_method))
    {
        return NULL;
    }
    Py_INCREF(op_method);
    Pipeline_append(self->head, OP_TYPE_FOR_EACH, op_method);
    return Pipeline_execute(self->head, (PyListObject*)self->spliterator);
}

static PyMemberDef Stream_members[] = {
    {"spliterator", T_OBJECT, offsetof(Stream, spliterator), 0, PyDoc_STR("a value for storing iterable data")},
    {"head", T_OBJECT, offsetof(Stream, head), 0, PyDoc_STR("a head pointer pointing to a Pipeline")},
    {NULL}
};

static PyMethodDef Stream_methods[] = {
    {"of", (PyCFunction)Stream_of, METH_STATIC, PyDoc_STR("The initializing method")},
    {"map", (PyCFunction)Stream_map, 0, PyDoc_STR("The mapping method")},
    {"for_each", (PyCFunction)Stream_for_each, 0, PyDoc_STR("The for each method")},
    {NULL, NULL}
};

PyDoc_STRVAR(doc_stream,
"Stream\n-\n\n\
A tool for accelerating python calculation.\n");

static PyTypeObject Stream_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "streampy.Stream",                      /* tp_name */
    sizeof(Stream),                         /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor) Stream_dealloc,            /* tp_dealloc */
    0,                                      /* tp_vectorcall_offset */
    (getattrfunc) 0,                        /* tp_getattr */
    (setattrfunc) 0,                        /* tp_setattr */
    0,                                      /* tp_as_async */
    (reprfunc) Stream_repr,                 /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    (hashfunc) 0,                           /* tp_hash */
    0,                                      /* tp_call */
    (reprfunc) Stream_repr,                 /* tp_str */
    (getattrofunc) PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc) PyObject_GenericSetAttr, /* tp_setattro */
    (PyBufferProcs *) 0,                    /* tp_as_buffer */
    (Py_TPFLAGS_DEFAULT|
     Py_TPFLAGS_BASETYPE),                  /* tp_flags */
    doc_stream,                             /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    Stream_methods,                         /* tp_methods */
    Stream_members,                         /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    Stream_new,                             /* tp_new */
    PyObject_Del,                           /* tp_free */
}

#ifdef __cplusplus
}
#endif

#endif // STREAM_H_