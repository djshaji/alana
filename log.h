/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef __SAMPLE_ANDROID_DEBUG_H__
#define __SAMPLE_ANDROID_DEBUG_H__
#include <cstdio>
#include <cstdlib>
#include <cmath>

#ifndef MODULE_NAME
#define MODULE_NAME  __FILE_NAME__
#endif

#ifdef __ANDROID__
#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, MODULE_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, MODULE_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,MODULE_NAME, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,MODULE_NAME, __VA_ARGS__)
#define HERE __android_log_print(ANDROID_LOG_VERBOSE, "!!", "%s: %d", __FILE__, __LINE__) ;
#define IN __android_log_print(ANDROID_LOG_VERBOSE, ">>", __PRETTY_FUNCTION__);
#define OUT __android_log_print(ANDROID_LOG_VERBOSE, "<<", __PRETTY_FUNCTION__);
#define PASS {int _hwye_ = 0 ;}
#define ASSERT(cond, ...) if (!(cond)) {__android_log_assert(#cond, MODULE_NAME, __VA_ARGS__);}
#else

#define logd(msg)   g_log_default_handler (MODULE_NAME, G_LOG_LEVEL_WARNING, msg, NULL);


#ifdef __linux__

#define LOGI printf
#define LOGD printf
#define LOGW printf
#define LOGE printf
#define LOGF printf
#define ASSERT(cond, ...)
//~ #define LOGD(arg) printf("%s [%s: %d] %s\n", time_str, __FILE__, __LINE__, arg) ;
#define LOGV(arg) printf("!! [%s: %d] %s\n", __FILE__, __LINE__, arg) ;
#define HERE printf("!! %s: %d\n", __FILE__, __LINE__) ;
#define IN printf(">> %s\n", __PRETTY_FUNCTION__);
#define OUT printf("<< %s \n", __PRETTY_FUNCTION__);

#else
#include <gtk/gtk.h>
#define IN printf_windows(">> %s\n", __PRETTY_FUNCTION__);
#define OUT printf_windows("<< %s \n", __PRETTY_FUNCTION__);
#include <windows.h>
#ifdef __cplusplus
extern "C" {
#endif

int printf_windows (const char* format, ...) ;
#ifdef __cplusplus
}
#endif

#define LOGI printf_windows
#define LOGD printf_windows
#define LOGW printf_windows
#define LOGE printf_windows
#define LOGF printf_windows
#define ASSERT(cond, ...)
#define LOGV(arg) printf_windows("!! [%s: %d] %s\n", __FILE__, __LINE__, arg) ;
#define HERE printf_windows("!! %s: %d\n", __FILE__, __LINE__) ;
#define IN printf_windows(">> %s\n", __PRETTY_FUNCTION__);
#define OUT printf_windows("<< %s \n", __PRETTY_FUNCTION__);

#endif

#endif

#endif // __SAMPLE_ANDROID_DEBUG_H__
