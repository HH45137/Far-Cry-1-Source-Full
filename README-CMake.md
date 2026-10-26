# Far Cry 1 — CMake + Clang 构建说明

这套 CMake 工程把原来只面向 Visual Studio 2003/2005（MSVC 7.1/8.0、Win32、`/MD`）的
`Game01.sln` + 若干 `.vcproj` 改造成可以用 **现代 clang-cl 正常编译和链接** 的形式。

原始的 `.sln/.vcproj` 全部保留未动，CMake 只是新增了一套并行的构建描述。

## 1. 环境要求

| 项目 | 要求 |
| --- | --- |
| 操作系统 | Windows 10 / 11（x64 主机） |
| 编译器 | LLVM/Clang for Windows（`clang-cl.exe`），建议 16 及以上（本工程用 23.1 验证） |
| 构建系统 | CMake ≥ 3.21（本工程用 4.2 验证），Ninja 或 Visual Studio 2022 生成器 |
| Visual Studio | VS 2022 / VS 2026 任一版本，需要 MSVC 工具集（提供 link.exe/lld-link、Windows SDK、MFC/ATL 头文件） |
| 目标平台 | **必须 32 位（Win32）**。源码树里所有预编译第三方库（D3D9 SDK、Bink、Cg、Ogg/Vorbis、Ubi.com）都是 x86 |

> 说明：`clang-cl` 会自动从注册表找到最新的 MSVC 工具集与 Windows SDK 头文件，
> 所以 **不必** 先执行 `vcvars32.bat`；但 Ninja 生成器需要环境变量里有 `link.exe`
> 的位置，脚本里已经帮忙调用 `vcvars32.bat`。

## 2. 快速开始

### 2.1 一键脚本（推荐）

```bat
:: Ninja + clang-cl（单配置，速度最快；只依赖 PATH 里的 LLVM，无需 VS 的 Clang 组件）
build-clang.bat Release

:: 或者：Visual Studio 2022 生成器 + ClangCL 工具集（多配置，可在 IDE 里调试）
:: 注意：需要先在 VS Installer 里安装 “适用于 Windows 的 C++ Clang 编译器” 组件
build-clang-vs.bat Release
```

产物统一输出到 `build-clang\Bin32`（或 `build-vs\Bin32`），与零售版游戏目录结构一致，
可以直接拷贝到游戏 `Bin32` 目录里覆盖原 DLL。

### 2.2 手动执行

```bat
cmake -S . -B build-clang -G Ninja ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl ^
      -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32

cmake --build build-clang --parallel
```

> `-m32` 不能省略：clang-cl 默认按 x64 目标编译，而 MSVC 的 `cl.exe` 由
> `vcvars32.bat` 决定。CMake 里会检查 `CMAKE_SIZEOF_VOID_P`，不是 32 位就直接报错终止。

### 2.3 构建成功后得到的文件

```
Bin32\
  FarCry.exe            # 启动器（LoadLibrary CrySystem.dll）
  FarCry_WinSV.exe      # 独立服务器
  CrySystem.dll  Cry3DEngine.dll  CryAnimation.dll  CryAISystem.dll(可选)
  CryEntitySystem.dll CryFont.dll CryGame.dll CryInput.dll CryMovie.dll
  CryNetwork.dll CryPhysics.dll CryScriptSystem.dll CrySoundSystem.dll
  XRenderD3D9.dll  XRenderNULL.dll  (XRenderOGL.dll 可选)
  rc.exe                # 资源编译器（ResourceCompiler）
Lib32\                  # 各 DLL 的导入库
```

## 3. 构建选项

| 选项 | 默认 | 说明 |
| --- | --- | --- |
| `FARCRY_BUILD_GAME` | ON | `CryGame.dll` + `FarCry.exe` |
| `FARCRY_BUILD_DEDICATED_SERVER` | ON | `FarCry_WinSV.exe` |
| `FARCRY_BUILD_RENDERER_D3D9` | ON | Direct3D 9 渲染器 |
| `FARCRY_BUILD_RENDERER_NULL` | ON | 无渲染的 NULL 渲染器（无显卡环境/服务器用） |
| `FARCRY_BUILD_RENDERER_OGL` | OFF | 老式 OpenGL 渲染器（代码最旧，还有若干编译错误待处理） |
| `FARCRY_BUILD_RESOURCE_COMPILER` | ON | `rc.exe`（资源编译器本体） |
| `FARCRY_BUILD_RESOURCE_COMPILER_PC` | OFF | `ResourceCompilerPC.dll` 插件，ATL/`CString` 代码量大，尚未移植完 |
| `FARCRY_BUILD_EDITOR` | OFF | `Editor.exe`，依赖 MFC + 预编译 XT 库，尚未移植 |
| `FARCRY_BUILD_AISYSTEM` | OFF | 公开源码里 CryAISystem 只有 8 个 cpp，缺少实现文件，无法链接 |
| `FARCRY_USE_UBICOM_SDK` | OFF | Ubi.com/GameSpy SDK 的预编译库是用 STLport + 旧 CRT 编的，无法与现在的 MSVC STL 链接 |
| `FARCRY_USE_PUNKBUSTER_SDK` | ON | PunkBuster 源码在树内，可直接编译 |
| `FARCRY_USE_BINK_SDK` / `FARCRY_USE_DIVX_SDK` / `FARCRY_USE_ASE_SDK` | Bink ON，其余 OFF | 通过 `NOT_USE_*_SDK` 宏控制 |

示例：只编引擎核心 + NULL 渲染器（服务器）：

```bat
cmake -S . -B build-sv -G Ninja -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl ^
      -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 ^
      -DFARCRY_BUILD_RENDERER_D3D9=OFF -DFARCRY_BUILD_GAME=OFF
```

## 4. 构建系统结构

```
CMakeLists.txt                  # 顶层：工具链检查、选项、全局设置、子目录
cmake/
  FarCryEngine.cmake            # 公共选项 + farcry_add_engine_module()/executable()
  compat/                       # 编译器/CRT 兼容层
    cry_stl_compat.h            #   强制包含(/FI)：std::hash_map 等旧 STL 名字、<_finddata_t>
    hash_map, hash_set          #   VS 2022 17.10+ 已删除的旧头文件
    afxres.h                    #   .rc 里 #include <afxres.h> 的替代（winres.h）
    atl_crt_shim.cpp            #   老库引用的 ATL::_AtlInitializeCriticalSectionEx
  sources/<Target>.cmake        # 由原 .vcproj 提取的源文件清单（生成后纳入版本库）
<模块>/CMakeLists.txt            # 每个模块：源文件 + 宏定义 + 头文件/库搜索路径 + 链接库
```

`cmake/sources/*.cmake` 里的文件列表是从各个 `.vcproj` 的 `<Files>` 节点提取的
（含 Release 配置的 `ExcludedFromBuild` 处理），与原始工程一一对应。

## 5. 为让 clang 编译通过所做的源码修改

这些修改都保持在“最小、语义等价”的范围内，并写上了注释，便于日后比对：

1. **模板显式特化缺少 `template<>`**
   `Cry_Vector3.h`、`I3DEngine.h`、`_ScriptableEx.h`（`_DECLARE_SCRIPTABLEEX`）、
   `CryPhysics/matrixnm.h`、`polynomial.h`、`CryMovie/AnimSplineTrack.h`，
   以及 `matrixnm.h` / `vectorn.h` 的静态成员池定义（`T x[N]` → `T x[N] = {}`）。

2. **类内 friend 函数依赖 ADL 的写法**
   `GetPlane()`（`Cry_Vector3.h`）和 `GetTranslationMat()`（`Cry_Matrix.h`）原来定义
   在类内部，靠实参类型做 ADL 才能找到；遇到隐式转换（`Matrix33`→`Matrix44`）时
   clang 找不到，现改为命名空间作用域的普通/模板函数。其它隐式转换的调用点
   （`Cry_Camera.h`、`CryBone.cpp`、`CryCharReShadowVolume.cpp`）显式写出转换。

3. **MSVC 才允许的“临时对象”用法**
   - `&SomeStruct()`：clang 报 `-Waddress-of-temporary`（默认是错误）。这类调用只在
     同一个表达式里传指针，安全，因此全局用 `-Wno-address-of-temporary` 关闭；
   - `func(string(...))` 绑定到 `string&`、`EndCall(char*&)` 这类
     `char*`→`const char*&` 的转换：统一改成局部变量，或补上 `char*&` 重载。

4. **内联汇编（`__asm` / `_asm`）**
   clang-cl 只支持部分 MASM 语法，不支持 x87 隐式操作数宽度、跨 `__asm` 块的标号、
   `offset 标号` 等。凡是解析不了的地方加 `#if !defined(CRY_NO_INLINE_ASM)` 并提供可移植
   实现（`cryMemcpy`→`memcpy`、`float2int`→浮点加法、`GetTicks`→`__rdtsc()`、
   `QuaternionExponentX87.cpp` 整体走 `MathUtils.h` 里的 `exp()` 版本）；
   只是缺少操作数宽度的 x87 指令补 `dword ptr`（`FFT_SSE.cpp` 等）。
   `CRY_NO_INLINE_ASM` 由 CMake 在 clang 下自动定义。

5. **C++17 之后被移除的 STL 设施**
   `std::binary_function`（多个模块）→ 去掉基类；
   `std::hash_map` / `<hash_map>` → `cmake/compat` 里的别名与占位头；
   `std::_Construct` / `std::_Destroy` → placement new / 显式析构；
   `gets()` → `fgets()`。

6. **clang 更严格的地方**
   - 多词类型的函数式转换 `unsigned char(x)` → `(unsigned char)(x)`；
   - 隐式 `int` 返回类型（`BindNULL`、`static const s_maxPasses = 3`）；
   - `struct XDOM::IXMLDOMDocument;` → `namespace XDOM { struct IXMLDOMDocument; }`；
   - 变参宏/函数的实参个数（`TRACE(str)` → `TRACE(...)`、`CHECK_LOADED`）以及
     把 `CString` 传给 `printf`（改成 `(const char *)`）；
   - `#pragma message("..." __FUNCTION__)` → 单一字符串字面量；
   - 把 `or` 当变量名（`or` 是 `||` 的替代记号）→ `orPos`；
   - `for (int k…)` 之后继续使用 `k`（旧 MSVC 的 for 作用域泄漏）；
   - 构造函数体内限定调用 `UVSetHeader::UVSetHeader();`（非法）；
   - `new (Type[N])` 这种非法 placement-new → 普通 `new` + `memcpy`。

7. **资源文件（.rc）**
   - `llvm-rc` 无法解析 Lua 调试器的 `TOOLBAR 16, 15` 语法，也处理不好 8 位代码页，
     因此 clang 下 CMake 改用 **Windows SDK 的 `rc.exe`**（并显式把 SDK include 目录
     传给它）；
   - `.rc` 中的 Windows-1252 字符（作者名里的 `ö`）转写为 ASCII；
   - 删除已无意义的 `#pragma code_page(1252)`；
   - `#include <afxres.h>` 通过 `cmake/compat/afxres.h` 转发到 `winres.h`。

8. **链接期问题**
   - `ijl15.lib`（Intel JPEG）不支持 `/SAFESEH` → 全工程统一 `/SAFESEH:NO`；
   - `nvDXTlib.lib` 是 VC6 时代静态库，带 `#DEFAULTLIB:LIBC/libcp`，并引用
     `_printf` / `__vsnprintf` / `ATL::_AtlInitializeCriticalSectionEx` →
     `/NODEFAULTLIB:` + `legacy_stdio_definitions.lib` + `cmake/compat/atl_crt_shim.cpp`；
   - `CrySoundSystem` 补上原工程漏写的 `crysound.lib`。

## 6. 已知限制

* **只支持 Win32（x86）**：D3D9 SDK / Bink / Cg / Ogg / Ubi.com 的预编译库都是 32 位。
* **Editor.exe**：需要 MFC 共享库 + `Editor/XT` 的预编译库，尚未移植（默认关闭）。
* **XRenderOGL.dll**：最老的渲染路径，仍有隐式 `int` 返回、for 作用域等编译错误未清理
  （默认关闭）。D3D9 与 NULL 渲染器已经可以编译链接。
* **ResourceCompilerPC.dll**：大量 ATL `CString` 与 `std::string` 的隐式互转需要逐个改造
  （默认关闭）。
* **CryAISystem**：公开源码缺少 `AIAttribute.cpp`、`AIPlayer.cpp` 等实现文件，
  目标可以配置但无法链接（默认关闭）。
* **Ubi.com / GameSpy 联机**：预编译库与现在的 STL 不兼容，默认关闭
  （会自动定义 `NOT_USE_UBICOM_SDK`）。
* 渲染器里部分内联汇编被替换为等价 C/C++ 代码，性能可能与原版有细微差异。

## 7. 常见问题

**Q: 提示必须 32 位 / `CMAKE_SIZEOF_VOID_P` 报错？**
A: Ninja 生成器忘了加 `-DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32`，
   或 VS 生成器忘了 `-A Win32`。

**Q: 找不到 `rc.exe` / `afxres.h`？**
A: 说明 CMake 回退到了 `llvm-rc`。确认安装了 Windows 10/11 SDK，
   并检查配置输出里的 `RC compiler:` 那一行。

**Q: 链接时报 `ijl15.dll is not compatible with SEH`？**
A: 本工程已统一加 `/SAFESEH:NO`；自行修改 CMake 时请保留该选项。

**Q: `MSB8020: ClangCL 工具集未安装`？**
A: 只有 Visual Studio 生成器 + `-T ClangCL` 才需要 VS 内的 Clang 组件。
   在 VS Installer 里勾选“适用于 Windows 的 C++ Clang 编译器”即可；
   或者直接用 `build-clang.bat`（Ninja + PATH 里的 LLVM，不需要该组件）。

**Q: 能用 MSVC 的 `cl.exe` 编吗？**
A: CMake 工程本身不限制编译器，但 `CRY_NO_INLINE_ASM` 兼容分支只在 clang 下启用；
   用 `cl.exe` 时会走原来的内联汇编路径。

