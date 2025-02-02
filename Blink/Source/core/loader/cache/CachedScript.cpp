/*
    Copyright (C) 1998 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2001 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Waldo Bastian (bastian@kde.org)
    Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
    Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

    This class provides all functionality needed for loading images, style sheets and html
    pages from the web. It has a memory cache for these objects.
*/

#include "config.h"
#include "core/loader/cache/CachedScript.h"

#include "core/dom/WebCoreMemoryInstrumentation.h"
#include "core/loader/TextResourceDecoder.h"
#include "core/loader/cache/CachedResourceClient.h"
#include "core/loader/cache/CachedResourceClientWalker.h"
#include "core/loader/cache/MemoryCache.h"
#include "core/platform/MIMETypeRegistry.h"
#include "core/platform/SharedBuffer.h"
#include "core/platform/network/HTTPParsers.h"
#include <wtf/Vector.h>

namespace WebCore {

CachedScript::CachedScript(const ResourceRequest& resourceRequest, const String& charset)
    : CachedResource(resourceRequest, Script)
    , m_decoder(TextResourceDecoder::create("application/javascript", charset))
{
    // It's javascript we want.
    // But some websites think their scripts are <some wrong mimetype here>
    // and refuse to serve them if we only accept application/x-javascript.
    setAccept("*/*");
}

CachedScript::~CachedScript()
{
}

void CachedScript::setEncoding(const String& chs)
{
    m_decoder->setEncoding(chs, TextResourceDecoder::EncodingFromHTTPHeader);
}

String CachedScript::encoding() const
{
    return m_decoder->encoding().name();
}

String CachedScript::mimeType() const
{
    return extractMIMETypeFromMediaType(m_response.httpHeaderField("Content-Type")).lower();
}

const String& CachedScript::script()
{
    ASSERT(!isPurgeable());

    if (!m_script && m_data) {
        m_script = m_decoder->decode(m_data->data(), encodedSize());
        m_script.append(m_decoder->flush());
        setDecodedSize(m_script.sizeInBytes());
    }
    m_decodedDataDeletionTimer.startOneShot(0);
    
    return m_script;
}

void CachedScript::destroyDecodedData()
{
    m_script = String();
    setDecodedSize(0);
    if (!MemoryCache::shouldMakeResourcePurgeableOnEviction() && isSafeToMakePurgeable())
        makePurgeable(true);
}

bool CachedScript::mimeTypeAllowedByNosniff() const
{
    return !parseContentTypeOptionsHeader(m_response.httpHeaderField("X-Content-Type-Options")) == ContentTypeOptionsNosniff || MIMETypeRegistry::isSupportedJavaScriptMIMEType(mimeType());
}

void CachedScript::reportMemoryUsage(MemoryObjectInfo* memoryObjectInfo) const
{
    MemoryClassInfo info(memoryObjectInfo, this, WebCoreMemoryTypes::CachedResourceScript);
    CachedResource::reportMemoryUsage(memoryObjectInfo);
    info.addMember(m_script, "script");
    info.addMember(m_decoder, "decoder");
}

} // namespace WebCore
