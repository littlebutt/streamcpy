#ifndef SORT_H
#define SORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>


int
_sort_compare(PyObject* obj1, PyObject* obj2, PyObject* comparefunc)          // May set Exception Flag!
{
    PyObject* res = PyObject_CallFunction(comparefunc, "OO", obj1, obj2);
    if (!res)
    {
        return -2;
    }
    if(!PyLong_Check(res))
    {
        PyErr_SetString(PyExc_TypeError, "Comparator returns wrong type");
        return -2;
    }
    int retval = (int)PyLong_AsLong(res);
    Py_XDECREF(res);
    return retval;
}


PyObject**
_sort_toarray(PyListObject* list)
{
    size_t len = (size_t)PyList_Size(list);
    PyObject** retval = (PyObject**)PyMem_RawMalloc(sizeof(PyObject*) * (len + 1));
    if (!retval)
    {
        return NULL;
    }
    for (size_t i = 0; i<len; ++i)
    {
        retval[i] = PyList_GetItem(list, (Py_ssize_t)i);
    }
    retval[len] = NULL;
    return retval;
}

PyListObject*
_sort_tolist(PyObject** array)
{
    long len = 0;
    for(; array[len] != NULL; ++len);
    PyListObject* retval = PyList_New(len);
    for (Py_ssize_t i = 0; i<len; ++i)
    {
        PyList_SetItem(retval, i, array[i]);
    }
    return retval;
}

void
_sort_freearray(PyObject** array)
{
    PyMem_RawFree(array);
}

long
_sort(PyObject** list, long left, long right, PyObject* comparefunc)
{
    PyObject* target = list[left];
    while (left < right)
    {
        while(left < right && _sort_compare(list[right], target, comparefunc) >= 0) 
            right--;  
        if(left < right) 
        {
            list[left] = list[right]; 
            left++;
        }
 
        while(left < right && _sort_compare(list[left], target, comparefunc) < 0)
            left++;  
        if(left < right) 
        {
            list[right] = list[left];
            right--;
        }
    }
    list[left] = target;
 
    return left;
}

void
sort(PyObject** list, long left, long right, PyObject* comparefunc)
{
    if (left < right)
    {
        long i = _sort(list, left, right, comparefunc);
        sort(list, left, i - 1, comparefunc);
        sort(list, i + 1, right, comparefunc);
    }
}

#ifdef __cplusplus
}
#endif

#endif // SORT_H