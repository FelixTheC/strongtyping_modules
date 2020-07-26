//
// Created by felix on 23.07.20.
//
#define PY_SSIZE_T_CLEAN
#include <Python.h>

long check_types(PyObject *obj, PyObject *typeObj);
long union_element_types(PyObject *obj, PyObject *typeObj);
long list_element_types(PyObject *obj, PyObject *typeObj);
long tuple_element_types(PyObject *obj, PyObject *typeObj);
long check_suptypes(PyObject *element, PyObject *type, int pyType);

long check_suptypes(PyObject *element, PyObject *type, int pyType) {
    /*
     * pyType => 1: list, 2: tuple
     */
    PyObject *baseType = PyObject_GetAttrString(type, "__origin__");

    if (pyType == 1 && PyObject_IsInstance(element, baseType) == 1) {
        PyObject *supTypes = PyObject_GetAttrString(type, "__args__");
        long result = 0;
        long supTypeSize = PyObject_Length(supTypes);
        for (int x = 0; x < supTypeSize; x++) {
            PyObject *supType = PyTuple_GetItem(supTypes, x);
            long recursiveResult = list_element_types(element, supType);
            result += recursiveResult;
        }
        return result >= supTypeSize;
    } else if (pyType == 2 && PyObject_IsInstance(element, baseType) == 1) {
        PyObject *supTypes = PyObject_GetAttrString(type, "__args__");
        long result = 0;
        long supTypeSize = PyObject_Length(supTypes);
        for (int x = 0; x < supTypeSize; x++) {
            PyObject *supType = PyTuple_GetItem(supTypes, x);
            long recursiveResult = tuple_element_types(element, supType);
            result += recursiveResult;
        }
        return result >= supTypeSize;
    } else {
        return PyObject_IsInstance(element, baseType);
    }
}

long union_element_types(PyObject *obj, PyObject *typeObj) {
    long ttype = 0;
    long result = 0;

    if (strcmp(obj->ob_type->tp_name, "list") == 0) {
        ttype = 1;
    }

    if (strcmp(obj->ob_type->tp_name, "tuple") == 0) {
        ttype = 2;
    }

    PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
    long typeArgSize = PyObject_Length(typeArgs);

    for (int i = 0; i < typeArgSize; i++) {
        PyObject *typeArg = PyTuple_GetItem(typeArgs, i);

        if (PyObject_HasAttrString(typeArg, "__args__") == 0 && ttype == 0) {
            result += PyObject_IsInstance(obj, typeArg);
        } else {

            PyObject *baseType = PyObject_GetAttrString(typeArg, "__origin__");

            if (baseType == NULL) {
                baseType = typeArg;
            }

            if (ttype == 1 && PyObject_IsInstance(obj, baseType) == 1) {
                long recursiveResult = list_element_types(obj, typeArg);
                result += recursiveResult;
            } else if (ttype == 2 && PyObject_IsInstance(obj, baseType) == 1) {
                long recursiveResult = tuple_element_types(obj, typeArg);
                result += recursiveResult;
            }
        }
    }
    return result;
}

long tuple_element_types(PyObject *obj, PyObject *typeObj) {
    long objSize = PyObject_Length(obj);

    if (PyObject_HasAttrString(typeObj, "__args__")) {

        PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
        PyObject *baseType = PyObject_GetAttrString(typeObj, "__origin__");
        long typeArgSize = PyObject_Length(typeArgs);

        if ((PyObject_IsInstance(obj, baseType) == 1) && (objSize == typeArgSize)) {
            long result;

            for (int i = 0; i < objSize; i++) {
                int supType = 0;

                PyObject *tupleElement = PyTuple_GetItem(obj, i);
                PyObject *tupleElementType = PyTuple_GetItem(typeArgs, i);

                if (strcmp(tupleElement->ob_type->tp_name, "list") == 0) {
                    supType = 1;
                }

                if (strcmp(tupleElement->ob_type->tp_name, "tuple") == 0) {
                    supType = 2;
                }

                if (PyObject_HasAttrString(tupleElementType, "__args__")) {
                    PyObject *newBaseType = PyObject_GetAttrString(tupleElementType, "__origin__");

                    if (strcmp(newBaseType->ob_type->tp_name, "_SpecialForm") == 0) {
                        result += union_element_types(tupleElement, tupleElementType);
                    } else {
                        if (supType == 0) {
                            continue;
                        } else {
                            if (check_suptypes(tupleElement, tupleElementType, supType) == 0) {
                                return 0;
                            }
                        }
                    }

                } else {
                    if (PyObject_IsInstance(tupleElement, tupleElementType) == 0) {
                        return 0;
                    }
                }
            }
            return result >= objSize;
        } else {
            return 0;
        }

    } else {
        return check_types(obj, typeObj);
    }
}

long list_element_types(PyObject *obj, PyObject *typeObj) {
    long objSize = PyObject_Length(obj);

    if (PyObject_HasAttrString(typeObj, "__args__")) {

        PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
        long typeArgSize = PyObject_Length(typeArgs);

        long result = 0;
        for(int i = 0; i < objSize; i++) {
            int supType = 0;

            PyObject *listElem = obj->ob_type->tp_as_sequence->sq_item(obj, i);

            if (strcmp(listElem->ob_type->tp_name, "list") == 0) {
                supType = 1;
            }

            if (strcmp(listElem->ob_type->tp_name, "tuple") == 0) {
                supType = 2;
            }

            for(int j = 0; j < typeArgSize; j++) {
                PyObject *type = PyTuple_GetItem(typeArgs, j);

                if (PyObject_HasAttrString(type, "__args__")) {
                    PyObject *baseType = PyObject_GetAttrString(type, "__origin__");

                    if (strcmp(baseType->ob_type->tp_name, "_SpecialForm") == 0) {
                        result += union_element_types(listElem, type);
                    } else {
                        if (supType == 0) {
                         continue;
                        } else {
                         result += check_suptypes(listElem, type, supType);
                        }
                    }
                } else {
                    result += PyObject_IsInstance(listElem, type);
                }
            }
        }
        return result >= objSize;

    } else {
        return check_types(obj, typeObj);
    }
}


long check_types(PyObject *obj, PyObject *typeObj) {
    long objSize = PyObject_Length(obj);

    if (objSize > 1) {
        long result = 0;
        if (PyObject_IsInstance(obj, typeObj) == 1) {
            return 1;
        }
        for(int i = 0; i < objSize; i++) {
            PyObject *listElem = obj->ob_type->tp_as_sequence->sq_item(obj, i);
            if (PyObject_IsInstance(listElem, typeObj) == 0)
                break;
            else {
                result++;
            }
        }
        // 0 means False
        return result >= objSize;
    } else {
        return PyObject_IsInstance(obj, typeObj);
    }
}


static PyObject* sttyping_stinstance(PyObject *self, PyObject *args) {

    static PyObject *obj;
    static PyObject *typeObj;

    // https://docs.python.org/3/c-api/arg.html
    if (!PyArg_ParseTuple(args, "OO", &obj, &typeObj))
        return NULL;

    return PyBool_FromLong(list_element_types(obj, typeObj));
}

static PyMethodDef SttypingMethods[] = {
        {"st_instance",  sttyping_stinstance, METH_VARARGS,"Utils for strongtyping."},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef sttypingmodule = {
        PyModuleDef_HEAD_INIT,
        "sttyping",   /* name of module */
        "Utils for strongtyping.", /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        SttypingMethods
};

PyMODINIT_FUNC PyInit_sttyping(void) {
    return PyModule_Create(&sttypingmodule);
}
