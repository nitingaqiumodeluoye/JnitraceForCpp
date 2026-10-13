# 维护状态：冻结

**本仓库自 2026-10-13 起标记为冻结，不再作为主开发目标。**

主力项目是 **[XposedJniTrace](../XposedJniTrace)**
(fork: `nitingaqiumodeluoye/XposedJniTrace`)，jnitrace 的核心能力已经全部融合过去，
并且在那里持续修 bug + 真机验证。

## 为什么冻结

核心 trace 代码在两个仓库里是**同一份拷贝**（不是各自演化的分支），
两边都改就要手工同步两次 —— 漏一边就是隐性 bug。
这次的 `va_list` 参数错位修复就是两边各改一遍过来的，所以收敛到一处维护。

融合情况（2026-10-13 核对）：

| 文件 | JnitraceForCpp | XposedJniTrace | 状态 |
| --- | --- | --- | --- |
| `JnitraceForC.cpp` | 1626 行 | 1626 行 | 字节级一致 |
| `stringHandler.cpp` | 578 行有效代码 | 577 行 | 只差 1 个 include，功能全同 |
| `traceLog.h` | 39 行 | 39 行 | 一致 |

## 保留价值（冻结不等于没用）

1. **独立 so 形态**：本仓库产出 `libjnitrace.so`，是个可嵌入库 ——
   宿主 App 自己 `System.loadLibrary("jnitrace")` 再调
   `startJnitrace(List<String> soList, String savePath)` 即可，
   **不依赖 Xposed/Vector**。配合 frida 注入、自写注入器、改包都能用。
   XposedJniTrace 是 Xposed 模块，必须有 Vector + root。
2. `nativeLib` 里这批能力**没有被融合过去**，只在这里：
   - `xunwind`（栈回溯）
   - `hidemaps`（隐藏 so 在 maps 里的痕迹）
   - `findsyscall`、`pmparser`、`xDL`
   - `hook/Dobby`（inline hook 引擎）、`hook/signhook`
   - `linker/linkerHandler.cpp` + `solist_util` + `Symbol`

## 将来要用的话：怎么同步

如果哪天需要把 XposedJniTrace 上的修复拿回来，同步这 3 个文件即可：

```
XposedJniTrace/app/src/main/cpp/hook/JnitraceForC.cpp      -> app/src/main/jni/JnitraceForC.cpp
XposedJniTrace/app/src/main/cpp/hook/stringHandler.cpp     -> app/src/main/jni/stringHandler.cpp
XposedJniTrace/app/src/main/cpp/includes/traceLog.h        -> app/src/main/jni/includes/traceLog.h
```

同步时的两个注意点：

1. `stringHandler.cpp` 的**注释**是故意不同的：XposedJniTrace 那份解释的是
   Vector `wxshadow` 的 x17 风险，这边解释的是 `Dobby` 的（两边 hook 引擎不同）。
   功能代码完全相同，同步时保留本仓库的注释即可。
2. 本仓库的 include 根是 `app/src/main/jni/`，那边是 `app/src/main/cpp/`，
   两份 `stringHandler.cpp` 因此差一个 `#include "common_macros.h"`（只有这边有）。

同步后走 CI 验证（`.github/workflows/build.yml`，push 触发），
产物 `jnitrace-debug-apk` 装到设备上用 demo 的 `test5` 验证参数读取：

```
test5(String,int,long,boolean) 由 JNI_OnLoad 以已知常量调用:
  "multi-arg" / 12345 / 0x1122334455667788 / true
日志里这四个值各要打印正确 —— 四个值全一样(或 int/long 变成指针低位)就是又退回去了。
```
