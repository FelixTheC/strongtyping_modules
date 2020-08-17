//
// Created by felix on 23.07.20.
//
#define PY_SSIZE_T_CLEAN
#include <Python.h>

long checkTypes(PyObject *obj, PyObject *typeObj);
long unionElementTypes(PyObject *obj, PyObject *typeObj);
long listElementTypes(PyObject *obj, PyObject *typeObj);
long tupleElementTypes(PyObject *obj, PyObject *typeObj);
long setElementTypes(PyObject *originalObj, PyObject *typeObj);
long dictElementTypes(PyObject *obj, PyObject *typeObj);
long checkSubtypes(PyObject *element, PyObject *type, int pyType);
int whichSubtype(PyObject *element);
long unionOrOther(PyObject *type, PyObject *element, int supType);

int whichSubtype(PyObject *element) {
    int subType = 0;

    if (strcmp(element->ob_type->tp_name, "list") == 0) {
        subType = 1;
    }

    if (strcmp(element->ob_type->tp_name, "tuple") == 0) {
        subType = 2;
    }

    if (strcmp(element->ob_type->tp_name, "dict") == 0) {
        subType = 3;
    }

    if (strcmp(element->ob_type->tp_name, "set") == 0) {
        subType = 4;
    }

    return subType;
}

long unionOrOther(PyObject *type, PyObject *element, int supType) {
    PyObject *baseType = PyObject_GetAttrString(type, "__origin__");
    long result = 0;

    if (strcmp(baseType->ob_type->tp_name, "_SpecialForm") == 0) {
        result += unionElementTypes(element, type);
    } else {
        if (supType == 0) {
            return -1;
        } else {
            if (checkSubtypes(element, type, supType) == 0) {
                return 0;
            }
        }
    }
    return result;
}

long checkSubtypes(PyObject *element, PyObject *type, int pyType) {
    /*
     * pyType => 1: list, 2: tuple, 3: dict, 4: set
     */
    PyObject *baseType = PyObject_GetAttrString(type, "__origin__");

    if (pyType == 1 && PyObject_IsInstance(element, baseType) == 1) {
        PyObject *supTypes = PyObject_GetAttrString(type, "__args__");
        long result = 0;
        long supTypeSize = PyObject_Length(supTypes);
        for (int x = 0; x < supTypeSize; x++) {
            PyObject *supType = PyTuple_GetItem(supTypes, x);
            long recursiveResult = listElementTypes(element, supType);
            result += recursiveResult;
        }
        return result >= supTypeSize;
    } else if (pyType == 2 && PyObject_IsInstance(element, baseType) == 1) {
        PyObject *supTypes = PyObject_GetAttrString(type, "__args__");
        long result = 0;
        long supTypeSize = PyObject_Length(supTypes);
        for (int x = 0; x < supTypeSize; x++) {
            PyObject *supType = PyTuple_GetItem(supTypes, x);
            long recursiveResult = tupleElementTypes(element, supType);
            result += recursiveResult;
        }
        return result >= supTypeSize;
    } else if (pyType == 3 && PyObject_IsInstance(element, baseType) == 1) {
        PyObject *supTypes = PyObject_GetAttrString(type, "__args__");
        long result = 0;
        long supTypeSize = PyObject_Length(supTypes);
        for (int x = 0; x < supTypeSize; x++) {
            PyObject *supType = PyTuple_GetItem(supTypes, x);
            long recursiveResult = dictElementTypes(element, supType);
            result += recursiveResult;
        }
        return result >= supTypeSize;
    } else if (pyType == 4 && PyObject_IsInstance(element, baseType) == 1) {
        PyObject *supTypes = PyObject_GetAttrString(type, "__args__");
        long result = 0;
        long supTypeSize = PyObject_Length(supTypes);
        for (int x = 0; x < supTypeSize; x++) {
            PyObject *supType = PyTuple_GetItem(supTypes, x);
            long recursiveResult = setElementTypes(element, supType);
            result += recursiveResult;
        }
        return result >= supTypeSize;
    } else {
        return PyObject_IsInstance(element, baseType);
    }
}

long unionElementTypes(PyObject *obj, PyObject *typeObj) {
    long ttype = 0;
    long result = 0;
    ttype = whichSubtype(obj);

    PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
    long typeArgSize = PyObject_Length(typeArgs);

    for (int i = 0; i < typeArgSize; i++) {
        PyObject *typeArg = PyTuple_GetItem(typeArgs, i);

        if (PyObject_HasAttrString(typeArg, "__args__") == 0 && ttype == 0) {
            result += PyObject_IsInstance(obj, typeArg);
        } else if (PyObject_HasAttrString(typeArg, "__origin__")) {

            PyObject *baseType = PyObject_GetAttrString(typeArg, "__origin__");

            if (baseType == NULL) {
                baseType = typeArg;
            }

            if (ttype == 1 && PyObject_IsInstance(obj, baseType) == 1) {
                long recursiveResult = listElementTypes(obj, typeArg);
                result += recursiveResult;
            } else if (ttype == 2 && PyObject_IsInstance(obj, baseType) == 1) {
                long recursiveResult = tupleElementTypes(obj, typeArg);
                result += recursiveResult;
            } else if (ttype == 3 && PyObject_IsInstance(obj, baseType) == 1) {
                long recursiveResult = dictElementTypes(obj, typeArg);
                result += recursiveResult;
            } else if (ttype == 4 && PyObject_IsInstance(obj, baseType) == 1) {
                long recursiveResult = setElementTypes(obj, typeArg);
                result += recursiveResult;
            }
        } else {
            result += PyObject_IsInstance(obj, typeArg);
        }
    }
    return result;
}


long setElementTypes(PyObject *originalObj, PyObject *typeObj) {
    long objSize = PySet_Size(originalObj);
    PyObject *test_obj = PySet_New(originalObj);

    if (PyObject_HasAttrString(typeObj, "__args__")) {
        PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
        PyObject *baseType = PyObject_GetAttrString(typeObj, "__origin__");

        if (PyObject_IsInstance(test_obj, baseType) == 1) {
            PyObject *setElementType = PyTuple_GetItem(typeArgs, 0);

            long typeArgSize = PyObject_Length(typeArgs);
            long result = 0;

            for (int i = 0; i < objSize; i++) {
                int supType = 0;
                PyObject *setElement = PySet_Pop(test_obj);

                supType = whichSubtype(setElement);

                for (int j = 0; j < typeArgSize; j++) {
                    PyObject *type = PyTuple_GetItem(typeArgs, j);

                    if (PyObject_HasAttrString(type, "__args__")) {
                        long tmp = unionOrOther(type, setElement, supType);
                        if (tmp > 1) {
                            result += tmp;
                        } else {
                            continue;
                        }
                    } else if (PyObject_HasAttrString(type, "_name")) {
                        PyObject *_name = PyObject_GetAttrString(type, "_name");
                        if (strcmp(_name->ob_type->tp_name, "Any") == 0) {
                            return 1;
                        }
                    } else {
                        result += PyObject_IsInstance(setElement, type);
                    }
                }
            }
            return result >= objSize;

        } else {
            return checkTypes(originalObj, typeObj);
        }
    }
}


long dictElementTypes(PyObject *obj, PyObject *typeObj) {
    long objSize = PyDict_Size(obj);

    if (PyObject_HasAttrString(typeObj, "__args__")) {

        PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
        PyObject *keyTypes = PyTuple_GetItem(typeArgs, 0);
        PyObject *valueTypes = PyTuple_GetItem(typeArgs, 1);

        PyObject *objItems = PyDict_Items(obj);

        long result = 0;

        for (int i = 0; i < objSize; i++) {
            PyObject *objItem = PyList_GetItem(objItems, i);
            PyObject *key = PyTuple_GetItem(objItem, 0);
            PyObject *value = PyTuple_GetItem(objItem, 1);

            if (PyObject_HasAttrString(keyTypes, "__args__")) {
                int supType = whichSubtype(key);

                long tmp = unionOrOther(keyTypes, key, supType);

                if (tmp > 1) {
                    result += tmp;
                } else {
                    continue;
                }

            } else if (PyObject_HasAttrString(keyTypes, "_name")) {
                PyObject *_name = PyObject_GetAttrString(keyTypes, "_name");
                if (strcmp(_name->ob_type->tp_name, "Any") == 0) {
                    return 1;
                }
            } else {
                if (PyObject_IsInstance(key, keyTypes) == 0) return 0;
            }

            if (PyObject_HasAttrString(valueTypes, "__args__")) {
                int supType = whichSubtype(value);
                long tmp = unionOrOther(valueTypes, value, supType);
                if (tmp >= 1) {
                    result += tmp;
                } else {
                    continue;
                }

            } else if (PyObject_HasAttrString(valueTypes, "_name")) {
                PyObject *_name = PyObject_GetAttrString(valueTypes, "_name");
                if (strcmp(_name->ob_type->tp_name, "Any") == 0) {
                    return 1;
                }
            } else {
                if (PyObject_IsInstance(value, valueTypes) == 0) return 0;
            }
        }
        if (result >= objSize) {
            return 1;
        } else {
            return 0;
        }

    } else {
        return checkTypes(obj, typeObj);
    }
}

long tupleElementTypes(PyObject *obj, PyObject *typeObj) {
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

                supType = whichSubtype(tupleElement);

                if (PyObject_HasAttrString(tupleElementType, "__args__")) {

                    long tmp = unionOrOther(tupleElement, tupleElementType, supType);
                    if (tmp > 1) {
                        result += tmp;
                    } else if (tmp == 0) {
                        return 0;
                    } else {
                        continue;
                    }

                } else if (PyObject_HasAttrString(tupleElementType, "_name")) {
                    PyObject *_name = PyObject_GetAttrString(tupleElementType, "_name");
                    if (strcmp(_name->ob_type->tp_name, "Any") == 0) {
                        return 1;
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
        return checkTypes(obj, typeObj);
    }
}

long listElementTypes(PyObject *obj, PyObject *typeObj) {
    long objSize = PyObject_Length(obj);

    if (PyObject_HasAttrString(typeObj, "__args__")) {

        PyObject *typeArgs = PyObject_GetAttrString(typeObj, "__args__");
        long typeArgSize = PyObject_Length(typeArgs);

        long result = 0;
        for(int i = 0; i < objSize; i++) {

            PyObject *listElem = obj->ob_type->tp_as_sequence->sq_item(obj, i);

            int supType = whichSubtype(listElem);

            for(int j = 0; j < typeArgSize; j++) {
                PyObject *type = PyTuple_GetItem(typeArgs, j);

                if (PyObject_HasAttrString(type, "__args__")) {
                    long tmp = unionOrOther(type, listElem, supType);
                    if (tmp > 1) {
                        result += tmp;
                    } else if (tmp == 0) {
                        return 0;
                    } else {
                        continue;
                    }

                } else if (PyObject_HasAttrString(type, "_name")) {
                    PyObject *_name = PyObject_GetAttrString(type, "_name");
                    if (strcmp(_name->ob_type->tp_name, "Any") == 0) {
                        return 1;
                    }
                } else {
                    result += PyObject_IsInstance(listElem, type);
                }
            }
        }
        return result >= objSize;

    } else {
        return checkTypes(obj, typeObj);
    }
}


long checkTypes(PyObject *obj, PyObject *typeObj) {
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


static PyObject* sttyping_stlist_instance(PyObject *self, PyObject *args) {

    static PyObject *obj;
    static PyObject *typeObj;

    // https://docs.python.org/3/c-api/arg.html
    if (!PyArg_ParseTuple(args, "OO", &obj, &typeObj))
        return NULL;

    return PyBool_FromLong(listElementTypes(obj, typeObj));
}

static PyObject* sttyping_sttuple_instance(PyObject *self, PyObject *args) {

    static PyObject *obj;
    static PyObject *typeObj;

    if (!PyArg_ParseTuple(args, "OO", &obj, &typeObj))
        return NULL;

    return PyBool_FromLong(tupleElementTypes(obj, typeObj));
}

static PyObject* sttyping_stdict_instance(PyObject *self, PyObject *args) {

    static PyObject *obj;
    static PyObject *typeObj;

    if (!PyArg_ParseTuple(args, "OO", &obj, &typeObj))
        return NULL;

    return PyBool_FromLong(dictElementTypes(obj, typeObj));
}

static PyObject* sttyping_stset_instance(PyObject *self, PyObject *args) {

    static PyObject *obj;
    static PyObject *typeObj;

    if (!PyArg_ParseTuple(args, "OO", &obj, &typeObj))
        return NULL;

    return PyBool_FromLong(setElementTypes(obj, typeObj));
}

static PyMethodDef SttypingMethods[] = {
        {"st_list",  sttyping_stlist_instance, METH_VARARGS,"Type checking for list elements."},
        {"st_tuple",  sttyping_sttuple_instance, METH_VARARGS,"Type checking for tuple elements."},
        {"st_dict",  sttyping_stdict_instance, METH_VARARGS,"Type checking for dict items."},
        {"st_set",  sttyping_stset_instance, METH_VARARGS,"Type checking for set elements."},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef sttypingmodule = {
        PyModuleDef_HEAD_INIT,
        "sttyping",   /* name of module */
        "Instance checks only for specific types. Submodule for strongtyping.", /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
        SttypingMethods
};

PyMODINIT_FUNC PyInit_sttyping(void) {
    return PyModule_Create(&sttypingmodule);
}
