#include <Python.h>

#include "modules/stream.h"


/*
 * Documentation for streampy.
 */
PyDoc_STRVAR(streampy_doc, "The streampy module");


static PyModuleDef streampy_def = {
    PyModuleDef_HEAD_INIT,
    "streampy",
    streampy_doc,
    -1,             /* m_size */
    NULL,           /* m_methods */
    NULL,           /* m_slots */
    NULL,           /* m_traverse */
    NULL,           /* m_clear */
    NULL,           /* m_free */
};

PyMODINIT_FUNC PyInit_streampy() {
    PyObject* m;
    if (PyType_Ready(&Pipeline_type) < 0)
    {
        return NULL;
    }
    if (PyType_Ready(&Stream_type) < 0)
    {
        return NULL;
    }
    m = PyModule_Create(&streampy_def);
    if (!m)
    {
        return NULL;
    }
    PyModule_AddStringConstant(m, "__author__", "littlebutt");
    PyModule_AddStringConstant(m, "__version__", "1.0.0");
    Py_INCREF(&Pipeline_type);
    Py_INCREF(&Stream_type);
    if (PyModule_AddObject(m, "Pipeline", (PyObject *) &Pipeline_type) < 0)
    {
        Py_DECREF(&Pipeline_type);
        Py_DECREF(&Stream_type);
        Py_DECREF(m);
        return NULL;
    }
    if (PyModule_AddObject(m, "Stream", (PyObject *) &Stream_type) < 0)
    {
        Py_DECREF(&Pipeline_type);
        Py_DECREF(&Stream_type);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}
