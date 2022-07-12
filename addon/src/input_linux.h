#pragma once

#include <napi.h>

using namespace Napi;

namespace HidInput {

class LinuxInput {
   public:
    static Value Open(const CallbackInfo &info);
    static Value Read(const CallbackInfo &info);
    static Value Close(const CallbackInfo &info);

   private:
    static void AsyncOpen(napi_env env, void *data);
    static void FinishAsyncOpen(napi_env env, napi_status status, void *data);
    static void AsyncRead(napi_env env, void *data);
    static void FinishAsyncRead(napi_env env, napi_status status, void *data);
    static void AsyncClose(napi_env env, void *data);
    static void FinishAsyncClose(napi_env env, napi_status status, void *data);
};

}  // namespace HidInput
