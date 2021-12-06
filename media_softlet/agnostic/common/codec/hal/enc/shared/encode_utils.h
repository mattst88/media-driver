/*
* Copyright (c) 2018-2021, Intel Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef __ENCODE_UTILS_H__
#define __ENCODE_UTILS_H__
#include <mutex>
#include "mos_util_debug.h"

#define _SW_BRC _WIN64 && (_DEBUG || _RELEASE_INTERNAL)

enum HuCFunction
{
    NONE_BRC = 0,
    BRC_INIT,
    BRC_RESET,
    BRC_UPDATE,
    LA_INIT,
    LA_RESET,
    LA_UPDATE,
    PAK_INTEGRATE,
    HPU_VP9
};

//------------------------------------------------------------------------------
// Macros specific to MOS_CODEC_SUBCOMP_ENCODE sub-comp
//------------------------------------------------------------------------------
#define ENCODE_ASSERT(_expr)                                                   \
    MOS_ASSERT(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _expr)

#define ENCODE_ASSERTMESSAGE(_message, ...)                                    \
    MOS_ASSERTMESSAGE(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _message, ##__VA_ARGS__)

#define ENCODE_NORMALMESSAGE(_message, ...)                                    \
    MOS_NORMALMESSAGE(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _message, ##__VA_ARGS__)

#define ENCODE_VERBOSEMESSAGE(_message, ...)                                   \
    MOS_VERBOSEMESSAGE(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _message, ##__VA_ARGS__)

#define ENCODE_CHK_NULL_RETURN(_ptr)                                                  \
    MOS_CHK_NULL_RETURN(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _ptr)

#define ENCODE_CHK_NULL_NO_STATUS_RETURN(_ptr)                                                  \
    MOS_CHK_NULL_NO_STATUS_RETURN(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _ptr)

#define ENCODE_CHK_STATUS_RETURN(_stmt)                                               \
    MOS_CHK_STATUS_RETURN(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _stmt)

#define ENCODE_CHK_COND_RETURN(_stmt, _message)                                               \
    MOS_CHK_COND_RETURN(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _stmt, _message)

#define ENCODE_CHK_STATUS_MESSAGE_RETURN(_stmt, _message, ...)                        \
    MOS_CHK_STATUS_MESSAGE_RETURN(MOS_COMPONENT_CODEC, MOS_CODEC_SUBCOMP_ENCODE, _stmt, _message, ##__VA_ARGS__)

namespace encode {

class Trace
{
public:
    Trace(const char* name) : m_name(name)
    {
        ENCODE_VERBOSEMESSAGE("Enter function:%s\r\n", name);
    }

    ~Trace()
    {
        ENCODE_VERBOSEMESSAGE("Exit function:%s\r\n", m_name);
    }

protected:
    const char* m_name;
};

class AutoLock
{
public:
    AutoLock(PMOS_MUTEX mutex) : m_mutex(mutex) { MosUtilities::MosLockMutex(mutex); }
    ~AutoLock() { MosUtilities::MosUnlockMutex(m_mutex); }
protected:
    PMOS_MUTEX m_mutex;
};

class Condition
{
public:
    Condition()
    {
        m_sem = MOS_CreateSemaphore(0, 1);
    }

    ~Condition()
    {
        MOS_DestroySemaphore(m_sem);
    }
    MOS_STATUS Wait(PMOS_MUTEX mutex)
    {
        MOS_STATUS status = MOS_STATUS_SUCCESS;
        MosUtilities::MosUnlockMutex(mutex);
        status = MOS_WaitSemaphore(m_sem, 5000);
        MosUtilities::MosLockMutex(mutex);
        return status;
    }
    MOS_STATUS Signal()
    {
        MOS_PostSemaphore(m_sem, 1);
        return MOS_STATUS_SUCCESS;
    }

protected:
    PMOS_SEMAPHORE m_sem;
};

}

#define ENCODE_FUNC_CALL() encode::Trace trace(__FUNCTION__);

#endif // !__ENCODE_UTILS_H__