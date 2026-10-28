# QtCourse

Qt 程序设计课程作业仓库（作者：杜恩泽，学号 2024414290208）。

## 作业目录

* [实验 1：带键盘事件的计算器](Experiment1_Calculator/README.md)
* 第 2 周作业：给示例程序增加“关于”按钮（当前仓库根目录）

## 第 2 周作业 · 第一部分：给示例程序增加“关于”按钮

在老师提供的样例程序 `samp2_4` 的基础上完成以下改造：

* 工具栏上“新建”按钮后面新增“关于”按钮（带信息图标）；
* 菜单栏新增“帮助”菜单，里面有“关于”菜单项；
* 单击“关于”按钮（或按 <kbd>F1</kbd>）弹出 Qt 标准的 About 窗口，显示：
  * 开发人员信息：杜恩泽
  * 学号：2024414290208
  * 开发环境：Qt 6.10.3（MinGW 64-bit）+ Qt Creator

### 改动清单

| 文件 | 改动内容 |
| --- | --- |
| `qwmainwind.ui` | 新增 `actAbout` 动作（图标 + 文字 + 快捷键 F1）、工具栏按钮、“帮助”菜单 |
| `qwmainwind.h` | 新增槽函数声明 `on_actAbout_triggered()` |
| `qwmainwind.cpp` | 新增槽函数实现，调用 `QMessageBox::about()` 弹窗 |
| `res.qrc` | 新增图标资源 `images/about.png` |
| `images/about.png` | 新绘制的“关于”按钮图标 |

> 说明：原压缩包里的 `ui_qwmainwind.h` 是 Qt 5.9 时代自动生成的旧文件，会覆盖 uic 新生成的结果导致编译失败，已删除。编译时 Qt 会自动重新生成。

### 开发环境

* Qt 6.10.3（MinGW 64-bit）
* Qt Creator
* 编译器：MinGW-w64 GCC 13.1.0

### 编译运行

方式一：用 Qt Creator 打开 `samp2_4.pro`，选择 “Desktop Qt 6.10.3 MinGW 64-bit” 套件，按 Ctrl+R 运行。

方式二：命令行

```bash
mkdir build && cd build
qmake ..\samp2_4.pro
mingw32-make
```

### 目录结构

```
samp2_4/
├── samp2_4.pro        工程文件
├── main.cpp           程序入口
├── qwmainwind.h/.cpp  主窗口类
├── qwmainwind.ui      界面设计文件（含新增的“关于”动作）
├── res.qrc            资源文件
├── AppIcon.ico        程序图标
└── images/            工具栏图标资源
```
