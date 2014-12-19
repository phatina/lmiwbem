/* ***** BEGIN LICENSE BLOCK *****
 *
 *   Copyright (C) 2014, Peter Hatina <phatina@redhat.com>
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation, either version 2.1 of the
 *   License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *   MA 02110-1301 USA
 *
 * ***** END LICENSE BLOCK ***** */

#include <config.h>
#include <boost/python/list.hpp>
#include <boost/python/str.hpp>
#include <boost/python/object_attributes.hpp>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMDateTime.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObject.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMValue.h>
#include "obj/cim/lmiwbem_class.h"
#include "obj/cim/lmiwbem_class_name.h"
#include "obj/cim/lmiwbem_instance.h"
#include "obj/cim/lmiwbem_instance_name.h"
#include "obj/cim/lmiwbem_types.h"
#include "obj/cim/lmiwbem_value.h"
#include "util/lmiwbem_convert.h"
#include "util/lmiwbem_util.h"

namespace {

template <typename T>
bp::object getPegasusValueCore(const T &value)
{
    return incref(bp::object(value));
}

template <>
bp::object getPegasusValueCore<Pegasus::CIMObject>(const Pegasus::CIMObject &value)
{
    return incref(CIMInstance::create(value));
}

template<>
bp::object getPegasusValueCore<Pegasus::CIMInstance>(const Pegasus::CIMInstance &value)
{
    return incref(CIMInstance::create(value));
}

// We could not use to_python converters here.
#define DEF_PEGASUS_VALUE_CORE(type) \
    template<> \
    bp::object getPegasusValueCore(const Pegasus::type &value) \
    { \
        return incref(type::create(value)); \
    }

DEF_PEGASUS_VALUE_CORE(Uint8)
DEF_PEGASUS_VALUE_CORE(Sint8)
DEF_PEGASUS_VALUE_CORE(Uint16)
DEF_PEGASUS_VALUE_CORE(Sint16)
DEF_PEGASUS_VALUE_CORE(Uint32)
DEF_PEGASUS_VALUE_CORE(Sint32)
DEF_PEGASUS_VALUE_CORE(Uint64)
DEF_PEGASUS_VALUE_CORE(Sint64)
DEF_PEGASUS_VALUE_CORE(Real32)
DEF_PEGASUS_VALUE_CORE(Real64)

template <typename T>
bp::object getPegasusValue(const Pegasus::CIMValue &value)
{
    // We return None for every empty CIMValue except of lists,
    // so we can reconstruct CIMValue out of Python's objects.
    if (value.isNull() && !value.isArray())
        return None;

    if (!value.isArray()) {
        T raw_value;
        value.get(raw_value);
        bp::object obj(getPegasusValueCore<T>(raw_value));
        return incref(obj);
    } else {
        bp::list py_array;
        Pegasus::Array<T> raw_array;
        value.get(raw_array);
        const Pegasus::Uint32 cnt = value.getArraySize();
        for (Pegasus::Uint32 i = 0; i < cnt; ++i) {
            const T &raw_value = raw_array[i];
            py_array.append(getPegasusValueCore<T>(raw_value));
        }
        return incref(py_array);
    }
}

template <typename T, typename R>
R setPegasusValueCore(const bp::object &value)
{
    return R(Conv::detail::extract<T>(value)());
}

template <>
Pegasus::CIMDateTime setPegasusValueCore<
    Pegasus::CIMDateTime,
    Pegasus::CIMDateTime>(const bp::object &value)
{
    return Pegasus::CIMDateTime(ObjectConv::asString(value));
}

template <>
Pegasus::CIMObjectPath setPegasusValueCore<
    Pegasus::CIMObjectPath,
    Pegasus::CIMObjectPath>(const bp::object &value)
{
    const CIMInstanceName &cim_path = CIMInstanceName::asNative(value);
    return cim_path.asPegasusCIMObjectPath();
}

template <>
Pegasus::CIMObject setPegasusValueCore<
    Pegasus::CIMClass,
    Pegasus::CIMObject>(const bp::object &value)
{
    CIMClass &cim_class = CIMClass::asNative(value);
    return Pegasus::CIMObject(cim_class.asPegasusCIMClass());
}

template <>
Pegasus::CIMObject setPegasusValueCore<
    Pegasus::CIMInstance,
    Pegasus::CIMObject>(const bp::object &value)
{
    CIMInstance &cim_instance = CIMInstance::asNative(value);
    return Pegasus::CIMObject(cim_instance.asPegasusCIMInstance());
}

template <>
Pegasus::String setPegasusValueCore<
    Pegasus::String,
    Pegasus::String>(const bp::object &value)
{
    return StringConv::asString(value);
}

template <typename T, typename R>
Pegasus::CIMValue setPegasusValue(
    const bp::object &value,
    const bool is_array = false)
{
    if (!is_array)
        return Pegasus::CIMValue(setPegasusValueCore<T, R>(value));

    bp::list py_list(value);
    Pegasus::Array<R> array;
    const int cnt = bp::len(py_list);
    for (int i = 0; i < cnt; ++i)
        array.append(setPegasusValueCore<T, R>(py_list[i]));

    return Pegasus::CIMValue(array);
}

// We are compiling under C++98, which does not support template's
// default parameters. So we define the same template function with
// "S" suffix; for shorter code used below.
template <typename T>
Pegasus::CIMValue setPegasusValueS(
    const bp::object &value,
    const bool is_array = false)
{
    return setPegasusValue<T, T>(value, is_array);
}

} // unnamed namespace

bp::object CIMValue::asLMIWbemCIMValue(const Pegasus::CIMValue &value)
{
    switch (value.getType()) {
    case Pegasus::CIMTYPE_BOOLEAN:
        return getPegasusValue<Pegasus::Boolean>(value);
    case Pegasus::CIMTYPE_UINT8:
        return getPegasusValue<Pegasus::Uint8>(value);
    case Pegasus::CIMTYPE_SINT8:
        return getPegasusValue<Pegasus::Sint8>(value);
    case Pegasus::CIMTYPE_UINT16:
        return getPegasusValue<Pegasus::Uint16>(value);
    case Pegasus::CIMTYPE_SINT16:
        return getPegasusValue<Pegasus::Sint16>(value);
    case Pegasus::CIMTYPE_UINT32:
        return getPegasusValue<Pegasus::Uint32>(value);
    case Pegasus::CIMTYPE_SINT32:
        return getPegasusValue<Pegasus::Sint32>(value);
    case Pegasus::CIMTYPE_UINT64:
        return getPegasusValue<Pegasus::Uint64>(value);
    case Pegasus::CIMTYPE_SINT64:
        return getPegasusValue<Pegasus::Sint64>(value);
    case Pegasus::CIMTYPE_REAL32:
        return getPegasusValue<Pegasus::Real32>(value);
    case Pegasus::CIMTYPE_REAL64:
        return getPegasusValue<Pegasus::Real64>(value);
    case Pegasus::CIMTYPE_CHAR16:
        return getPegasusValue<Pegasus::Char16>(value);
    case Pegasus::CIMTYPE_STRING:
        return getPegasusValue<Pegasus::String>(value);
    case Pegasus::CIMTYPE_DATETIME:
        return getPegasusValue<Pegasus::CIMDateTime>(value);
    case Pegasus::CIMTYPE_REFERENCE:
        return getPegasusValue<Pegasus::CIMObjectPath>(value);
    case Pegasus::CIMTYPE_OBJECT:
        return getPegasusValue<Pegasus::CIMObject>(value);
    case Pegasus::CIMTYPE_INSTANCE:
        return getPegasusValue<Pegasus::CIMInstance>(value);
    default:
        // As we have covered all CIM types, we should never get here.
        LMIWBEM_UNREACHABLE(assert(false && "Unknown CIM type value passed"));
        return None;
    }
}

Pegasus::CIMValue CIMValue::asPegasusCIMValue(
    const bp::object &value,
    const String &def_type)
{
    bool is_array = isarray(value);
    if (isnone(value) || (is_array && bp::len(value) == 0))
        return Pegasus::CIMValue(CIMTypeConv::asCIMType(def_type), true);

    bp::object py_value_type_check = is_array ? value[0] : value;

    if (isinstance(py_value_type_check, CIMType::type())) {
        String c_type = StringConv::asString(py_value_type_check.attr("cimtype"));
        if (c_type == "uint8")
            return setPegasusValueS<Pegasus::Uint8>(value, is_array);
        else if (c_type == "sint8")
            return setPegasusValueS<Pegasus::Sint8>(value, is_array);
        else if (c_type == "uint16")
            return setPegasusValueS<Pegasus::Uint16>(value, is_array);
        else if (c_type == "sint16")
            return setPegasusValueS<Pegasus::Sint16>(value, is_array);
        else if (c_type == "uint32")
            return setPegasusValueS<Pegasus::Uint32>(value, is_array);
        else if (c_type == "sint32")
            return setPegasusValueS<Pegasus::Sint32>(value, is_array);
        else if (c_type == "uint64")
            return setPegasusValueS<Pegasus::Uint64>(value, is_array);
        else if (c_type == "sint64")
            return setPegasusValueS<Pegasus::Sint64>(value, is_array);
        else if (c_type == "real32")
            return setPegasusValueS<Pegasus::Real32>(value, is_array);
        else if (c_type == "real64")
            return setPegasusValueS<Pegasus::Real32>(value, is_array);
        else if (c_type == "datetime")
            return setPegasusValueS<Pegasus::CIMDateTime>(value, is_array);
    } else if (isinstance(py_value_type_check, CIMInstance::type())) {
        return setPegasusValue<Pegasus::CIMInstance, Pegasus::CIMObject>(value, is_array);
    } else if (isinstance(py_value_type_check, CIMClass::type())) {
        return setPegasusValue<Pegasus::CIMClass, Pegasus::CIMObject>(value, is_array);
    } else if (isinstance(py_value_type_check, CIMInstanceName::type())) {
        return setPegasusValueS<Pegasus::CIMObjectPath>(value, is_array);
    } else if (isinstance(py_value_type_check, CIMClassName::type())) {
        throw_TypeError("CIMClassName: Unsupported TOG-Pegasus type");
    } else if (isbasestring(py_value_type_check)) {
        return setPegasusValueS<Pegasus::String>(value, is_array);
    } else if (isbool(py_value_type_check)) {
        return setPegasusValueS<bool>(value, is_array);
#  if PY_MAJOR_VERSION < 3
    } else if (isint(py_value_type_check)) {
        if (sizeof(long) == sizeof(Pegasus::Sint32)) {
            return setPegasusValueS<Pegasus::Sint32>(value, is_array);
        }
        return setPegasusValueS<Pegasus::Sint64>(value, is_array);
#  endif // PY_MAJOR_VERSION
    } else if (islong(py_value_type_check)) {
        return setPegasusValueS<Pegasus::Sint64>(value, is_array);
    } else if (isfloat(py_value_type_check)) {
        return setPegasusValueS<float>(value, is_array);
    }

    throw_TypeError("CIMValue: Unsupported TOG-Pegasus type");
    return Pegasus::CIMValue();
}
