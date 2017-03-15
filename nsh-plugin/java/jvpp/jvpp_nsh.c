/*
 * Copyright (c) 2016 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vnet/vnet.h>

#include <vpp-api/nsh_msg_enum.h>
#define vl_typedefs             /* define message structures */
#include <vpp-api/nsh_all_api_h.h>
#undef vl_typedefs

#define vl_endianfun
#include <vpp-api/nsh_all_api_h.h>
#undef vl_endianfun

#define vl_print(handle, ...)
#define vl_printfun
#include <vpp-api/nsh_all_api_h.h>
#undef vl_printfun

#include <vnet/api_errno.h>
#include <vlibapi/api.h>
#include <vlibmemory/api.h>

#include <jvpp-common/jvpp_common.h>

#include "jvpp/io_fd_vpp_jvpp_nsh_JVppNshImpl.h"

#include "jvpp_nsh.h"
#include "jvpp/jvpp_nsh_gen.h"

/*
 * Class:     io_fd_vpp_jvpp_nsh_JVppNshImpl
 * Method:    init0
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_io_fd_vpp_jvpp_nsh_JVppNshImpl_init0
  (JNIEnv *env, jclass clazz, jobject callback, jlong queue_address, jint my_client_index) {
    nsh_main_t * plugin_main = &nsh_main;
    clib_warning ("Java_io_fd_vpp_jvpp_nsh_JVppNshImpl_init0");

    plugin_main->my_client_index = my_client_index;
    plugin_main->vl_input_queue = (unix_shared_memory_queue_t *)queue_address;

    plugin_main->callbackObject = (*env)->NewGlobalRef(env, callback);
    plugin_main->callbackClass = (jclass)(*env)->NewGlobalRef(env, (*env)->GetObjectClass(env, callback));

    // verify API has not changed since jar generation
    #define _(N)             \
        get_message_id(env, #N);
        foreach_supported_api_message;
    #undef _

    #define _(N,n)                                  \
        vl_msg_api_set_handlers(get_message_id(env, #N), #n,     \
    		vl_api_##n##_t_handler,             \
    		vl_noop_handler,                    \
    		vl_api_##n##_t_endian,              \
    		vl_api_##n##_t_print,               \
	    	sizeof(vl_api_##n##_t), 1);
        foreach_api_reply_handler;
    #undef _
}

JNIEXPORT void JNICALL Java_io_fd_vpp_jvpp_nsh_JVppNshImpl_close0
(JNIEnv *env, jclass clazz) {
	nsh_main_t * plugin_main = &nsh_main;

    // cleanup:
    (*env)->DeleteGlobalRef(env, plugin_main->callbackClass);
    (*env)->DeleteGlobalRef(env, plugin_main->callbackObject);

    plugin_main->callbackClass = NULL;
    plugin_main->callbackObject = NULL;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv* env;

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_8) != JNI_OK) {
        return JNI_EVERSION;
    }

    if (cache_class_references(env) != 0) {
        clib_warning ("Failed to cache class references\n");
        return JNI_ERR;
    }

    return JNI_VERSION_1_8;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
    JNIEnv* env;
    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_8) != JNI_OK) {
        return;
    }
    delete_class_references(env);
}


