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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include "lmiwbem_urlinfo.h"

namespace {
    const unsigned int BUFLEN = 1024;

    Pegasus::String get_fqdn() {
        struct addrinfo hints, *info, *p;
        int gai_result;
        char name[BUFLEN + 1];
        char port[BUFLEN];

        if (gethostname(name, BUFLEN) < 0) {
            return Pegasus::String("localhost");
        }

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;

        snprintf(port, BUFLEN, "%d", URLInfo::DEF_HTTPS_PORT);
        if ((gai_result = getaddrinfo(name, port, &hints, &info)) == 0) {
            for (p = info; p != NULL; p = p->ai_next) {
                if (p->ai_canonname != NULL) {
                    snprintf(name, BUFLEN, p->ai_canonname);
                    break;
                }
            }
            freeaddrinfo(info);
        }
        return Pegasus::String(name);
    }
}

URLInfo::URLInfo()
    : m_url("https://unknown")
    , m_hostname("unknown")
    , m_port(URLInfo::DEF_HTTPS_PORT)
    , m_is_https(true)
    , m_is_local(false)
{
}

URLInfo::URLInfo(const URLInfo &copy)
    : m_url(copy.m_url)
    , m_hostname(copy.m_hostname)
    , m_port(copy.m_port)
    , m_is_https(copy.m_is_https)
    , m_is_local(copy.m_is_local)
{
}

bool URLInfo::set(Pegasus::String url)
{
    m_url = url;
    /* Handle local connection */
    if (url.subString(0, 7) == "file://" ||
            url == "localhost" ||
            url == "localhost.localdomain" ||
            url == "localhost4" ||
            url == "localhost4.localdomain4" ||
            url == "localhost6" ||
            url == "localhost6.localdomain6" ||
            url == "127.0.0.1" || url == "::1") {
        m_is_https = false;
        m_is_local = true;
        m_hostname = get_fqdn();
        m_port = 0;
        return true;
    }
    /* Handle remote connection */
    if (url.subString(0, 7) == "http://") {
        url.remove(0, 7);
        m_port = URLInfo::DEF_HTTP_PORT;
        m_is_https = false;
    } else if (url.subString(0, 8) == "https://") {
        url.remove(0, 8);
        m_port = URLInfo::DEF_HTTPS_PORT;
        m_is_https = true;
    } else {
        return false;
    }

    Pegasus::Uint32 pos = url.reverseFind(':');
    if (pos != Pegasus::PEG_NOT_FOUND) {
        m_hostname = url.subString(0, pos);
        long int port = strtol(url.subString(pos + 1,
            url.size() - pos - 1).getCString(), NULL, 10);
        if (errno == ERANGE || port < 0 || port > 65535)
            return false;
        m_port = static_cast<Pegasus::Uint32>(port);
    } else {
        m_hostname = url;
    }

    return true;
}

std::string URLInfo::asStdString() const
{
    std::stringstream ss;

    if (m_is_https)
        ss << "https://";
    else
        ss << "http://";

    ss << m_hostname << ':' << m_port;

    return ss.str();
}

URLInfo &URLInfo::operator =(const URLInfo &rhs)
{
    m_hostname = rhs.m_hostname;
    m_port = rhs.m_port;
    m_is_https = rhs.m_is_https;
    return *this;
}
