#include "addon.h"

#include <napi.h>

#if __linux__
#include "input_linux.h"
#endif

using namespace Napi;
using namespace HidInput;

static Object Init(Env env, Object exports) {
#if __linux__
    exports.Set(String::New(env, "openInput"), Function::New<LinuxInput::Open>(env, "openInput"));
#endif
    return exports;
}

NODE_API_MODULE(hidInput, Init);
