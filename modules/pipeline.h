#ifndef PIPELINE_H_
#define PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "op_types.h"


static PyTypeObject Pipeline_type;

typedef struct _Pipeline {
    PyObject_HEAD

    int op_type;      // op_type

    PyObject* op_method;    // op_method

    struct _Pipeline* next; // next

} Pipeline;

static void
Pipeline_dealloc(Pipeline* pl)
{
    Py_XDECREF(pl->op_method);
    Py_XDECREF(pl->next);
    Py_TYPE(pl)->tp_free(pl);
}

static PyObject*
Pipeline_repr(Pipeline* pl)
{
    PyObject* retval;
    PyObject* op_method_repr;
    PyObject* op_method_repr = PyObject_Repr(pl->op_method);
    if (!op_method_repr)
    {
        return NULL;
    }
    const char* op_method_repr_str = PyString_AsString(res);
    retval = PyUnicode_FromFormat("Pipeline(%s, %d)", op_method_repr_str, pl->op_type);
    return retval;
}

static int
Pipeline_append(Pipeline* pl, const int op_type, PyObject* op_method)
{
    Pipeline* last = (Pipeline*)PyObject_New(Pipeline, Pipeline_type);
    if (last == NULL)
    {
        Py_XDECREF(pl);
        Py_XDECREF(op_method);
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
                PyObject* tmp = data;
                data = new_data;
                Py_XDECREF(tmp);
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
        ptr = ptr->next;
    }
    
    return Py_None;

    FAILURE:
        Py_XDECREF(pl);
        Py_XDECREF(init_data);
        return Py_None;
}

PyDoc_STRVAR(doc_pipeline,
"Pipeline\n-\n\n\
The inner implementation of the Pipeline in Stream.\n");

static PyTypeObject Pipeline_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "streampy.Pipeline",                    /* tp_name */
    sizeof(Pipeline),                       /* tp_basicsize */
    0,                                      /* tp_itemsize */
    (destructor) Pipeline_dealloc,          /* tp_dealloc */
    0,                                      /* tp_vectorcall_offset */
    (getattrfunc) 0,                        /* tp_getattr */
    (setattrfunc) 0,                        /* tp_setattr */
    0,                                      /* tp_as_async */
    (reprfunc) Pipeline_repr,               /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    (hashfunc) 0,                           /* tp_hash */
    0,                                      /* tp_call */
    (reprfunc) Pipeline_repr,               /* tp_str */
    (getattrofunc) PyObject_GenericGetAttr, /* tp_getattro */
    (setattrofunc) 0,                       /* tp_setattro */
    (PyBufferProcs *) 0,                    /* tp_as_buffer */
    (Py_TPFLAGS_DEFAULT|
     Py_TPFLAGS_BASETYPE),                  /* tp_flags */
    doc_pipeline,                            /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    0,                                      /* tp_methods */
    0,                                      /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    PyType_GenericNew,                      /* tp_new */
    PyObject_Del,                           /* tp_free */
}

#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H_