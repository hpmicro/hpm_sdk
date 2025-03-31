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
import re
import sys
import datetime
from pathlib import Path
import sphinx_rtd_theme

HPM_SDK_BASE = Path(__file__).resolve().parents[2]
HTML_STATIC_DIR = Path(__file__).resolve().parents[0] / "_static"
DOXY_OUT = HTML_STATIC_DIR / "api_doc"

sys.path.insert(0, str(HPM_SDK_BASE / "docs" / "_ext"))
 
os.environ["HPM_SDK_BASE"] = str(HPM_SDK_BASE)
project = 'HPMicro Software Development Kit'
copyright = '2020-%s, HPMicro' % datetime.date.today().year
author = 'HPMicro Software Team'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx_rtd_theme',
    'myst_parser',
    "sphinx.ext.todo",
    "sphinx.ext.extlinks",
    'sphinx.ext.duration',
    "sphinx.ext.viewcode",
    'sphinxcontrib.moderncmakedomain',
    "external_content",
    "doxyrunner",
    'sphinx_tabs.tabs'
]

templates_path = ['_templates']

DOXY_OUT.mkdir(parents = True, exist_ok = True)
doxyrunner_doxygen = os.environ.get("DOXYGEN_EXECUTABLE", "doxygen")
doxyrunner_doxyfile = HPM_SDK_BASE / "docs" / "doxygen" / "Doxyfile" 
doxyrunner_outdir = DOXY_OUT
doxyrunner_outdir_var = "DOXYGEN_OUTPUT_DIR"

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []
html_extra_path = []

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

external_content_contents = [
    (HPM_SDK_BASE,  "CHANGELOG.md"),
    (HPM_SDK_BASE,  "docs/*.rst"),
    (HPM_SDK_BASE / "docs/en", "[!_]*"),
    (HPM_SDK_BASE, "boards/**/*_en.rst",),
    (HPM_SDK_BASE, "boards/**/doc"),
    (HPM_SDK_BASE, "components/**/*_en.rst"),
    (HPM_SDK_BASE, "components/**/doc"),
    (HPM_SDK_BASE, "samples/**/*_en.rst",),
    (HPM_SDK_BASE, "samples/**/doc"),
]


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
html_theme_options = {
    "logo_only": True,
    "prev_next_buttons_location": None
}
html_logo = str(HPM_SDK_BASE / "docs" / "assets" / "logo.png")
html_static_path = [str(HTML_STATIC_DIR)]
html_last_updated_fmt = "%b %d, %Y"
html_domain_indices = False
html_split_index = True
html_show_sphinx = False

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
