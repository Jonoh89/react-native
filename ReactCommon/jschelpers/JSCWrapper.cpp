/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include "JSCWrapper.h"

#if defined(__APPLE__)

#include <glog/logging.h>
#include <objc/runtime.h>

// Crash the app (with a descriptive stack trace) if a function that is not supported by
// the system JSC is called.
#define UNIMPLEMENTED_SYSTEM_JSC_FUNCTION(FUNC_NAME)   \
static void Unimplemented_##FUNC_NAME(void* args...) { \
  assert(false);                                       \
}

UNIMPLEMENTED_SYSTEM_JSC_FUNCTION(JSEvaluateBytecodeBundle)
#if WITH_FBJSCEXTENSIONS
UNIMPLEMENTED_SYSTEM_JSC_FUNCTION(JSStringCreateWithUTF8CStringExpectAscii)
#endif
UNIMPLEMENTED_SYSTEM_JSC_FUNCTION(JSPokeSamplingProfiler)
UNIMPLEMENTED_SYSTEM_JSC_FUNCTION(JSStartSamplingProfilingOnMainJSCThread)
UNIMPLEMENTED_SYSTEM_JSC_FUNCTION(configureJSCForIOS)

bool JSSamplingProfilerEnabled() {
  return false;
}

namespace facebook {
namespace react {

static const JSCWrapper* s_customWrapper = nullptr;

static JSCWrapper s_systemWrapper = {
  .JSGlobalContextCreateInGroup = JSGlobalContextCreateInGroup,
  .JSGlobalContextRelease = JSGlobalContextRelease,
  .JSGlobalContextSetName = JSGlobalContextSetName,

  .JSContextGetGlobalContext = JSContextGetGlobalContext,
  .JSContextGetGlobalObject = JSContextGetGlobalObject,

  .JSEvaluateScript = JSEvaluateScript,
  .JSEvaluateBytecodeBundle =
    (decltype(&JSEvaluateBytecodeBundle))Unimplemented_JSEvaluateBytecodeBundle,

  .JSStringCreateWithUTF8CString = JSStringCreateWithUTF8CString,
  .JSStringCreateWithCFString = JSStringCreateWithCFString,
#if WITH_FBJSCEXTENSIONS
  .JSStringCreateWithUTF8CStringExpectAscii =
    (decltype(&JSStringCreateWithUTF8CStringExpectAscii))Unimplemented_JSStringCreateWithUTF8CStringExpectAscii,
#endif
  .JSStringCopyCFString = JSStringCopyCFString,
  .JSStringGetCharactersPtr = JSStringGetCharactersPtr,
  .JSStringGetLength = JSStringGetLength,
  .JSStringGetMaximumUTF8CStringSize = JSStringGetMaximumUTF8CStringSize,
  .JSStringIsEqualToUTF8CString = JSStringIsEqualToUTF8CString,
  .JSStringRelease = JSStringRelease,
  .JSStringRetain = JSStringRetain,

  .JSClassCreate = JSClassCreate,
  .JSClassRelease = JSClassRelease,

  .JSObjectCallAsConstructor = JSObjectCallAsConstructor,
  .JSObjectCallAsFunction = JSObjectCallAsFunction,
  .JSObjectGetPrivate = JSObjectGetPrivate,
  .JSObjectGetProperty = JSObjectGetProperty,
  .JSObjectGetPropertyAtIndex = JSObjectGetPropertyAtIndex,
  .JSObjectIsConstructor = JSObjectIsConstructor,
  .JSObjectIsFunction = JSObjectIsFunction,
  .JSObjectMake = JSObjectMake,
  .JSObjectMakeArray = JSObjectMakeArray,
  .JSObjectMakeError = JSObjectMakeError,
  .JSObjectMakeFunctionWithCallback = JSObjectMakeFunctionWithCallback,
  .JSObjectSetPrivate = JSObjectSetPrivate,
  .JSObjectSetProperty = JSObjectSetProperty,

  .JSObjectCopyPropertyNames = JSObjectCopyPropertyNames,
  .JSPropertyNameArrayGetCount = JSPropertyNameArrayGetCount,
  .JSPropertyNameArrayGetNameAtIndex = JSPropertyNameArrayGetNameAtIndex,
  .JSPropertyNameArrayRelease = JSPropertyNameArrayRelease,

  .JSValueCreateJSONString = JSValueCreateJSONString,
  .JSValueGetType = JSValueGetType,
  .JSValueMakeFromJSONString = JSValueMakeFromJSONString,
  .JSValueMakeBoolean = JSValueMakeBoolean,
  .JSValueMakeNull = JSValueMakeNull,
  .JSValueMakeNumber = JSValueMakeNumber,
  .JSValueMakeString = JSValueMakeString,
  .JSValueMakeUndefined = JSValueMakeUndefined,
  .JSValueProtect = JSValueProtect,
  .JSValueToBoolean = JSValueToBoolean,
  .JSValueToNumber = JSValueToNumber,
  .JSValueToObject = JSValueToObject,
  .JSValueToStringCopy = JSValueToStringCopy,
  .JSValueUnprotect = JSValueUnprotect,

  .JSSamplingProfilerEnabled = JSSamplingProfilerEnabled,
  .JSPokeSamplingProfiler =
    (decltype(&JSPokeSamplingProfiler))Unimplemented_JSPokeSamplingProfiler,
  .JSStartSamplingProfilingOnMainJSCThread =
    (decltype(&JSStartSamplingProfilingOnMainJSCThread))Unimplemented_JSStartSamplingProfilingOnMainJSCThread,

  .configureJSCForIOS = (decltype(&configureJSCForIOS))Unimplemented_configureJSCForIOS,

  .JSBytecodeFileFormatVersion = -1,

  .JSContext = nullptr,
  .JSValue = nullptr,
};

void setCustomJSCWrapper(const JSCWrapper* wrapper) {
  CHECK(s_customWrapper == nullptr) << "Can't set custom JSC wrapper multiple times";
  s_customWrapper = wrapper;
}

const JSCWrapper* systemJSCWrapper() {
  // Note that this is not used on Android. All methods are statically linked instead.
  // Some fields are lazily initialized
  static std::once_flag flag;
  std::call_once(flag, []() {
    s_systemWrapper.JSContext = objc_getClass("JSContext");
    s_systemWrapper.JSValue = objc_getClass("JSValue");
  });
  return &s_systemWrapper;
}

const JSCWrapper* customJSCWrapper() {
  CHECK(s_customWrapper != nullptr) << "Accessing custom JSC wrapper before it's set";
  return s_customWrapper;
}

} }

#endif
