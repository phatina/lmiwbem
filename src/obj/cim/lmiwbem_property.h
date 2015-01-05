/* ***** BEGIN LICENSE BLOCK *****
 *
 *   Copyright (C) 2014-2015, Peter Hatina <phatina@redhat.com>
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

#ifndef   LMIWBEM_PROPERTY_H
#  define LMIWBEM_PROPERTY_H

#  include <list>
#  include <boost/python/object.hpp>
#  include "lmiwbem.h"
#  include "lmiwbem_refcountedptr.h"
#  include "obj/lmiwbem_cimbase.h"
#  include "util/lmiwbem_string.h"

BOOST_PYTHON_BEGIN
class CIMConstQualifier;
class CIMProperty;
class CIMType;
class CIMValue;
BOOST_PYTHON_END

namespace bp = boost::python;

class CIMProperty: public CIMBase<CIMProperty>
{
public:
    CIMProperty();
    CIMProperty(
        const bp::object &name,
        const bp::object &value,
        const bp::object &type,
        const bp::object &class_origin,
        const bp::object &array_size,
        const bp::object &propagated,
        const bp::object &qualifiers,
        const bp::object &is_array,
        const bp::object &reference_class);

    static void init_type();
    static bp::object create(const Pegasus::CIMConstProperty &property);
    static bp::object create(
        const bp::object &name,
        const bp::object &value);

    Pegasus::CIMProperty asPegasusCIMProperty();

#  if PY_MAJOR_VERSION < 3
    int cmp(const bp::object &other);
#  else
    bool eq(const bp::object &other);
    bool gt(const bp::object &other);
    bool lt(const bp::object &other);
    bool ge(const bp::object &other);
    bool le(const bp::object &other);
#  endif // PY_MAJOR_VERSION

    bp::object repr();

    bp::object copy();

    String getName() const;
    String getType() const;
    String getClassOrigin() const;
    String getReferenceClass() const;
    int getArraySize() const;
    bool getIsArray() const;
    bool getIsPropagated() const;
    bp::object getPyName() const;
    bp::object getPyType() const;
    bp::object getPyClassOrigin() const;
    bp::object getPyReferenceClass() const;
    bp::object getPyArraySize() const;
    bp::object getPyIsArray() const;
    bp::object getPyIsPropagated() const;
    bp::object getPyValue();
    bp::object getPyQualifiers();

    void setName(const String &name);
    void setType(const String &type);
    void setClassOrigin(const String &class_origin);
    void setReferenceClass(const String &reference_class);
    void setArraySize(int array_size);
    void setIsArray(bool is_array);
    void setIsPropagated(bool is_propagated);
    void setPyName(const bp::object &name);
    void setPyType(const bp::object &type);
    void setPyValue(const bp::object &value);
    void setPyClassOrigin(const bp::object &class_origin);
    void setPyReferenceClass(const bp::object &reference_class);
    void setPyArraySize(const bp::object &array_size);
    void setPyIsArray(const bp::object &is_array);
    void setPyIsPropagated(const bp::object &propagated);
    void setPyQualifiers(const bp::object &qualifiers);

private:
    static String propertyTypeAsString(const Pegasus::CIMType type);

    String m_name;
    String m_type;
    String m_class_origin;
    String m_reference_class;
    bool m_is_array;
    bool m_is_propagated;
    int m_array_size;
    bp::object m_value;
    bp::object m_qualifiers;

    RefCountedPtr<Pegasus::CIMValue> m_rc_prop_value;
    RefCountedPtr<std::list<Pegasus::CIMConstQualifier> > m_rc_prop_qualifiers;
};

#endif // LMIWBEM_PROPERTY_H
