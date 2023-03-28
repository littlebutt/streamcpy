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
    PyObject* op_method_repr = PyObject_Repr(pl->op_method);
    if (!op_method_repr)
    {
        return NULL;
    }
    retval = PyUnicode_FromFormat("Pipeline(%U, %d)", op_method_repr, pl->op_type);
    Py_XDECREF(op_method_repr);
    return retval;
}

// inner method
static int
Pipeline_append(Pipeline* pl, const int op_type, PyObject* op_method)
{
    Pipeline* last = PyObject_New(Pipeline, &Pipeline_type);
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

// inner method
static PyObject*
Pipeline_execute(Pipeline* pl, PyObject* init_data)
{
    PyListObject* data = (PyListObject*)PyList_New(0);
    if(!data)
    {
        goto FAILURE;
    }
    PyObject* iter = PyObject_GetIter(init_data);
    if(!iter)
    {
        Py_DECREF(data);
        goto FAILURE;
    }
    PyObject* item;
    while ((item = PyIter_Next(iter)))
    {
        if(PyList_Append(data, item) < 0)
        {
            Py_DECREF(data);
            Py_DECREF(iter);
            Py_DECREF(item);
            goto FAILURE;
        }
        Py_DECREF(item);
    }
    Py_DECREF(init_data);
    
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
                    Py_XDECREF(res);
                }
                PyObject* tmp = data;
                data = new_data;
                Py_XDECREF(tmp);
                break;
            }
            case OP_TYPE_FILTER:
            {
                PyListObject* new_data = (PyListObject*)PyList_New(0);
                if (!new_data)
                {
                    goto FAILURE;
                }
                for (Py_ssize_t i = 0; i<PyList_Size(data); ++i)
                {
                    PyObject* item = PyList_GetItem(data, i);
                    Py_XINCREF(item);
                    PyObject* res = PyObject_CallFunction(ptr->op_method, "O", item);
                    if (!res)
                    {
                        Py_XDECREF(item);
                        goto FAILURE;
                    }
                    if (!PyBool_Check(res))
                    {
                        PyErr_SetString(PyExc_RuntimeError, "The given method cannot return a bool");
                        Py_XDECREF(item);
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    if(res == Py_True && PyList_Append(new_data, item) == -1)
                    {
                        Py_XDECREF(item);
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    Py_XDECREF(item);
                    Py_DECREF(res);
                }
                PyObject* tmp = data;
                data = new_data;
                Py_XDECREF(tmp);
                break;
            }
            case OP_TYPE_DISTINCT:
            {
                PyObject* _set = PySet_New(NULL);
                if (!_set)
                {
                    goto FAILURE;
                }
                PyListObject* new_data = (PyListObject*)PyList_New(0);
                if (!new_data)
                {
                    Py_DECREF(_set);
                    goto FAILURE;
                }
                for (Py_ssize_t i = 0; i<PyList_Size(data); ++i)
                {
                    PyObject* item = PyList_GetItem(data, i);
                    if (PySet_Contains(_set, item) <= 0)
                    {
                        if (PyList_Append(new_data, item) < 0)
                        {
                            Py_DECREF(_set);
                            Py_DECREF(new_data);
                            goto FAILURE;
                        }
                        if (PySet_Add(_set, item))
                        {
                            Py_DECREF(_set);
                            Py_DECREF(new_data);
                            goto FAILURE;
                        }
                    }
                }
                PyObject* tmp = data;
                data = new_data;
                Py_XDECREF(tmp);
                Py_DECREF(_set);
                break;
            }
            case OP_TYPE_LIMIT:
            {
                PyListObject* new_data = PyList_GetSlice(data, 0, PyLong_AsSsize_t(ptr->op_method));
                if (!new_data)
                {
                    goto FAILURE;
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
            case OP_TYPE_REDUCE:
            {
                if (PyList_Size(data) == 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, "The size of data is 0");
                    goto FAILURE;
                }
                PyObject* res = PyList_GetItem(data, 0);
                if (PyList_Size(data) == 1)
                {
                    Py_XDECREF(pl);
                    Py_XDECREF(data);
                    Py_INCREF(res);
                    return res;
                }
                for (Py_ssize_t i = 1; i<PyList_Size(data); ++i)
                {
                    PyObject* param = PyList_GetItem(data, i);
                    PyObject* tmp = PyObject_CallFunction(ptr->op_method, "OO", res, param);
                    if (!tmp)
                    {
                        PyErr_SetString(PyExc_RuntimeError, "The reduce method is invoked with a exception");
                        goto FAILURE;
                    }
                    else
                    {
                        Py_XDECREF(res);
                        res = tmp;
                    }
                }
                Py_XDECREF(pl);
                Py_XDECREF(data);
                return res;
            }
            case OP_TYPE_COLLECT:
            {
                for (Py_ssize_t i = 0; i<PyList_Size(data); ++i)
                {
                    if (PyList_Append(ptr->op_method, PyList_GetItem(data, i)) == -1)
                    {
                        goto FAILURE;
                    }
                }
                break;
            }
            default: {
                PyErr_SetString(PyExc_NotImplementedError, "Unimplemented op_method!");
                Py_XDECREF(pl);
                Py_XDECREF(data);
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
        ptr = ptr->next;
    }
    Py_INCREF(Py_None);
    return Py_None;

    FAILURE:
        Py_XDECREF(pl);
        Py_XDECREF(data);
        Py_INCREF(Py_None);
        return Py_None;
}

static PyMemberDef Pipeline_members[] = {
    {"op_type", T_INT, offsetof(Pipeline, op_type), 0, PyDoc_STR("a value representing the method type")},
    {"op_method", T_OBJECT, offsetof(Pipeline, op_method), 0, PyDoc_STR("a callable value for eval the given data")},
    {"next", T_OBJECT, offsetof(Pipeline, next), 0, PyDoc_STR("a pointer pointing to the next Pipeline")},
    {NULL}
};

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
    doc_pipeline,                           /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    0,                                      /* tp_methods */
    Pipeline_members,                       /* tp_members */
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
};

#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H_