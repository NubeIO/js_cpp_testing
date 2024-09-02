#include <iostream>
#include <vector>
#include <chrono>
#include <quickjs/quickjs.h>
#include <cstring>

static JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    return JS_UNDEFINED;
}

static void js_console_init(JSContext *ctx) {
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_console_log, "log", 1));
    JS_SetPropertyStr(ctx, global, "console", console);
    JS_FreeValue(ctx, global);
}

std::vector<uint8_t> compileToByteCode(JSContext *ctx, const char *js_code) {
    JSValue func = JS_Eval(ctx, js_code, strlen(js_code), "<input>", JS_EVAL_FLAG_COMPILE_ONLY);
    size_t bytecode_len;
    uint8_t* bytecode = JS_WriteObject(ctx, &bytecode_len, func, JS_WRITE_OBJ_BYTECODE);
    JS_FreeValue(ctx, func);
    std::vector<uint8_t> result(bytecode, bytecode + bytecode_len);
    js_free(ctx, bytecode);
    return result;
}

void executeByteCode(JSContext *ctx, const std::vector<uint8_t>& bytecode) {
    JSValue func = JS_ReadObject(ctx, bytecode.data(), bytecode.size(), JS_READ_OBJ_BYTECODE);
    JSValue result = JS_EvalFunction(ctx, func);
    JS_FreeValue(ctx, result);
}

void executeRawString(JSContext *ctx, const char* js_code) {
    JSValue result = JS_Eval(ctx, js_code, strlen(js_code), "<input>", 0);
    JS_FreeValue(ctx, result);
}

void runSpeedTest(JSContext *ctx, const char* js_code, int iterations, int runs) {
    auto bytecode = compileToByteCode(ctx, js_code);

    double total_bytecode_time = 0;
    double total_rawstring_time = 0;

    for (int run = 0; run < runs; run++) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            executeByteCode(ctx, bytecode);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto bytecode_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        total_bytecode_time += bytecode_duration.count();

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; i++) {
            executeRawString(ctx, js_code);
        }
        end = std::chrono::high_resolution_clock::now();
        auto rawstring_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        total_rawstring_time += rawstring_duration.count();

        std::cout << "Run " << run + 1 << ":" << std::endl;
        std::cout << "  Bytecode execution time: " << bytecode_duration.count() << " microseconds" << std::endl;
        std::cout << "  Raw string execution time: " << rawstring_duration.count() << " microseconds" << std::endl;
        std::cout << "  Ratio: " << (double)rawstring_duration.count() / bytecode_duration.count() << std::endl << std::endl;
    }

    double avg_bytecode_time = total_bytecode_time / runs;
    double avg_rawstring_time = total_rawstring_time / runs;

    std::cout << "Average results over " << runs << " runs:" << std::endl;
    std::cout << "  Avg Bytecode execution time: " << avg_bytecode_time << " microseconds" << std::endl;
    std::cout << "  Avg Raw string execution time: " << avg_rawstring_time << " microseconds" << std::endl;
    std::cout << "  Avg Ratio: " << avg_rawstring_time / avg_bytecode_time << std::endl;
}

int main() {
    JSRuntime *rt = JS_NewRuntime();
    JSContext *ctx = JS_NewContext(rt);

    js_console_init(ctx);

    const char *js_code = R"(
        function fibonacci(n) {
            if (n <= 1) return n;
            return fibonacci(n - 1) + fibonacci(n - 2);
        }
        console.log(fibonacci(20));
    )";

    const char *js_code_complex = R"(
        function complexOperation(n) {
            let result = 0;
            for (let i = 0; i < n; i++) {
                for (let j = 0; j < n; j++) {
                    result += Math.sin(i) * Math.cos(j);
                }
            }
            return result;
        }
        console.log(complexOperation(100));
    )";

    int iterations = 1000;
    int runs = 5;

    std::cout << "Simple Fibonacci Test:" << std::endl;
    runSpeedTest(ctx, js_code, iterations, runs);

    std::cout << "\nComplex Operation Test:" << std::endl;
    runSpeedTest(ctx, js_code_complex, iterations, runs);

    JS_RunGC(rt);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    return 0;
}