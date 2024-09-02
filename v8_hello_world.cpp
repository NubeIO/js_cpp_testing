#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include "libplatform/libplatform.h"
#include "v8.h"

// Helper function to read file content
std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Failed to open file: %s\n", filename.c_str());
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to extract result from V8
std::string GetStringFromV8String(v8::Isolate* isolate, v8::Local<v8::String> v8String) {
    v8::String::Utf8Value utf8(isolate, v8String);
    return std::string(*utf8);
}

// Callback function for console.log
void ConsoleLog(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope handle_scope(isolate);

    std::string result;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(isolate);
        if (i > 0) result += " ";

        v8::Local<v8::Value> arg = args[i];
        if (arg->IsString()) {
            result += GetStringFromV8String(isolate, arg.As<v8::String>());
        } else {
            v8::Local<v8::String> stringified = v8::JSON::Stringify(isolate->GetCurrentContext(), arg).ToLocalChecked();
            result += GetStringFromV8String(isolate, stringified);
        }
    }

    printf("%s\n", result.c_str());
}

void RunScript(v8::Isolate* isolate, v8::Local<v8::Context>& context, const std::string& js_code, bool use_jit) {
    auto start = std::chrono::high_resolution_clock::now();

    v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, js_code.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

    v8::ScriptCompiler::Source script_source(source);
    v8::Local<v8::Script> script;

    if (use_jit) {
        script = v8::ScriptCompiler::Compile(context, &script_source).ToLocalChecked();
    } else {
        v8::ScriptCompiler::CompileOptions no_jit_option = v8::ScriptCompiler::kNoCompileOptions;
        script = v8::ScriptCompiler::Compile(context, &script_source, no_jit_option).ToLocalChecked();
    }

    script->Run(context).ToLocalChecked();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    printf("Total time (including compilation) with %s: %lld ms\n",
           use_jit ? "JIT" : "no JIT", duration.count());
}

int main(int argc, char* argv[]) {
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    // Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);

        // Create a new context.
        v8::Local<v8::Context> context = v8::Context::New(isolate);
        v8::Context::Scope context_scope(context);

        // Set up the console.log function
        v8::Local<v8::Object> global = context->Global();
        v8::Local<v8::Object> console = v8::Object::New(isolate);
        console->Set(context,
                     v8::String::NewFromUtf8(isolate, "log", v8::NewStringType::kNormal).ToLocalChecked(),
                     v8::Function::New(context, ConsoleLog).ToLocalChecked()).Check();
        global->Set(context,
                    v8::String::NewFromUtf8(isolate, "console", v8::NewStringType::kNormal).ToLocalChecked(),
                    console).Check();

        // Read the JavaScript file
        std::string js_code = ReadFile("../test.js");

        // Run without JIT
        RunScript(isolate, context, js_code, false);

        // Run with JIT
        RunScript(isolate, context, js_code, true);
    }

    // Dispose the isolate and tear down V8.
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}