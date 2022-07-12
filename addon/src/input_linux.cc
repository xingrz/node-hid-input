#include "input_linux.h"

extern "C" {
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}

#include "keymap.h"
#include "promised_async.h"

using namespace Napi;

namespace HidInput {

class FdHandle {
   public:
    char path[255];
    bool exclusive;
    int fd;
    char *error;
};

Value LinuxInput::Open(const CallbackInfo &info) {
    auto env = info.Env();
    if (!info[0].IsString()) {
        TypeError::New(env, "The first argument should be a string").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    if (!info[1].IsObject()) {
        TypeError::New(env, "The second argument should be an object").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto options = info[1].ToObject();
    auto exclusive = options.Get("exclusive");

    auto baton = new PromisedAsync<FdHandle>(
        env, "open", LinuxInput::AsyncOpen, LinuxInput::FinishAsyncOpen);

    strcpy(baton->context.path, info[0].ToString().Utf8Value().c_str());
    baton->context.exclusive = exclusive.IsBoolean() && exclusive.ToBoolean().Value();

    return baton->Queue();
}

void LinuxInput::AsyncOpen(napi_env env, void *data) {
    auto *baton = static_cast<PromisedAsync<FdHandle> *>(data);
    auto ctx = &baton->context;
    ctx->fd = open(ctx->path, O_RDONLY | O_NONBLOCK);
    if (ctx->fd == -1) {
        ctx->error = strerror(errno);
    } else if (ctx->exclusive) {
        ioctl(ctx->fd, EVIOCGRAB, 1);
    }
}

void LinuxInput::FinishAsyncOpen(napi_env env, napi_status status, void *data) {
    auto *baton = static_cast<PromisedAsync<FdHandle> *>(data);
    auto ctx = &baton->context;
    if (ctx->fd != -1) {
        auto handle = new FdHandle(*ctx);
        Object result = Object::New(env);
        result.Set(String::New(env, "read"), Function::New<LinuxInput::Read>(env, "read", handle));
        result.Set(
            String::New(env, "close"), Function::New<LinuxInput::Close>(env, "close", handle));
        baton->Resolve(result);
    } else {
        baton->Reject(
            String::New(env, std::string("Failed opening device: ") + std::string(ctx->error)));
    }
    delete baton;
}

class ReadContext {
   public:
    FdHandle *handle;
    uint8_t shift;
    std::string line;
};

Value LinuxInput::Read(const CallbackInfo &info) {
    auto env = info.Env();

    auto baton = new PromisedAsync<ReadContext>(
        env, "read", LinuxInput::AsyncRead, LinuxInput::FinishAsyncRead);

    baton->context.handle = static_cast<FdHandle *>(info.Data());
    baton->context.shift = 0;
    baton->context.line.resize(0);

    return baton->Queue();
}

void LinuxInput::AsyncRead(napi_env env, void *data) {
    auto *baton = static_cast<PromisedAsync<ReadContext> *>(data);
    auto ctx = &baton->context;

    int ret;
    struct input_event input = {0};
    while (ctx->handle->fd != 0) {
        ret = read(ctx->handle->fd, &input, sizeof(input));
        if (ret == -1) {
            if (errno == EAGAIN) {
                usleep(10 * 1000);
                continue;
            } else {
                break;
            }
        }
        if (input.type == EV_KEY && input.value) {
            if (input.code == KEY_ENTER) {
                break;
            } else if (input.code == KEY_LEFTSHIFT || input.code == KEY_RIGHTSHIFT) {
                ctx->shift = 1;
            } else {
                for (int i = 0; i < COUNT(keymap); i++) {
                    if (keymap[i][0] == input.code) {
                        ctx->line.push_back(keymap[i][1 + ctx->shift]);
                        ctx->shift = 0;
                    }
                }
            }
        }
    }
}

void LinuxInput::FinishAsyncRead(napi_env env, napi_status status, void *data) {
    auto *baton = static_cast<PromisedAsync<ReadContext> *>(data);
    auto ctx = &baton->context;
    baton->Resolve(String::New(env, ctx->line));
    delete baton;
}

class CloseContext {
   public:
    FdHandle *handle;
};

Value LinuxInput::Close(const CallbackInfo &info) {
    auto env = info.Env();

    auto baton = new PromisedAsync<ReadContext>(
        env, "close", LinuxInput::AsyncClose, LinuxInput::FinishAsyncClose);

    baton->context.handle = static_cast<FdHandle *>(info.Data());

    return baton->Queue();
}

void LinuxInput::AsyncClose(napi_env env, void *data) {
    auto *baton = static_cast<PromisedAsync<CloseContext> *>(data);
    auto ctx = &baton->context;
    int fd = ctx->handle->fd;
    ctx->handle->fd = 0;
    if (ctx->handle->exclusive) {
        ioctl(fd, EVIOCGRAB, 0);
    }
    close(fd);
    delete ctx->handle;
}

void LinuxInput::FinishAsyncClose(napi_env env, napi_status status, void *data) {
    auto *baton = static_cast<PromisedAsync<CloseContext> *>(data);
    baton->Resolve(Env(env).Undefined());
    delete baton;
}

}  // namespace HidInput
