#ifndef LIST_H
#define LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

typedef struct {

    PyObject** _list; // inner list

    ssize_t len;
}list;

list*
list_New()
{
    list* retval = (list*)PyMem_RawMalloc(sizeof(list));
    if (!retval)
    {
        return NULL;
    }
    retval->len = 0;
    retval->_list = NULL;
    return retval;
}

int
list_append(list* target, PyObject* item)
{
    target->len ++;
    target->_list = (PyObject**)PyMem_RawRealloc(target->_list, target->len);
    if (!target->_list)
    {
        return -1;
    }
    target->_list[target->len - 1] = item;
    return target->len;
}

void
list_free(list* target)
{
    if (!target)
    {
        return;
    }
    PyMem_RawFree(target->_list);
    PyMem_RawFree(target);
}



#ifdef __cplusplus
}
#endif

#endif // LIST_H