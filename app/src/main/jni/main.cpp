#include <jni.h>
#include <logging.h>
#include <parse.h>
#include <string>
#include <map>
#include <list>
#include <memory>
#include <cstring>
#include <cstdio>
#include <regex>
#include <cerrno>
#include <climits>
#include <iostream>
#include <fstream>

#include "includes/JnitraceForC.h"
#include "includes/stringHandler.h"

/**
 * 日志文件的生命周期由这里统一持有。
 *
 * 原来是用 new 出来的裸指针同时交给 Jnitrace 和 stringHandler 两个模块,
 * 它们的 stopjnitrace() 各自 delete 一次,是double free。
 * 现在两个模块只持有指针,不负责释放。
 */
static std::unique_ptr<std::ofstream> gJnitraceOs;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_jnitrace_MainActivity_startJnitrace(JNIEnv *env, jobject thiz, jobject jmap,
                                                     jstring save_path) {
    std::list<string> clist{};
    if (jmap != nullptr) {
        clist = parse::jlist2clist(env, jmap);
    }
    if (clist.empty()) {
        LOGE("jnitrace filter list is empty, nothing will be traced!")
    }

    //save_path为null或者文件打不开时,只输出到logcat,不影响hook本身(原来会直接return,导致hook一个都不装)
    std::ofstream *saveOs = nullptr;
    if (save_path != nullptr) {
        const string path = parse::jstring2str(env, save_path);
        if (!path.empty()) {
            gJnitraceOs = std::make_unique<std::ofstream>();
            gJnitraceOs->open(path, ios::app);
            if (gJnitraceOs->is_open()) {
                saveOs = gJnitraceOs.get();
            } else {
                LOGE("jnitrace open save file error : %s", path.c_str())
                gJnitraceOs.reset();
            }
        }
    }

    const std::list<string> forbid_list{CORE_SO_NAME};
    Jnitrace::startjnitrace(env, false, forbid_list, clist, saveOs);
    stringHandler::hookStrHandler(false, forbid_list, clist, saveOs);
}
