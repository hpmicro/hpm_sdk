# HPM SDK Doc Generation

## Minimum required version of dependencies

| Name   |   Version |
| -------|---------- |
| Make  |    3.82.90 |
| Python |    3.8 |
| Sphinx |  4.4.0 |
| doxygen |  1.9.3 |

## Install Dependencies

- Refer to the [Install Dependencies](lab_python_install_depend) to install them, and then do the following:

    :::{tab} Ubuntu
    ```shell
    apt install make doxygen
    ```
    
    Switch to the `hpm_sdk/doc` directory and run the following command

    ```shell
    pip install --user -r requirements.txt
    ```
    
    :::

    :::{tab} Windows

    Windows command prompt:
  - Download and install git bash from https://git-scm.com/downloads

  - Install make, open windows cmd console as administrator to install the following packages

    ```batch
    choco install make doxygen.install
    ```

  - Install other dependencies

    Switch to the `hpm_sdk/doc` directory and run the following command
    ```batch
    pip install --user -r requirements.txt
    ```
  - Add path of sphinx-build and doxygen into windows PATH environment variable, make sure these commands can be found.
    :::

- Install vscode MyST-Markdown plugin

## Compile Doc

- Environment Variables
    
    :::{tab} Ubuntu
    
    Linux:
    Refer to [Environment Variables](lab_set_hpm_sdk_env) to set the environment variables under Linux, then switch to hpm_sdk/doc/src/sdk_doc` directory
    
    :::

    :::{tab} Windows
    
    Windows:
    Refer to [Environment Variables](lab_set_hpm_sdk_env) to set the environment variables under Windows, then switch to hpm_sdk/doc/src/sdk_doc` directory
    :::

- Generate Documentation

  - Execute `make latest` to compile the documentation. Output is in English and will be marked as published documents, linked to the document repository as the latest version. The default location of the documentation is `$(HPM_SDK_BASE)/doc/output/sdk_doc/en/latest/`. The following message is expected after executing the command if everything works
    ```batch
    The HTML pages are in ..\..\output\sdk_doc\en\latest\html.
    ```
  - Language configuration, execute `make latest LANGUAGE=en` to output English document, execute `make latest LANGUAGE=zh` to output Chinese document, the command line output the following Chinese prompt
    ```
    HTML 页面保存在 ..\..\output\sdk_doc\zh\latest\html 目录
    ```
  - Release Document, execute `make release` document will be marked as a release document, and according to `$(HPM_SDK_BASE)/VERSION` automatically generate the version number. And it will be linked to the document repository as a document under this version number. The output directory will be named with the specific version.
  - The document output location can be configured with `DOC_OUTPUT_DIR=$HPM_SDK_BASE/path1`.

  - To remove generated documents, use `make clean_latest` or `make clean_release`. `LANGUAGE` can be specified to clean generated documentation for the sepcific language.
