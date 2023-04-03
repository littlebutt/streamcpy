#ifndef SORT_H
#define SORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

int
sort_compare(PyObject* param1, PyObject* param2, PyObject* comparefunc)
{
    PyObject* res = PyObject_CallFunction(comparefunc, "OO", param1, param2);
    if (!PyLong_Check(res))
    {
        PyErr_SetString(PyExc_TypeError, "The return value of the comparator has a wrong type");
        Py_XDECREF(res);
        return -2;
    }
    int retval = PyLong_AsLong(res);
    Py_DECREF(res);
    return retval;
}

#ifdef __cplusplus
}
#endif

#endif // SORT_H