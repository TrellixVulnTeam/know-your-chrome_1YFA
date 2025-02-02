/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WTF_SHA1_h
#define WTF_SHA1_h

#include <wtf/Vector.h>
#include <wtf/text/CString.h>

namespace WTF {

class SHA1 {
public:
    SHA1();

    void addBytes(const Vector<uint8_t>& input)
    {
        addBytes(input.data(), input.size());
    }
    void addBytes(const CString& input)
    {
        const char* string = input.data();
        // Make sure that the creator of the CString didn't make the mistake
        // of forcing length() to be the size of the buffer used to create the
        // string, prior to inserting the null terminator earlier in the
        // sequence.
        ASSERT(input.length() == strlen(string));
        addBytes(reinterpret_cast<const uint8_t*>(string), input.length());
    }
    void addBytes(const uint8_t* input, size_t length);

    // computeHash has a side effect of resetting the state of the object.
    void computeHash(Vector<uint8_t, 20>&);
    
    // Get a hex hash from the digest. Pass a limit less than 40 if you want a shorter digest.
    static CString hexDigest(const Vector<uint8_t, 20>&);
    
    // Compute the hex digest directly. Pass a limit less than 40 if you want a shorter digest.
    CString computeHexDigest();
    
private:
    void finalize();
    void processBlock();
    void reset();

    uint8_t m_buffer[64];
    size_t m_cursor; // Number of bytes filled in m_buffer (0-64).
    uint64_t m_totalBytes; // Number of bytes added so far.
    uint32_t m_hash[5];
};

} // namespace WTF

using WTF::SHA1;

#endif // WTF_SHA1_h
