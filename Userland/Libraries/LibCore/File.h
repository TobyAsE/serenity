/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
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

#include <AK/OSError.h>
#include <AK/Result.h>
#include <AK/String.h>
#include <LibCore/IODevice.h>
#include <sys/stat.h>

namespace Core {

class File final : public IODevice {
    C_OBJECT(File)
public:
    virtual ~File() override;

    static Result<NonnullRefPtr<File>, String> open(String filename, IODevice::OpenMode, mode_t = 0644);

    String filename() const { return m_filename; }
    void set_filename(const String filename) { m_filename = move(filename); }

    bool is_directory() const;
    static bool is_directory(const String& filename);

    bool is_device() const;
    static bool is_device(const String& filename);

    static bool exists(const String& filename);
    static bool ensure_parent_directories(const String& path);

    enum class RecursionMode {
        Allowed,
        Disallowed
    };

    enum class LinkMode {
        Allowed,
        Disallowed
    };

    enum class AddDuplicateFileMarker {
        Yes,
        No,
    };

    struct CopyError {
        OSError error_code;
        bool tried_recursing;
    };

    static Result<void, CopyError> copy_file(const String& dst_path, const struct stat& src_stat, File& source);
    static Result<void, CopyError> copy_directory(const String& dst_path, const String& src_path, const struct stat& src_stat, LinkMode = LinkMode::Disallowed);
    static Result<void, CopyError> copy_file_or_directory(const String& dst_path, const String& src_path, RecursionMode = RecursionMode::Allowed, LinkMode = LinkMode::Disallowed, AddDuplicateFileMarker = AddDuplicateFileMarker::Yes);

    static String real_path_for(const String& filename);
    static String read_link(String const& link_path);
    static Result<void, OSError> link_file(const String& dst_path, const String& src_path);

    struct RemoveError {
        String file;
        OSError error_code;
    };
    static Result<void, RemoveError> remove(const String& path, RecursionMode, bool force);

    virtual bool open(IODevice::OpenMode) override;

    enum class ShouldCloseFileDescriptor {
        No = 0,
        Yes
    };
    bool open(int fd, IODevice::OpenMode, ShouldCloseFileDescriptor);

    static NonnullRefPtr<File> standard_input();
    static NonnullRefPtr<File> standard_output();
    static NonnullRefPtr<File> standard_error();

private:
    File(Object* parent = nullptr)
        : IODevice(parent)
    {
    }
    explicit File(String filename, Object* parent = nullptr);

    bool open_impl(IODevice::OpenMode, mode_t);

    String m_filename;
    ShouldCloseFileDescriptor m_should_close_file_descriptor { ShouldCloseFileDescriptor::Yes };
};

}
