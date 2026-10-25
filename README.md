# main分支只适配了 KDE 6.3, KDE 新版请看 dev 分支
# Screen Color Filter — 系统级屏幕色彩滤镜（KDE Plasma 6 / KWin 6 · Wayland）

一个**系统全局生效**的 KWin 合成器特效，给整个屏幕叠加一层可自定义**颜色**和**强度**的滤镜，类似 Windows 的颜色滤镜 / f.lux。

- 在 KWin 合成层对**每一个窗口（含桌面、面板、壁纸）**施加像素级颜色变换：
  - **不影响鼠标 / 键盘输入**（只改像素，不拦截事件）；
  - 在 X11 与 Wayland 下都通过 KWin 合成生效（Wayland 同样可用）。
- 通过 **KWin 设置面板**调颜色和强度，并用**全局快捷键**快速开关 / 微调强度。

> 说明：本项目是一个 **C++ KWin 6 特效插件**，需要在你的机器上编译安装（依赖 KWin 开发包）。
> 它基于已验证可工作的 `kwin-colorcorrection-effect`（GPL）改造而来，替换了着色器与控制逻辑。

---

## 1. 安装编译依赖

### Debian 13+ / Ubuntu（Plasma 6）
```bash
sudo apt install git cmake g++ extra-cmake-modules \
     kwin-dev qt6-base-dev qt6-base-dev-tools qt6-base-private-dev \
     libkf6kcmutils-dev libkf6configwidgets-dev libkf6globalaccel-dev \
     libkf6i18n-dev libepoxy-dev libxcb1-dev
```

### Fedora 40+（Plasma 6）
```bash
sudo dnf install git cmake gcc-c++ extra-cmake-modules kwin-devel \
     kf6-kcmutils-devel kf6-kconfigwidgets-devel kf6-kglobalaccel-devel \
     kf6-i18n-devel libepoxy-devel qt6-qtbase-private-devel wayland-devel
```

### Arch Linux
```bash
sudo pacman -S git cmake extra-cmake-modules base-devel kwin qt6-tools \
     kcmutils kconfigwidgets kglobalaccel i18n epoxy libxcb
```

### openSUSE Tumbleweed
```bash
sudo zypper install git cmake gcc-c++ extra-cmake-modules kwin6-devel \
     kf6-kcmutils-devel kf6-kconfigwidgets-devel kf6-kglobalaccel-devel \
     kf6-i18n-devel libepoxy-devel
```

---

## 2. 编译并安装

```bash
git clone <本仓库> kwin-screencolorfilter
cd kwin-screencolorfilter
mkdir build && cd build
cmake .. --install-prefix /usr
make -j$(nproc)
sudo make install
```

安装后重启 KWin（**注销再登录**，或 `kwin_wayland --replace &` / 在 X11 下 `kwin --replace &`）让插件被加载。

### 打包（可选）

在 `build/` 目录下可生成 Debian 包与 tarball：

```bash
cd build
cpack -G DEB    # 生成 kwin-screencolorfilter_<ver>_<arch>.deb（自动探测依赖）
cpack -G TGZ    # 生成 kwin-screencolorfilter-<ver>-Linux.tar.gz
```

安装 deb：`sudo apt install ./kwin-screencolorfilter_*.deb`

---

## 3. 启用与配置

1. 打开 **系统设置 → 工作区行为 → 桌面特效（Desktop Effects）**。
2. 在列表中找到 **Screen Color Filter（屏幕色彩滤镜）**，勾选启用。
3. 点击它旁边的 **配置（齿轮）按钮**，在弹出的设置面板里调整：
   - **滤镜颜色（Filter color）**：选任意颜色（暖色琥珀色 `#ffad66` 适合夜间护眼）；
   - **强度（Intensity）**：0 = 关闭，1 = 最强；
   - **混合模式（Blend mode）**：`叠加`（像戴有色眼镜，向该颜色混合）/ `正片叠底`（按比例压暗对应通道，适合做"暖色夜灯"）。
4. 点击**应用**。

> 也可直接在配置面板搜索"Screen Color Filter"快速定位。

---

## 4. 全局快捷键（无需打开设置面板）

| 快捷键 | 作用 |
| --- | --- |
| `Meta + F` | 一键开关滤镜 |
| `Meta + +` | 增强强度（+0.05） |
| `Meta + -` | 减弱强度（-0.05） |

想改键位：**系统设置 → 快捷键 → 在 KWin / 该特效名下**重新绑定
（对象名：`ScreenColorFilterToggle` / `ScreenColorFilterIncrease` / `ScreenColorFilterDecrease`）。

---

## 5. 命令行启用 / 关闭（可选）

```bash
# 启用
dbus-send --session --dest=org.kde.KWin /Effects org.kde.KWin.Effects.loadEffect string:kwin4_effect_screencolorfilter
# 关闭
dbus-send --session --dest=org.kde.KWin /Effects org.kde.KWin.Effects.unloadEffect string:kwin4_effect_screencolorfilter
# 是否已加载
dbus-send --session --dest=org.kde.KWin /Effects org.kde.KWin.Effects.isEffectLoaded string:kwin4_effect_screencolorfilter
```

---

## 6. 卸载

```bash
cd build
sudo make uninstall
```
然后注销重登录。

---

## 7. 排错 / 注意事项

- **必须在 OpenGL 合成下使用**：特效的 `supported()` 检查 `isOpenGLCompositing()`。
  Plasma 6 默认即 OpenGL；若你切换到 Vulkan 后端，着色器类特效不可用。
- **着色器文件路径**：`Tint.frag` 与 `Tint_core.frag` 会被安装到 `<datadir>/kwin/shaders/`
  （通常是 `/usr/share/kwin/shaders/`）。若滤镜完全无效果，先确认这两个文件都存在。
- **KWin 升级后失效**：KWin 插件依赖当前 KWin 的 ABI，升级 KWin 后请重新 `cmake && make && sudo make install` 一次。
- **多显示器**：每个屏幕的窗口都会被分别着色，无需额外配置。

---

## 8. 项目结构

```
kwin-screencolorfilter/
├── CMakeLists.txt              # 顶层构建（自动适配 Qt5/Qt6，Plasma 6 用 Qt6）+ CPack 打包
├── LICENSE                     # GPL-3.0
├── README.md
└── src/
    ├── CMakeLists.txt
    ├── metadata.json           # 特效元数据（含 X-KDE-ConfigModule 指向 KCM）
    ├── options.kcfg            # KConfigXT 配置定义（颜色 / 强度 / 混合模式）
    ├── ScreenColorFilterConfig.kcfgc
    ├── ScreenColorFilterEffect.{h,cpp}   # 特效主体（OffscreenEffect + 着色器）
    ├── ScreenColorFilterShader.{h,cpp}   # GLSL 着色器封装
    ├── plugin.cpp
    ├── shaders/Tint.frag        # 实际执行色调变换的 GLSL 片段着色器
    │                            # （安装时同时生成 Tint_core.frag 供核心配置使用）
    └── kcm/                     # 桌面特效里的"配置"面板（KCM）
        ├── CMakeLists.txt
        ├── metadata.json
        ├── plugin.cpp
        ├── ScreenColorFilterKCM.{h,cpp}
        ├── ScreenColorFilterKCM.ui
        └── TintColorButton.{h,cpp}  # 内置颜色选择按钮（不依赖 KColorWidgets）
                                     # 通过 kcfg_property 绑定 QColor 属性
```

---

## 9. 致谢 / 许可

改造自 [`kwin-colorcorrection-effect`](https://github.com/lliurex/kwin-colorcorrection-effect)（GPL-3.0，作者 juanma1980 等）。
本项目沿用 **GPL** 许可。
