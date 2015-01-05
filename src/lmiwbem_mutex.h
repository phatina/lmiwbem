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

#ifndef   LMIWBEM_MUTEX_H
#  define LMIWBEM_MUTEX_H

extern "C" {
#  include <pthread.h>
}

class Mutex
{
public:
    Mutex();
    ~Mutex();

    bool lock();
    bool unlock();
    bool isLocked() const;

private:
    bool m_good;
    bool m_locked;
    pthread_mutex_t m_mutex;
};

class ScopedMutex
{
public:
    ScopedMutex(Mutex &m);
    ~ScopedMutex();

    bool lock();
    bool unlock();
    bool isLocked() const;

private:
    Mutex &m_mutex;
};

#endif // LMIWBEM_MUTEX_H
