#ifndef PIPELINE_H_
#define PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "op_types.h"
#include "sort.h" // for OP_TYPE_SORTED
#include "list.h"

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
    if (pl->op_method && Py_REFCNT(pl->op_method) >= 0)
    {
        Py_DECREF(pl->op_method);
    }
    if (pl->next && Py_REFCNT(pl->next) >= 0)
    {
        Py_DECREF(pl->next);
    }
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

list*
_Pipeline_execute_map(list* data, PyObject* op_method)
{
    list* new_data = list_New();
    if (!new_data)
    {
        return NULL;
    }
    for (Py_ssize_t i = 0; i < data->len; ++ i)
    {
        PyObject* res = PyObject_CallFunction(op_method, "O", data->_list[i]);
        if (!res)
        {
            list_Free(new_data);
            return NULL;
        }
        if(list_append(new_data, res) == -1)
        {
            Py_DECREF(res);
            list_Free(new_data);
            return NULL;
        }
        Py_DECREF(res);
    }
    list_Free(data);
    return new_data;
}

// inner method
static PyObject*
Pipeline_execute(Pipeline* pl /*borrowed ref*/, PyObject* init_data /*borrowed ref*/)
{
    list* list_data = list_New();
    if (!list_data)
    {
        goto FAILURE;
    }
    PyObject* iter = PyObject_GetIter(init_data);
    if(!iter)
    {
        goto FAILURE;
    }
    PyObject* item;
    while ((item = PyIter_Next(iter)))
    {
        if (list_append(list_data, item) < 0)
        {
            Py_DECREF(iter);
            Py_DECREF(item);
            goto FAILURE;
        }
        Py_DECREF(item);
    }
    Py_XDECREF(iter);
    
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
                list* res = _Pipeline_execute_map(list_data, ptr->op_method);
                if (!res)
                {
                    goto FAILURE;
                }
                list_data = res;
                break;
            }
            case OP_TYPE_FILTER:
            {
                list* new_data = list_New();
                if (!new_data)
                {
                    goto FAILURE;
                }
                for (Py_ssize_t i = 0; i < list_data->len; ++ i)
                {
                    PyObject* item = list_data->_list[i];
                    Py_INCREF(item);
                    PyObject* res = PyObject_CallFunction(ptr->op_method, "O", item);
                    if (!res)
                    {
                        Py_DECREF(item);
                        list_Free(new_data);
                        goto FAILURE;
                    }
                    if (!PyBool_Check(res))
                    {
                        PyErr_SetString(PyExc_RuntimeError, "The given function cannot return a bool");
                        Py_DECREF(item);
                        list_Free(new_data);
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    if(res == Py_True && list_append(new_data, item) == -1)
                    {
                        Py_DECREF(item);
                        list_Free(new_data);
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    Py_DECREF(item);
                    Py_DECREF(res);
                }
                list* tmp = list_data;
                list_data = new_data;
                list_Free(tmp);
                break;
            }
            case OP_TYPE_DISTINCT:
            {
                PyObject* _set = PySet_New(NULL);
                if (!_set)
                {
                    goto FAILURE;
                }
                list* new_data = list_New();
                if (!new_data)
                {
                    Py_DECREF(_set);
                    goto FAILURE;
                }
                for (Py_ssize_t i = 0; i < list_data->len; ++i)
                {
                    PyObject* item = list_data->_list[i];
                    Py_INCREF(item);
                    if (PySet_Contains(_set, item) <= 0)
                    {
                        if (list_append(new_data, item) < 0)
                        {
                            Py_DECREF(item);
                            Py_DECREF(_set);
                            list_Free(new_data);
                            goto FAILURE;
                        }
                        if (PySet_Add(_set, item))
                        {
                            Py_DECREF(item);
                            Py_DECREF(_set);
                            list_Free(new_data);
                            goto FAILURE;
                        }
                    }
                    Py_DECREF(item);
                }
                list* tmp = list_data;
                list_data = new_data;
                list_Free(tmp);
                Py_DECREF(_set);
                break;
            }
            case OP_TYPE_LIMIT:
            {
                size_t res = list_slice(list_data, (size_t)PyLong_AsSsize_t(ptr->op_method));
                if (res < 0)
                {
                    goto FAILURE;
                }
                break;
            }
            case OP_TYPE_SORTED:
            {
                // PyObject** _array = _sort_toarray(data);
                // if (!_array)
                // {
                //     goto FAILURE;
                // }
                // sort(_array, 0, (int)PyList_Size(data) - 1, ptr->op_method);
                // if (PyErr_Occurred())
                // {
                //     goto FAILURE;
                // }
                // PyListObject* new_data = _sort_tolist(_array);
                // _sort_freearray(_array);
                // PyObject* tmp = data;
                // data = new_data;
                // Py_DECREF(tmp);
                // break;
            }
            case OP_TYPE_FOR_EACH:
            {
                for (Py_ssize_t i = 0; i < list_data->len; ++ i)
                {
                    if(!PyObject_CallFunction(ptr->op_method, "O", list_data->_list[i]))
                    {
                        goto FAILURE;
                    }
                }
                break;
            }
            case OP_TYPE_REDUCE:
            {
                if (list_data->len == 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, "The size of data is 0");
                    goto FAILURE;
                }
                PyObject* res = list_data->_list[0];
                Py_INCREF(res);
                if (list_data->len == 1)
                {
                    list_Free(list_data);
                    return res;
                }
                for (Py_ssize_t i = 1; i < list_data->len; ++ i)
                {
                    PyObject* param = list_data->_list[i];
                    PyObject* tmp = PyObject_CallFunction(ptr->op_method, "OO", res, param);
                    if (!tmp)
                    {
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    else
                    {
                        PyObject* _t = res;
                        Py_XDECREF(_t);
                        res = tmp;
                    }
                }
                list_Free(list_data);
                return res;
            }
            case OP_TYPE_MAX:
            {
                if (list_data->len == 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, "The size of data is 0");
                    goto FAILURE;
                }
                PyObject* res = list_data->_list[0];
                Py_INCREF(res);
                if (list_data->len == 1)
                {
                    list_Free(list_data);
                    return res;
                }
                for (Py_ssize_t i = 1; i < list_data->len; ++ i)
                {
                    PyObject* _compared = list_data->_list[i];
                    PyObject* _res1 = PyObject_CallFunction(ptr->op_method, "O", _compared);
                    PyObject* _res2 = PyObject_CallFunction(ptr->op_method, "O", res);
                    if (!_res1 || !_res2)
                    {
                        Py_XDECREF(_res1);
                        Py_XDECREF(_res2);
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    if (PyObject_RichCompareBool(_res1, _res2, Py_GT) == 1)
                    {
                        PyObject* _t = res;
                        Py_DECREF(_t);
                        Py_INCREF(_compared);
                        res = _compared;
                    }
                    Py_XDECREF(_res1);
                    Py_XDECREF(_res2);
                }
                list_Free(list_data);
                return res;
            }
            case OP_TYPE_MIN:
            {
                if (list_data->len == 0)
                {
                    PyErr_SetString(PyExc_RuntimeError, "The size of data is 0");
                    goto FAILURE;
                }
                PyObject* res = list_data->_list[0];
                Py_INCREF(res);
                if (list_data->len == 1)
                {
                    list_Free(list_data);
                    return res;
                }
                for (Py_ssize_t i = 1; i < list_data->len; ++ i)
                {
                    PyObject* _compared = list_data->_list[i];
                    PyObject* _res1 = PyObject_CallFunction(ptr->op_method, "O", _compared);
                    PyObject* _res2 = PyObject_CallFunction(ptr->op_method, "O", res);
                    if (!_res1 || !_res2)
                    {
                        Py_XDECREF(_res1);
                        Py_XDECREF(_res2);
                        Py_DECREF(res);
                        goto FAILURE;
                    }
                    if (PyObject_RichCompareBool(_res1, _res2, Py_LT) == 1)
                    {
                        PyObject* _t = res;
                        Py_DECREF(_t);
                        Py_INCREF(_compared);
                        res = _compared;
                    }
                    Py_XDECREF(_res1);
                    Py_XDECREF(_res2);
                }
                list_Free(list_data);
                return res;
            }
            case OP_TYPE_COUNT:
            {
                PyObject* res = PyLong_FromSsize_t((Py_ssize_t)list_data->len);
                if (!res)
                {
                    goto FAILURE;
                }
                list_Free(list_data);
                return res;
            }
            case OP_TYPE_ANY_MATCH:
            {
                for (Py_ssize_t i = 0; i < list_data->len; ++ i)
                {
                    PyObject* res = PyObject_CallFunction(ptr->op_method, "O", list_data->_list[i]);
                    if (!res)
                    {
                        goto FAILURE;
                    }
                    else if (!PyBool_Check(res))
                    {
                        PyErr_Format(PyExc_TypeError, "The retval of op_method must be a bool but %U returned", PyObject_Repr(res));
                        goto FAILURE;
                    } else if (res == Py_True)
                    {
                        list_Free(list_data);
                        Py_INCREF(Py_True);
                        return Py_True;
                    }
                }
                list_Free(list_data);
                Py_INCREF(Py_False);
                return Py_False;
            }
            case OP_TYPE_ALL_MATCH:
            {
                for (Py_ssize_t i = 0; i < list_data->len; ++ i)
                {
                    PyObject* res = PyObject_CallFunction(ptr->op_method, "O", list_data->_list[i]);
                    if (!res)
                    {
                        goto FAILURE;
                    }
                    else if (!PyBool_Check(res))
                    {
                        PyErr_Format(PyExc_TypeError, "The retval of op_method must be a bool but %U returned", PyObject_Repr(res));
                        goto FAILURE;
                    } else if (res == Py_False)
                    {
                        list_Free(list_data);
                        Py_INCREF(Py_False);
                        return Py_False;
                    }
                }
                list_Free(list_data);
                Py_INCREF(Py_True);
                return Py_True;
            }
            case OP_TYPE_COLLECT:
            {
                for (Py_ssize_t i = 0; i < list_data->len; ++ i)
                {
                    if (PyList_Append(ptr->op_method, list_data->_list[i]) == -1)
                    {
                        goto FAILURE;
                    }
                }
                list_Free(list_data);
                break;
            }
            default: {
                PyErr_SetString(PyExc_NotImplementedError, "Unimplemented op_method!");
                list_Free(list_data);
                Py_INCREF(Py_None);
                return Py_None;
            }
        }
        ptr = ptr->next;
    }
    Py_INCREF(Py_None);
    return Py_None;

    FAILURE:
        list_Free(list_data);
        if (PyErr_Occurred())
        {
            PyErr_Print();
        }
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