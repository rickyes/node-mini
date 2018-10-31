// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/wasm/function-compiler.h"

#include "src/compiler/wasm-compiler.h"
#include "src/counters.h"
#include "src/macro-assembler-inl.h"
#include "src/wasm/baseline/liftoff-compiler.h"
#include "src/wasm/wasm-code-manager.h"

namespace v8 {
namespace internal {
namespace wasm {

namespace {

const char* GetExecutionTierAsString(ExecutionTier mode) {
  switch (mode) {
    case ExecutionTier::kBaseline:
      return "liftoff";
    case ExecutionTier::kOptimized:
      return "turbofan";
    case ExecutionTier::kInterpreter:
      return "interpreter";
  }
  UNREACHABLE();
}

}  // namespace

// static
ExecutionTier WasmCompilationUnit::GetDefaultExecutionTier() {
  return FLAG_liftoff ? ExecutionTier::kBaseline : ExecutionTier::kOptimized;
}

WasmCompilationUnit::WasmCompilationUnit(WasmEngine* wasm_engine,
                                         NativeModule* native_module,
                                         FunctionBody body, int index,
                                         Counters* counters, ExecutionTier mode)
    : wasm_engine_(wasm_engine),
      func_body_(body),
      counters_(counters),
      func_index_(index),
      native_module_(native_module),
      mode_(mode) {
  const WasmModule* module = native_module->module();
  DCHECK_GE(index, module->num_imported_functions);
  DCHECK_LT(index, module->functions.size());
  // Always disable Liftoff for asm.js, for two reasons:
  //    1) asm-specific opcodes are not implemented, and
  //    2) tier-up does not work with lazy compilation.
  if (module->origin == kAsmJsOrigin) mode = ExecutionTier::kOptimized;
  if (V8_UNLIKELY(FLAG_wasm_tier_mask_for_testing) && index < 32 &&
      (FLAG_wasm_tier_mask_for_testing & (1 << index))) {
    mode = ExecutionTier::kOptimized;
  }
  SwitchMode(mode);
}

// Declared here such that {LiftoffCompilationUnit} and
// {TurbofanWasmCompilationUnit} can be opaque in the header file.
WasmCompilationUnit::~WasmCompilationUnit() = default;

void WasmCompilationUnit::ExecuteCompilation(CompilationEnv* env,
                                             WasmFeatures* detected) {
  const WasmModule* module = native_module_->module();
  auto size_histogram =
      SELECT_WASM_COUNTER(counters_, module->origin, wasm, function_size_bytes);
  size_histogram->AddSample(
      static_cast<int>(func_body_.end - func_body_.start));
  auto timed_histogram = SELECT_WASM_COUNTER(counters_, module->origin,
                                             wasm_compile, function_time);
  TimedHistogramScope wasm_compile_function_time_scope(timed_histogram);

  if (FLAG_trace_wasm_compiler) {
    PrintF("Compiling wasm function %d with %s\n\n", func_index_,
           GetExecutionTierAsString(mode_));
  }

  switch (mode_) {
    case ExecutionTier::kBaseline:
      if (liftoff_unit_->ExecuteCompilation(env, detected)) break;
      // Otherwise, fall back to turbofan.
      SwitchMode(ExecutionTier::kOptimized);
      V8_FALLTHROUGH;
    case ExecutionTier::kOptimized:
      turbofan_unit_->ExecuteCompilation(env, detected);
      break;
    case ExecutionTier::kInterpreter:
      UNREACHABLE();  // TODO(titzer): compile interpreter entry stub.
  }
}

void WasmCompilationUnit::ReportError(ErrorThrower* thrower) const {
  DCHECK(result_.failed());
  // Add the function as another context for the exception. This is
  // user-visible, so use official format.
  EmbeddedVector<char, 128> message;
  wasm::ModuleWireBytes wire_bytes(native_module()->wire_bytes());
  wasm::WireBytesRef name_ref =
      native_module()->module()->LookupFunctionName(wire_bytes, func_index_);
  if (name_ref.is_set()) {
    wasm::WasmName name = wire_bytes.GetNameOrNull(name_ref);
    SNPrintF(message, "Compiling wasm function \"%.*s\" failed", name.length(),
             name.start());
  } else {
    SNPrintF(message, "Compiling wasm function \"wasm-function[%d]\" failed",
             func_index_);
  }
  thrower->CompileFailed(message.start(), result_);
}

void WasmCompilationUnit::SwitchMode(ExecutionTier new_mode) {
  // This method is being called in the constructor, where neither
  // {liftoff_unit_} nor {turbofan_unit_} are set, or to switch mode from
  // kLiftoff to kTurbofan, in which case {liftoff_unit_} is already set.
  mode_ = new_mode;
  switch (new_mode) {
    case ExecutionTier::kBaseline:
      DCHECK(!turbofan_unit_);
      DCHECK(!liftoff_unit_);
      liftoff_unit_.reset(new LiftoffCompilationUnit(this));
      return;
    case ExecutionTier::kOptimized:
      DCHECK(!turbofan_unit_);
      liftoff_unit_.reset();
      turbofan_unit_.reset(new compiler::TurbofanWasmCompilationUnit(this));
      return;
    case ExecutionTier::kInterpreter:
      UNREACHABLE();  // TODO(titzer): allow compiling interpreter entry stub.
  }
  UNREACHABLE();
}

// static
bool WasmCompilationUnit::CompileWasmFunction(
    Isolate* isolate, NativeModule* native_module, WasmFeatures* detected,
    ErrorThrower* thrower, const WasmFunction* function, ExecutionTier mode) {
  ModuleWireBytes wire_bytes(native_module->wire_bytes());
  FunctionBody function_body{function->sig, function->code.offset(),
                             wire_bytes.start() + function->code.offset(),
                             wire_bytes.start() + function->code.end_offset()};

  WasmCompilationUnit unit(isolate->wasm_engine(), native_module, function_body,
                           function->func_index, isolate->counters(), mode);
  CompilationEnv env = native_module->CreateCompilationEnv();
  unit.ExecuteCompilation(&env, detected);
  if (unit.failed()) {
    unit.ReportError(thrower);
    return false;
  }
  return true;
}

void WasmCompilationUnit::SetResult(WasmCode* code) {
  DCHECK(!result_.failed());
  DCHECK_NULL(result_.value());
  result_ = Result<WasmCode*>(code);
  native_module()->PublishCode(code);

  counters_->wasm_generated_code_size()->Increment(
      static_cast<int>(code->instructions().size()));
  counters_->wasm_reloc_size()->Increment(
      static_cast<int>(code->reloc_info().size()));
}

}  // namespace wasm
}  // namespace internal
}  // namespace v8
