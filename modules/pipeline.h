#ifndef PIPELINE_H_
#define PIPELINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "op_types.h"

typedef struct _PyPipeline {
    PyObject_HEAD

    enum op_type type;      // op_type

    PyObject* op_method;    // op_method

    struct _PyPipeline* next; // next

} PyPipeline;

static int
PyPipeline_init(PyPipeline* self, PyObject *args, PyObject *kwds) {
    static char* kwlist[] = {"op_type", "op_method", NULL};
    PyObject* op_method;
    PyObject* next;
    PyObject* tmp;
    int* _i = 0;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|iOO", kwlist,
                                    &_i, &op_method, &next)) {
                                        return -1;
                                    }
    self->type = get_op_type(_i);
}

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