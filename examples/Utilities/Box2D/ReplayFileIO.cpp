/*
  ==============================================================================

   This file is part of the JUCE framework examples.
   Copyright (c) Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   to use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
   REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
   AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
   INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
   LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
   OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
   PERFORMANCE OF THIS SOFTWARE.

  ==============================================================================
*/

#include "Box2DSamples.h"
#include "ReplayFileIO.h"

namespace Box2DSamples
{

ReplayFileReadResult readReplayFile (const URL& url)
{
    ReplayFileReadResult result;
    result.displayName = url.getFileName();

    auto stream = url.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress));

    if (stream == nullptr)
    {
        result.error = ReplayFileError::cannotOpenForReading;
        return result;
    }

    const auto expectedNumBytes = stream->getTotalLength();
    const auto numBytesRead = stream->readIntoMemoryBlock (result.data);

    if (result.data.isEmpty())
    {
        result.error = ReplayFileError::emptyRecording;
        return result;
    }

    if (expectedNumBytes >= 0 && numBytesRead != (size_t) expectedNumBytes)
    {
        result.data.reset();
        result.error = ReplayFileError::readFailed;
    }

    return result;
}

std::optional<ReplayFileError> writeReplayFile (const URL& url, const MemoryBlock& data)
{
    if (url.isLocalFile())
    {
        const auto file = url.getLocalFile();

        if (file.existsAsFile() && ! file.deleteFile())
            return ReplayFileError::writeFailed;
    }

    auto stream = url.createOutputStream();

    if (stream == nullptr)
        return ReplayFileError::cannotOpenForWriting;

    if (! stream->write (data.getData(), data.getSize()))
        return ReplayFileError::writeFailed;

    stream->flush();
    return std::nullopt;
}

String getReplayFileErrorMessage (ReplayFileError error)
{
    switch (error)
    {
        case ReplayFileError::cannotOpenForReading:
            return TRANS ("The recording could not be opened for reading.");
        case ReplayFileError::cannotOpenForWriting:
            return TRANS ("The recording destination could not be opened for writing.");
        case ReplayFileError::emptyRecording:
            return TRANS ("The selected recording is empty.");
        case ReplayFileError::readFailed:
            return TRANS ("The recording could not be read completely.");
        case ReplayFileError::writeFailed:
            return TRANS ("The recording could not be written completely.");
    }

    jassertfalse;
    return TRANS ("The recording operation failed.");
}

} // namespace Box2DSamples
