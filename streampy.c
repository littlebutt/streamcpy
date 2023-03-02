#include <Python.h>

#include "modules/pipeline.h"

/*
 * Implements an example function.
 */
PyDoc_STRVAR(streampy_example_doc, "example(obj, number)\
\
Example function");

PyObject *streampy_example(PyObject *self, PyObject *args, PyObject *kwargs) {
    /* Shared references that do not need Py_DECREF before returning. */
    PyObject *obj = NULL;
    int number = 0;

    /* Parse positional and keyword arguments */
    static char* keywords[] = { "obj", "number", NULL };
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", keywords, &obj, &number)) {
        return NULL;
    }

    /* Function implementation starts here */

    if (number < 0) {
        PyErr_SetObject(PyExc_ValueError, obj);
        return NULL;    /* return NULL indicates error */
    }

    Py_RETURN_NONE;
}

/*
 * List of functions to add to streampy in exec_streampy().
 */
static PyMethodDef streampy_functions[] = {
    { "example", (PyCFunction)streampy_example, METH_VARARGS | METH_KEYWORDS, streampy_example_doc },
    { NULL, NULL, 0, NULL } /* marks end of array */
};

/*
 * Initialize streampy. May be called multiple times, so avoid
 * using static state.
 */
int exec_streampy(PyObject *module) {
    PyModule_AddFunctions(module, streampy_functions);

    PyModule_AddStringConstant(module, "__author__", "luoga");
    PyModule_AddStringConstant(module, "__version__", "1.0.0");
    PyModule_AddIntConstant(module, "year", 2023);

    return 0; /* success */
}

/*
 * Documentation for streampy.
 */
PyDoc_STRVAR(streampy_doc, "The streampy module");


static PyModuleDef_Slot streampy_slots[] = {
    { Py_mod_exec, exec_streampy },
    { 0, NULL }
};

static PyModuleDef streampy_def = {
    PyModuleDef_HEAD_INIT,
    "streampy",
    streampy_doc,
    0,              /* m_size */
    NULL,           /* m_methods */
    streampy_slots,
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_streampy() {
    return PyModuleDef_Init(&streampy_def);
}
