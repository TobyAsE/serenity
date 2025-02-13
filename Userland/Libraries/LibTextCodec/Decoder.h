/*
 * Copyright (c) 2020-2021, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <AK/Forward.h>

namespace TextCodec {

class Decoder {
public:
    virtual String to_utf8(const StringView&) = 0;

protected:
    virtual ~Decoder() = default;
};

class UTF8Decoder final : public Decoder {
public:
    virtual String to_utf8(const StringView&) override;
};

class UTF16BEDecoder final : public Decoder {
public:
    virtual String to_utf8(const StringView&) override;
};

class Latin1Decoder final : public Decoder {
public:
    virtual String to_utf8(const StringView&) override;
};

class Latin2Decoder final : public Decoder {
public:
    virtual String to_utf8(const StringView&) override;
};

class HebrewDecoder final : public Decoder {
public:
    virtual String to_utf8(const StringView&) override;
};

Decoder* decoder_for(const String& encoding);
String get_standardized_encoding(const String& encoding);
bool is_standardized_encoding(const String& encoding);

}
