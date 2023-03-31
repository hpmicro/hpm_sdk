# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import os.path
import subprocess
import shutil

os.environ[r'HPM_SDK_BASE'] = os.path.abspath(r'../..')
os.environ[r'DOXYGEN_OUTPUT_DIR'] = os.path.abspath(r'_static/api_doc')

def doxygen_run(hpm_sdk_path):
    shutil.rmtree(os.environ[r'DOXYGEN_OUTPUT_DIR'], ignore_errors=True)
    doxygen_conf = os.path.join(hpm_sdk_path, r'docs/doxygen/Doxyfile')
    doxygen_cmd = r'doxygen ' + doxygen_conf

    return_code, out = subprocess.getstatusoutput(doxygen_cmd)

    if 0 != return_code:
        print("Doxygen failed!!!")
        print(out)

doxygen_run(os.environ[r'HPM_SDK_BASE'])

project = 'HPM_SDK'
copyright = '2020-2023, HPMicro'
author = 'HPMicro software team'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx_rtd_theme',
    'myst_parser',
    "sphinx.ext.todo",
    "sphinx.ext.extlinks",
    'sphinx.ext.duration',
    'sphinx_inline_tabs',
    "sphinx.ext.viewcode",
]

templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []
html_extra_path = []

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

suppress_warnings = ['toc.excluded',
                    'toc.not_readable',
                    'toc.secnum','toc.circular','epub.duplicated_toc_entry','autosectionlabel.*',
                    'app.add_source_parser']

myst_heading_anchors = 2

myst_enable_extensions = [
    "amsmath",
    "colon_fence",
    "deflist",
    "dollarmath",
    "fieldlist",
    "html_admonition",
    "html_image",
    "replacements",
    "smartquotes",
    "strikethrough",
    "substitution",
    "tasklist",
]

html_show_sphinx = False
html_logo = r'../assets/logo.png'



