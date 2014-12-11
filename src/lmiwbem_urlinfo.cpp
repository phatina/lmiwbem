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
#include <cerrno>
#include <sstream>
#include "lmiwbem_urlinfo.h"

URLInfo::URLInfo()
    : m_hostname("unknown")
    , m_port(URLInfo::DEF_HTTPS_PORT)
    , m_is_https(true)
{
}

URLInfo::URLInfo(const URLInfo &copy)
    : m_hostname(copy.m_hostname)
    , m_port(copy.m_port)
    , m_is_https(copy.m_is_https)
{
}

bool URLInfo::set(String url)
{
    if (url.substr(0, 7) == "http://") {
        url.erase(0, 7);
        m_port = URLInfo::DEF_HTTP_PORT;
        m_is_https = false;
    } else if (url.substr(0, 8) == "https://") {
        url.erase(0, 8);
        m_port = URLInfo::DEF_HTTPS_PORT;
        m_is_https = true;
    } else {
        return false;
    }

    size_t pos = url.rfind(':');
    if (pos != String::npos) {
        m_hostname = url.substr(0, pos);
        long int port = strtol(url.substr(pos + 1,
            url.size() - pos - 1).c_str(), NULL, 10);
        if (errno == ERANGE || port < 0 || port > 65535)
            return false;
        m_port = static_cast<uint32_t>(port);
    } else {
        m_hostname = url;
    }

    return true;
}

String URLInfo::asString() const
{
    std::stringstream ss;

    if (m_is_https)
        ss << "https://";
    else
        ss << "http://";

    ss << m_hostname << ':' << m_port;

    return String(ss.str());
}

URLInfo &URLInfo::operator =(const URLInfo &rhs)
{
    m_hostname = rhs.m_hostname;
    m_port = rhs.m_port;
    m_is_https = rhs.m_is_https;
    return *this;
}
