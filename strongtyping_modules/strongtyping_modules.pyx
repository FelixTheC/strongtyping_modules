#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@created: 16.08.20
@author: felix
"""
from __future__ import print_function


cdef int not_supported = -999


cdef int matches_origin(object obj, object type_obj):
    cdef object type_origin
    cdef str type_origin_name

    type_origin = getattr(type_obj, '__origin__')

    if hasattr(type_origin, '_name'):
        type_origin_name = type_origin._name

        if type_origin_name == 'Union' or type_origin_name == 'Any':
            return 1

    return isinstance(obj, type_origin)


cdef int which_subtype(object element):
    cdef int sub_type = 0;
    cdef str element_name

    if hasattr(element, 'is_typed_dict'):
        sub_type = 30
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
        if element_name == 'Literal':
            sub_type = 5
        if element_name == 'Any':
            sub_type = -2
        if element_name is None:
            if hasattr(element, '__origin__'):
                if hasattr(element.__origin__, '_name'):
                    if element.__origin__._name == 'Union':
                        sub_type = -1
                    if element.__origin__._name == 'Any':
                        return -2
                    if element.__origin__._name == 'Literal':
                        return 5

    if hasattr(element, '__str__'):
        try:
            if element.__str__() == 'Ellipsis':
                return 6
        except TypeError:
            return sub_type
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
        result += dict_elements(obj, type_obj)
    if subtype == 4:
        result += set_elements(obj, type_obj)
    if subtype == 5:
        result += literal_elements(obj, type_obj)
    if subtype == 30:
        result += typeddict_element(obj, type_obj)

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
            elif ttype == not_supported:
                return not_supported
            else:
                result += sub_type_result(obj, type_arg, ttype)
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
        elif ttype == not_supported:
            return not_supported
        else:
            result += sub_type_result(obj, type_arg, ttype)
    return result


cpdef int dict_elements(object obj, object type_obj):
    cdef object type_args
    cdef object key_type_args
    cdef object type_key_type_args
    cdef object value_type_args
    cdef object type_value_type_args

    cdef object key_element
    cdef object value_element

    cdef int key_result = 0
    cdef int value_result = 0

    cdef int obj_len = len(obj)

    if hasattr(type_obj, '__args__'):

        if matches_origin(obj, type_obj) == 0:
            return 0

        type_args = getattr(type_obj, '__args__')
        key_type_args = type_args[0]
        value_type_args = type_args[1]
        type_key_type_args = which_subtype(key_type_args)
        type_value_type_args = which_subtype(value_type_args)

        for key_element in obj.keys():
            if hasattr(key_type_args, '__args__'):
                key_result += sub_type_result(key_element, key_type_args, type_key_type_args)
            else:
                key_result += isinstance(key_element, key_type_args)

        for value_element in obj.values():
            if hasattr(value_type_args, '__args__'):
                value_result += sub_type_result(value_element, value_type_args, type_value_type_args)
            else:
                value_result += isinstance(value_element, value_type_args)

        return key_result >= obj_len and value_result >= obj_len
    else:
        return isinstance(obj, type_obj)


cpdef int set_elements(object obj, object type_obj):
    cdef object set_element
    cdef int result = 0
    cdef int tmp = 0
    cdef int ttype

    if hasattr(type_obj, '__args__'):

        if matches_origin(obj, type_obj) == 0:
            return 0

        for set_element in obj:
            tmp = element_check(set_element, type_obj)
            if tmp == -2:
                return 1
            elif tmp == not_supported:
                return not_supported
            result += tmp

        return result >= len(obj)
    else:
        return isinstance(obj, type_obj)


cdef int ellipsis_inside(object type_obj):
    cdef object element
    cdef int result

    for element in type_obj:
        result = which_subtype(element)
        if result == 6:
            return 6
    return 0

cdef int validate_tuple_elements(object tuple_element, object type_arg):
    ttype = which_subtype(type_arg)
    if ttype == 0:
        return isinstance(tuple_element, type_arg)
    elif ttype == -2:
        return 1
    elif ttype == not_supported:
        return not_supported
    else:
        return sub_type_result(tuple_element, type_arg, ttype)

cpdef int tuple_elements(object obj, object type_obj):
    cdef object type_args
    cdef object tuple_element
    cdef object type_arg
    cdef int result = 0
    cdef int ttype
    cdef int tmp
    if hasattr(type_obj, '__args__'):

        if matches_origin(obj, type_obj) == 0:
            return 0

        type_args = getattr(type_obj, '__args__')

        if isinstance(obj, tuple):
            if ellipsis_inside(type_args) == 6:
                for tuple_element in obj:
                    result += validate_tuple_elements(tuple_element, type_args[0])
                return result >= len(obj)
            elif len(obj) == len(type_args):
                for tuple_element, type_arg in zip(obj, type_args):
                    result += validate_tuple_elements(tuple_element, type_arg)
                return result >= len(obj)
        else:
            return 0
    else:
        for tuple_element, type_arg in zip(obj, type_obj):
            result += validate_tuple_elements(tuple_element, type_arg)
        return result >= len(obj)


cpdef int list_elements(object obj, object type_obj):
    cdef object list_element
    cdef int result = 0
    cdef int tmp = 0
    cdef int ttype

    if hasattr(type_obj, '__args__'):

        if matches_origin(obj, type_obj) == 0:
            return 0

        for list_element in obj:
            tmp = element_check(list_element, type_obj)
            if tmp == -2:
                return 1
            elif tmp == not_supported:
                return not_supported
            else:
                result += tmp
        return result >= len(obj)
    else:
        return isinstance(obj, type_obj)


cpdef int literal_elements(object obj, object type_obj):
    cdef object type_args

    if hasattr(type_obj, '__args__'):
        type_args = getattr(type_obj, '__args__')
        return obj in type_args
    else:
        return 0


cpdef int typeddict_element(object obj, object type_obj):
    cdef int total = 0
    cdef int result = 0
    cdef int ttype = 0
    cdef dict required_fields = getattr(type_obj, '__annotations__')

    for field in required_fields.keys():
        ttype = which_subtype(required_fields[field])
        if ttype == 0:
            result = isinstance(obj[field], required_fields[field])
        else:
            result = sub_type_result(obj[field], required_fields[field], ttype)
        if result == -2:
            return -2
        total += result
    return total
