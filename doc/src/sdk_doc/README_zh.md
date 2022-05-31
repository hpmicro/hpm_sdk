# HPM SDK文档生成

## 依赖软件最低版本要求

| 名称   |   版本 |
| -------|---------- |
| Make  |    3.82.90 |
| Python |    3.8 |
| Sphinx |  4.4.0 |
| doxygen | 1.9.3 |

## 安装依赖

- 首先参考 {ref}`SDK快速使用指南 <lab_python_install_depend>`安装依赖章节进行安装，然后执行如下操作:

    :::{tab} Ubuntu
    ``` shell
    apt install make doxygen
    ```

    切换到`hpm_sdk/doc`目录下执行如下指令

    ```shell
    pip install --user -r requirements.txt
    ```

    :::

    :::{tab} Windows
    - 从https://git-scm.com/downloads下载并安装git bash

    以管理员身份打开cmd.exe执行如下命令

    - 安装make

        ```batch
        choco install make doxygen.install
        ```

    - 安装其他依赖
        切换到`hpm_sdk\doc`目录下执行如下指令
        ```batch
        pip install --user -r requirements.txt
        ```
    - 将sphinx-build.exe和doxygen所在的路径加到环境变量PATH中

    :::

- 安装vscode MyST-Markdown插件

## 编译文档

- 设置系统环境变量

    :::{tab} Ubuntu

    Linux:
    参考 {ref}`SDK编译所需环境变量设置<lab_set_hpm_sdk_env>`一节设置Linux下系统环境变量，然后切换到hpm_sdk/doc/src/sdk_doc`目录下

    :::

    :::{tab} Windows

    Windows:
    参考 {ref}`SDK编译所需环境变量设置<lab_set_hpm_sdk_env>`一节设置Windows下系统环境变量，然后切换到hpm_sdk/doc/src/sdk_doc`目录下
    :::

- 编译选项

  - 执行`make latest`编译文档，此时默认编译为英文文档，文档会被标记为出版文档，此时会将文档链接到文档版本库中并作为最新的版本，输出文档默认位置为`HPM_SDK_BASE/doc/output/sdk_doc/en/latest/`，此时命令行会有提示
    ```batch
    The HTML pages are in ..\..\output\sdk_doc\en\latest\html.
    ```
  - 文档语言配置，执行`make latest LANGUAGE=en`输出英文文档，执行`make latest LANGUAGE=zh`输出中文文档,中文文档也会有相应输出提示
    ```
    HTML 页面保存在 ..\..\output\sdk_doc\zh\latest\html 目录
    ```
  - 文档发行，执行`make release`文档会被标记为发行文档，并根据HPM_SDK_BASE/VERSION中的内容自动生成版本号，此时会将文档链接到文档版本库中作为该release版本下的文档，同样生成目录也会发生变化，可观察命令行窗口提示确定位置
  - 文档输出位置可以通过 `DOC_OUTPUT_DIR=$HPM_SDK_BASE/path1`进行配置

  - 清除文档，执行`make clean_latest`或者`make clean_release`即可清除对应的英文文档。若指定`LANGUAGE`则会清楚对应语言的文档
