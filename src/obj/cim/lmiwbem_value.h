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

#ifndef   LMIWBEM_VALUE_H
#  define LMIWBEM_VALUE_H

#  include <boost/python/object.hpp>
#  include "lmiwbem.h"
#  include "util/lmiwbem_string.h"

PEGASUS_BEGIN
class CIMValue;
PEGASUS_END

class CIMValue
{
public:
    static bp::object asLMIWbemCIMValue(
        const Pegasus::CIMValue &value);
    static Pegasus::CIMValue asPegasusCIMValue(
        const bp::object &value,
        const String &def_type = String());
    static String asString(const Pegasus::CIMValue &value);
};

#endif // LMIWBEM_VALUE_H
