#ifndef PIPELINE_H_
#define PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "op_types.h"

typedef struct _PyPipeline{
    PyObject_HEAD

    enum op_type type;      // op_type

    PyObject* op_method;    // op_method

    struct _PyPipeline* next; // next

} PyPipeline;

static PyTypeObject PyPipeline_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "streampy.Pipeline",
    .tp_doc = PyDoc_STR("Pipeline Object"),
    .tp_basicsize = sizeof(PyPipeline),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
};

#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H_