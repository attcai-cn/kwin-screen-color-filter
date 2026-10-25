# Screen Color Filter — 系统级屏幕色彩滤镜（KDE Plasma 6 / KWin 6 · Wayland）

一个**系统全局生效**的 KWin 合成器特效，给整个屏幕叠加一层可自定义**颜色**和**强度**的滤镜，类似 Windows 的颜色滤镜 / f.lux。

- 在 KWin 合成层对**每一个窗口（含桌面、面板、壁纸）**施加像素级颜色变换：
  - **不影响鼠标 / 键盘输入**（只改像素，不拦截事件）；
  - 在 X11 与 Wayland 下都通过 KWin 合成生效（Wayland 同样可用）。
- 通过 **KWin 设置面板**调颜色和强度，并用**全局快捷键**快速开关 / 微调强度。

> 说明：本项目是一个 **C++ KWin 6 特效插件**，需要在你的机器上编译安装（依赖 KWin 开发包）。
> 它基于已验证可工作的 `kwin-colorcorrection-effect`（GPL）改造而来，替换了着色器与控制逻辑。
>
> **已验证的构建环境**：Debian forky/sid · CMake 4.3.4 · ECM 6.30 · KWin 6.7.4（`kwin-dev` 4:6.7.4）·
> KF6 6.30 · Qt 6.11。`cmake → make → cpack -G DEB/TGZ → cmake --install` 全流程均已跑通。

---

## 1. 安装编译依赖

### Debian 13+ / Ubuntu 24.10+（Plasma 6）✅ 已验证
```bash
sudo apt install git cmake g++ pkg-config extra-cmake-modules gettext \
     kwin-dev libdrm-dev qt6-base-dev qt6-base-dev-tools \
     libkf6kcmutils-dev libkf6configwidgets-dev libkf6globalaccel-dev \
     libkf6i18n-dev libepoxy-dev libxcb1-dev
```

- `pkg-config` 供 CMake 的 `FindXCB` / `Findepoxy` 使用，`gettext` 供 KI18n 构建流程使用；
- **`libdrm-dev` 必须单独安装**：KWin 的 `KWinConfig.cmake` 会 `find_dependency(Libdrm)`
  （用它自带的 `FindLibdrm.cmake` 查找 `xf86drm.h` 和 `libdrm.so`，并把 `/usr/include/libdrm`
  加进所有编译单元的路径），但 Debian 的 `kwin-dev` 并不依赖 `libdrm-dev`，
  缺少它会在配置阶段直接报 `Could NOT find Libdrm`；
- 本项目**不需要** `qt6-base-private-dev`（不包含任何 Qt 私有头）。

### Fedora 40+（Plasma 6）
```bash
sudo dnf install git cmake gcc-c++ pkgconf extra-cmake-modules kwin-devel \
     libdrm-devel qt6-qtbase-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel \
     kf6-kglobalaccel-devel kf6-ki18n-devel libepoxy-devel
```

### Arch Linux
```bash
sudo pacman -S git cmake extra-cmake-modules base-devel pkgconf kwin libdrm \
     qt6-base qt6-declarative kcmutils kconfigwidgets kglobalaccel ki18n epoxy
```

### openSUSE Tumbleweed
```bash
sudo zypper install git cmake gcc-c++ pkg-config extra-cmake-modules kwin6-devel \
     libdrm-devel qt6-base-devel kf6-kcmutils-devel kf6-kconfigwidgets-devel \
     kf6-kglobalaccel-devel kf6-ki18n-devel libepoxy-devel
```

> Fedora / Arch / openSUSE 的包名按上游命名整理，**未在上述已验证环境中实测**。
> 配置阶段 CMake 会明确提示缺哪个 `find_package`，按提示补装对应 `-devel`/`-dev` 包即可。

---

## 2. 编译并安装

```bash
git clone <本仓库> kwin-screencolorfilter
cd kwin-screencolorfilter

cmake -S . -B build --install-prefix /usr   # 可省略前缀：KDEInstallDirs 默认就是 /usr
cmake --build build -j$(nproc)
sudo cmake --install build                  # 等价于 cd build && sudo make install
```

- CMake 版本较老、不识别 `--install-prefix` 时改用：`cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr`
- 老式写法同样可用：`mkdir build && cd build && cmake .. --install-prefix /usr && make -j$(nproc)`

安装后落盘的文件（Debian/Ubuntu 多架构路径，其他发行版 `/usr/lib` 后的三元组不同）：

| 路径 | 作用 |
| --- | --- |
| `/usr/lib/x86_64-linux-gnu/qt6/plugins/kwin/effects/plugins/kwin4_effect_screencolorfilter.so` | 特效本体（插件 ID） |
| `/usr/lib/x86_64-linux-gnu/qt6/plugins/kwin/effects/configs/kwin_screencolorfilter_config.so` | 桌面特效设置里的配置面板 |
| `/usr/share/kwin/shaders/Tint.frag`、`/usr/share/kwin/shaders/Tint_core.frag` | 着色器（核心配置用 `_core` 副本） |

确认安装结果：

```bash
find /usr/lib /usr/share/kwin -name '*screencolorfilter*' -o -name 'Tint*.frag'
```

安装后**通常无需重启 KWin**：直接用第 5 节的 D-Bus 命令即可加载（KWin 每次 `loadEffect` 都会重新扫描插件目录）。
若命令加载不生效，再注销重新登录一次。

### 打包（可选）

在 `build/` 目录下可生成 Debian 包与 tarball：

```bash
cd build
cpack -G DEB    # kwin-screencolorfilter_1.0.0_amd64.deb（SHLIBDEPS 自动探测运行时依赖）
cpack -G TGZ    # kwin-screencolorfilter-1.0.0-Linux.tar.gz
```

安装 deb：`sudo apt install ./kwin-screencolorfilter_*.deb`
（deb 内含上述 4 个文件，依赖如 `libkwin6 (>= 4:6.7.4)`、`libkf6kcmutils6` 等由 `dpkg-shlibdeps` 自动生成。）

---

## 3. 启用与配置

1. 打开 **系统设置 → 工作区行为 → 桌面特效（Desktop Effects）**。
2. 在列表中找到 **Screen Color Filter（屏幕色彩滤镜）**，勾选启用。
3. 点击它旁边的 **配置（齿轮）按钮**，在弹出的设置面板里调整：
   - **Filter color（滤镜颜色）**：选任意颜色（暖色琥珀色 `#ffad66` 适合夜间护眼，也是默认值）；
   - **Intensity（强度）**：0 = 关闭，1 = 最强，步进 0.05；
   - **Blend mode（混合模式）**：`Overlay (mix toward color)`（叠加，像戴有色眼镜，向该颜色混合）/
     `Multiply (darken channels)`（正片叠底，按比例压暗对应通道，适合做"暖色夜灯"）。
4. 点击**应用**（保存后 KCM 会自动 `unloadEffect` + `loadEffect` 让滤镜立即生效）。

> 也可直接在配置面板搜索 "Screen Color Filter" 快速定位。

配置写在 `kwinrc` 的 `[Effect-ScreenColorFilter]` 组：

```ini
[Effect-ScreenColorFilter]
tintColor=#ffad66
intensity=0.35
blendMode=0
```

---

## 4. 全局快捷键（无需打开设置面板）

| 快捷键 | 作用 | 动作对象名 |
| --- | --- | --- |
| `Meta + F` | 一键开关滤镜 | `ScreenColorFilterToggle` |
| `Meta + +` | 增强强度（+0.05，上限 1） | `ScreenColorFilterIncrease` |
| `Meta + -` | 减弱强度（-0.05，下限 0） | `ScreenColorFilterDecrease` |

强度调整直接写 `kwinrc` 并立即重载，可与设置面板共存。
想改键位：**系统设置 → 键盘 → 快捷键**，搜索上面的对象名（`ScreenColorFilter*`）重新绑定，
它们通常显示在 KWin / Screen Color Filter 分组下。

---

## 5. 命令行启用 / 关闭（可选）

```bash
# 启用
dbus-send --session --dest=org.kde.KWin /Effects org.kde.kwin.Effects.loadEffect string:kwin4_effect_screencolorfilter
# 关闭
dbus-send --session --dest=org.kde.KWin /Effects org.kde.kwin.Effects.unloadEffect string:kwin4_effect_screencolorfilter
# 是否已加载（返回 boolean true / false）
dbus-send --session --print-reply --dest=org.kde.KWin /Effects org.kde.kwin.Effects.isEffectLoaded string:kwin4_effect_screencolorfilter
# 本机是否支持（安装路径不对 / ABI 不匹配时返回 false）
dbus-send --session --print-reply --dest=org.kde.KWin /Effects org.kde.kwin.Effects.isEffectSupported string:kwin4_effect_screencolorfilter
# 改完 kwinrc 后让特效重新读取配置
dbus-send --session --dest=org.kde.KWin /Effects org.kde.kwin.Effects.reconfigureEffect string:kwin4_effect_screencolorfilter
```

> ⚠️ Plasma 6 的接口名是 **`org.kde.kwin.Effects`（`kwin` 小写）**，目标名才是 `org.kde.KWin`。
> 写成 `org.kde.KWin.Effects` 会报 `org.freedesktop.DBus.Error.UnknownInterface`。
> 其他可用方法：`toggleEffect`、`areEffectsSupported`、`activeEffects`（属性）等。

---

## 6. 卸载

```bash
cd build
sudo make uninstall        # 或 sudo cmake --build . --target uninstall
```

然后 `dbus-send ... unloadEffect` 关掉已加载的特效，或注销重新登录。

---

## 7. 排错 / 注意事项

- **必须在 OpenGL 合成下使用**：特效的 `supported()` 检查 `isOpenGLCompositing()`。
  Plasma 6 默认即 OpenGL；若你切换到 Vulkan 后端，着色器类特效不可用。
- **配置阶段报 `Could NOT find Libdrm`（missing: Libdrm_LIBRARY Libdrm_INCLUDE_DIR）**：
  安装 `libdrm-dev`（Fedora `libdrm-devel`、Arch `libdrm`、openSUSE `libdrm-devel`）。
  KWin 的 `KWinConfig.cmake` 会 `find_dependency(Libdrm)`，其 `FindLibdrm.cmake` 需要
  `xf86drm.h` 和 `libdrm.so`，但 `kwin-dev` 本身并不依赖 `libdrm-dev`。
- **着色器文件路径**：`Tint.frag` 与 `Tint_core.frag` 会被安装到 `<datadir>/kwin/shaders/`
  （通常是 `/usr/share/kwin/shaders/`）。若滤镜完全无效果，先确认这两个文件都存在。
  KWin 加载着色器失败时会在 `journalctl --user -b` 里打印 `ScreenColorFilter:` 或 `kwin_gl` 相关错误。
- **特效不出现在列表里 / `isEffectSupported` 返回 false**：
  确认 `kwin4_effect_screencolorfilter.so` 位于 `.../qt6/plugins/kwin/effects/plugins/` 下
  （用第 2 节的 `find` 命令检查），且与当前 KWin 版本 ABI 匹配。
- **配置面板打不开**：确认 `kwin_screencolorfilter_config.so` 位于
  `.../qt6/plugins/kwin/effects/configs/` 下（`metadata.json` 里的 `X-KDE-ConfigModule` 必须与文件名一致）。
- **KWin 升级后失效**：KWin 插件依赖当前 KWin 的 ABI，升级 KWin 后请重新
  `cmake --build build && sudo cmake --install build` 一次。
- **从旧版本源码升级**：当前代码已适配 KWin 6.7 的 API（`GLShader::isValid()` 已被移除、
  不再链接 `Qt6::CorePrivate`），改动后请删除 `build/` 重新 `cmake` 一次。
- **多显示器**：每个屏幕的窗口都会被分别着色，无需额外配置。

---

## 8. 项目结构

```
kwin-screencolorfilter/
├── CMakeLists.txt              # 顶层构建（自动适配 Qt5/Qt6，Plasma 6 用 Qt6）+ CPack 打包
├── LICENSE                     # GPL-3.0
├── README.md
└── src/
    ├── CMakeLists.txt          # 特效插件目标 + 着色器安装（Tint.frag / Tint_core.frag）
    ├── metadata.json           # 特效元数据（含 X-KDE-ConfigModule 指向 KCM）
    ├── options.kcfg            # KConfigXT 配置定义（颜色 / 强度 / 混合模式，kwinrc 组 Effect-ScreenColorFilter）
    ├── ScreenColorFilterConfig.kcfgc
    ├── ScreenColorFilterEffect.{h,cpp}   # 特效主体（OffscreenEffect + 重定向窗口）
    ├── ScreenColorFilterShader.{h,cpp}   # GLSL 着色器封装（ShaderManager + ShaderBinder）
    ├── plugin.cpp              # KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED 工厂
    ├── shaders/Tint.frag        # 实际执行色调变换的 GLSL 片段着色器
    │                            # （安装时同时生成 Tint_core.frag 供核心配置使用）
    └── kcm/                     # 桌面特效里的"配置"面板（KCM）
        ├── CMakeLists.txt
        ├── metadata.json
        ├── plugin.cpp
        ├── ScreenColorFilterKCM.{h,cpp}
        ├── ScreenColorFilterKCM.ui        # 颜色 / 强度 / 混合模式三个分组
        └── TintColorButton.{h,cpp}  # 内置颜色选择按钮（不依赖 KColorWidgets）
                                     # 通过 kcfg_property 绑定 QColor 属性
```

构建产物（均被 `.gitignore` 忽略）：`build/bin/kwin/effects/{plugins,configs}/*.so`、
`build/kwin-screencolorfilter_1.0.0_amd64.deb`、`build/kwin-screencolorfilter-1.0.0-Linux.tar.gz`。

---

## 9. 致谢 / 许可

改造自 [`kwin-colorcorrection-effect`](https://github.com/lliurex/kwin-colorcorrection-effect)（GPL-3.0，作者 juanma1980 等）。
架构参照 KWin 内置的 `colorblindnesscorrection` 特效。
本项目沿用 **GPL** 许可。
