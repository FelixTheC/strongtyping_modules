#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@created: 16.08.20
@author: felix
"""
from __future__ import print_function


cdef int which_subtype(object element):
    cdef int sub_type = 0;
    cdef str element_name

    if hasattr(element, '_name'):
        element_name = element._name
        if element_name == 'List':
            sub_type = 1
        if element_name == 'Tuple':
            sub_type = 2
        if element_name == 'Dict':
            sub_type = 3
        if element_name == 'Set':
            sub_type = 4
        if element_name == 'Any':
            sub_type = -2
        if element_name is None:
            if hasattr(element, '__origin__'):
                if hasattr(element.__origin__, '_name'):
                    if element.__origin__._name == 'Union':
                        sub_type = -1
                    if element.__origin__._name == 'Any':
                        return -2

    return sub_type


cdef int sub_type_result(object obj, object type_obj, int subtype):
    cdef int result = 0

    if subtype == -1:
        result += union_element(obj, type_obj)
    if subtype == 1:
        result += list_elements(obj, type_obj)
    if subtype == 2:
        result += tuple_elements(obj, type_obj)
    if subtype == 3:
        pass
    if subtype == 4:
        result += set_elements(obj, type_obj)

    return result


cdef int union_element(object obj, object type_obj):
    cdef int result = 0
    cdef object type_arg

    if hasattr(type_obj, '__args__'):

        type_args = getattr(type_obj, '__args__')
        for type_arg in type_args:
            ttype = which_subtype(type_arg)

            if ttype == 0:
                result += isinstance(obj, type_arg)
            elif ttype == -2:
                return 1
            else:
                sub_type_result(obj, type_obj, ttype)
    else:
        result += isinstance(obj, type_obj)

    return result


cdef int element_check(object obj, object type_obj):
    cdef object type_args
    cdef object type_arg
    cdef int result = 0
    cdef int ttype

    type_args = getattr(type_obj, '__args__')
    for type_arg in type_args:

        ttype = which_subtype(type_arg)

        if ttype == 0:
            result += isinstance(obj, type_arg)
        elif ttype == -2:
            return -2
        else:
            result += sub_type_result(obj, type_arg, ttype)
    return result


cpdef int dict_elements(object obj, object type_obj):
    cdef object type_args
    cdef object key_type_args
    cdef object value_type_args

    cdef object key_element
    cdef object value_element

    cdef int key_result = 0
    cdef int value_result = 0

    cdef int obj_len = len(obj)

    if hasattr(type_obj, '__args__'):
        type_args = getattr(type_obj, '__args__')
        key_type_args = type_args[0]
        value_type_args = type_args[1]

        for key_element in obj.keys():
            if hasattr(key_type_args, '__args__'):
                key_result += element_check(key_element, key_type_args)
            else:
                key_result += isinstance(key_element, key_type_args)

        for value_element in obj.values():
            if hasattr(value_type_args, '__args__'):
                value_result += element_check(value_element, value_type_args)
            else:
                value_result += isinstance(value_element, value_type_args)

        return key_result >= obj_len and value_result >= obj_len
    else:
        return isinstance(type_obj, dict)



cpdef int set_elements(object obj, object type_obj):
    cdef object type_args
    cdef object set_element
    cdef object type_arg
    cdef int result = 0
    cdef int tmp = 0
    cdef int ttype

    if hasattr(type_obj, '__args__'):

        for set_element in obj:
            tmp = element_check(set_element, type_obj)
            if tmp == -2:
                return 1
            result += tmp

        return result >= len(obj)
    else:
        return isinstance(obj, set)


cpdef int tuple_elements(object obj, object type_obj):
    cdef object type_args
    cdef object tuple_element
    cdef object type_arg
    cdef int result = 0
    cdef int ttype

    if hasattr(type_obj, '__args__'):
        type_args = getattr(type_obj, '__args__')

        if len(obj) == len(type_args) and isinstance(obj, tuple):
            for tuple_element, type_arg in zip(obj, type_args):

                ttype = which_subtype(tuple_element)

                if ttype == 0:
                    result += isinstance(tuple_element, type_arg)
                elif ttype == -2:
                    result += 1
                else:
                    result += sub_type_result(tuple_element, type_arg, ttype)
            return result >= len(obj)
        else:
            return 0
    else:
        return isinstance(type_obj, tuple)


cpdef int list_elements(object obj, object type_obj):
    cdef object type_args
    cdef object list_element
    cdef object type_arg
    cdef int result = 0
    cdef int tmp = 0
    cdef int ttype

    if hasattr(type_obj, '__args__'):

        type_args = getattr(type_obj, '__args__')

        for list_element in obj:
            tmp = element_check(list_element, type_obj)
            if tmp == -2:
                return 1
            result += tmp

        return result >= len(obj)
    else:
        return isinstance(obj, list)
