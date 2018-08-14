// Copyright 2004-present Facebook. All Rights Reserved.

#include "JSLogging.h"

#include <fb/Environment.h>
#include <jni/LocalString.h>
#include <android/log.h>
#include <algorithm>
#include <fb/log.h>

#include <jschelpers/Value.h>
using namespace facebook::jni;
namespace facebook {
namespace react {
namespace JSLogging {
  static jclass gLoggerClass;
  static jmethodID gVerboseLogMethod;
  static jmethodID gDebugLogMethod;
  static jmethodID gInfoLogMethod;
  static jmethodID gWarnLogMethod;
  static jmethodID gErrorLogMethod;

JSValueRef nativeLoggingHook(
    JSContextRef ctx,
    JSObjectRef function,
    JSObjectRef thisObject,
    size_t argumentCount,
    const JSValueRef arguments[], JSValueRef *exception) {
  android_LogPriority logLevel = ANDROID_LOG_DEBUG;
  if (argumentCount > 1) {
    int level = (int)Value(ctx, arguments[1]).asNumber();
    // The lowest log level we get from JS is 0. We shift and cap it to be
    // in the range the Android logging method expects.
    logLevel = std::min(
        static_cast<android_LogPriority>(level + ANDROID_LOG_DEBUG),
        ANDROID_LOG_FATAL);
  }
  if (argumentCount > 0) {
    String message = Value(ctx, arguments[0]).toString();
    FBLOG_PRI(logLevel, "ReactNativeJS", "%s", message.str().c_str());
  }
  return Value::makeUndefined(ctx);
}

jmethodID priorityToMethodId(int prio) {
  jmethodID methodId;
    switch (prio) {
      case ANDROID_LOG_VERBOSE:
        methodId = gVerboseLogMethod;
        break;
      case ANDROID_LOG_DEBUG:
        methodId = gDebugLogMethod;
        break;
      case ANDROID_LOG_INFO:
        methodId = gInfoLogMethod;
        break;
      case ANDROID_LOG_WARN:
        methodId = gWarnLogMethod;
        break;
      case ANDROID_LOG_ERROR:
      case ANDROID_LOG_FATAL:
        methodId = gErrorLogMethod;
        break;
      default:
        methodId = gVerboseLogMethod;
    }
    return methodId;
}

void log(int prio, const char* tag, const char* message) {
  ThreadScope threadScope;
  JNIEnv *env = Environment::current();
  LocalString tagString(tag);
  LocalString messageString(message);
  env->CallStaticVoidMethod(gLoggerClass, priorityToMethodId(prio), tagString.string(), messageString.string());

}

void registerNatives() {
  JNIEnv* env = Environment::current();
  jclass loggerClass = env->FindClass("com/facebook/common/logging/FLog");
  gLoggerClass = (jclass)env->NewGlobalRef(loggerClass);
  gVerboseLogMethod = env->GetStaticMethodID(loggerClass, "v", "(Ljava/lang/String;Ljava/lang/String;)V");
  gDebugLogMethod = env->GetStaticMethodID(loggerClass, "d", "(Ljava/lang/String;Ljava/lang/String;)V");
  gInfoLogMethod = env->GetStaticMethodID(loggerClass, "i", "(Ljava/lang/String;Ljava/lang/String;)V");
  gWarnLogMethod = env->GetStaticMethodID(loggerClass, "w", "(Ljava/lang/String;Ljava/lang/String;)V");
  gErrorLogMethod = env->GetStaticMethodID(loggerClass, "e", "(Ljava/lang/String;Ljava/lang/String;)V");

  setLogHandler(log);
}

}}};
