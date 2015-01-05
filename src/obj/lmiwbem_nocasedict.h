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

#ifndef   LMIWBEM_NOCASEDICT_H
#  define LMIWBEM_NOCASEDICT_H

#  include <map>
#  include <boost/python/class.hpp>
#  include <boost/python/object.hpp>
#  include "lmiwbem.h"
#  include "lmiwbem_cimbase.h"
#  include "util/lmiwbem_convert.h"
#  include "util/lmiwbem_string.h"

namespace bp = boost::python;

BOOST_PYTHON_BEGIN
class list;
class dict;
BOOST_PYTHON_END

class NocaseDictComparator;

typedef std::map <String, bp::object, NocaseDictComparator> nocase_map_t;

class NocaseDictComparator
{
public:
    bool operator ()(const String &a, const String &b) const;
};

class NocaseDict: public CIMBase<NocaseDict>
{
public:
    NocaseDict();
    NocaseDict(const bp::object &d);

    static void init_type();
    static bp::object create();
    static bp::object create(const bp::object &d);

    nocase_map_t::iterator begin();
    nocase_map_t::iterator end();
    nocase_map_t::const_iterator begin() const;
    nocase_map_t::const_iterator end() const;

    bool empty();

    void delitem(const bp::object &key);
    void setitem(const bp::object &key, const bp::object &value);
    bp::object getitem(const bp::object &key);

    String repr();

    bp::list keys();
    bp::list values();
    bp::list items();
    bp::object iterkeys();
    bp::object itervalues();
    bp::object iteritems();

    bp::object haskey(const bp::object &key) const;
    bp::object len() const;

    void update(const bp::object &d);
    void clear();

    bp::object get(
        const bp::object &key,
        const bp::object &def);
    bp::object pop(
        const bp::object &key,
        const bp::object &def);

    bp::object copy();

#  if PY_MAJOR_VERSION < 3
    int cmp(const bp::object &other);
#  else
    bool eq(const bp::object &other);
    bool gt(const bp::object &other);
    bool lt(const bp::object &other);
    bool ge(const bp::object &other);
    bool le(const bp::object &other);
#  endif // PY_MAJOR_VERSION

private:
    nocase_map_t m_dict;
};

class NocaseDictIterator
{
public:
    virtual bp::object iter() = 0;
    virtual bp::object next() = 0;

protected:
    template <typename T>
    static void init_type(const char *classname)
    {
        CIMBase<T>::init_type(bp::class_<T>(
            classname, bp::init<>())
            .def("__iter__", &T::iter)
#  if PY_MAJOR_VERSION < 3
            .def("next", &T::next)
#  else
            .def("__next__", &T::next)
#  endif // PY_MAJOR_VERSION
        );
    }

    template <typename T>
    static bp::object create(const nocase_map_t &dict)
    {
        bp::object inst = CIMBase<T>::create();
        T &fake_this = Conv::as<T&>(inst);
        fake_this.m_dict = dict;
        fake_this.m_iter = fake_this.m_dict.begin();
        return inst;
    }

    nocase_map_t m_dict;
    nocase_map_t::const_iterator m_iter;
};

class NocaseDictKeyIterator:
    public CIMBase<NocaseDictKeyIterator>,
    public NocaseDictIterator
{
public:
    static void init_type();
    static bp::object create(const nocase_map_t &dict);

    virtual bp::object iter();
    virtual bp::object next();

private:
    friend class NocaseDictIterator;
};

class NocaseDictValueIterator:
    public CIMBase<NocaseDictValueIterator>,
    public NocaseDictIterator
{
public:
    static void init_type();
    static bp::object create(const nocase_map_t &dict);

    virtual bp::object iter();
    virtual bp::object next();

private:
    friend class NocaseDictIterator;
};

class NocaseDictItemIterator:
    public CIMBase<NocaseDictItemIterator>,
    public NocaseDictIterator
{
public:
    static void init_type();
    static bp::object create(const nocase_map_t &dict);

    virtual bp::object iter();
    virtual bp::object next();

private:
    friend class NocaseDictIterator;
};

#endif // LMIWBEM_NOCASEDICT_H
