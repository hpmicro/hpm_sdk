# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
from distutils.log import error
import os
import sys
import re
from tkinter.tix import Tree
#sys.path.insert(0, os.path.abspath('../../'))
HPM_SDK_BASE = os.path.abspath('../../../')
if tags.has("en"):
    HPM_SDK_LANGUAGE = "en"
elif tags.has("zh"):
    HPM_SDK_LANGUAGE = "zh"
else:
    error("Py:LANGUAGE err.")

HPM_DOC_OUTPUT_BASE = f"{HPM_SDK_BASE}/doc/output"
HPM_DOC_OUTPUT_SDK_BASE = f"{HPM_DOC_OUTPUT_BASE}/sdk_doc/{HPM_SDK_LANGUAGE}"
HPM_DOC_OUTPUT_API_BASE = os.environ.get('HPM_DOC_APIDOC_OUTPUT_DIR')
# -- Project information -----------------------------------------------------

project = 'hpm_sdk readme'
copyright = '2020-2022, hpmicro'
author = 'hpmicro'

with open(f"{HPM_SDK_BASE}/VERSION") as f:
    m = re.match(
        (
            r"^VERSION_MAJOR\s*=\s*(\d+)$\n"
            + r"^VERSION_MINOR\s*=\s*(\d+)$\n"
            + r"^PATCHLEVEL\s*=\s*(\d+)$\n"
            + r"^VERSION_TWEAK\s*=\s*\d+$\n"
            + r"^EXTRAVERSION\s*=\s*(.*)$"
        ),
        f.read(),
        re.MULTILINE,
    )

if not m:
    sys.stderr.write("Warning: Could not extract kernel version\n")
    version = "Unknown"
else:
    major, minor, patch, extra = m.groups(1)
    version = ".".join((major, minor, patch))
    if extra:
        version += "-" + extra

# The full version, including alpha/beta/rc tags
release = version

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
# "breathe",
"sphinx.ext.todo",
"sphinx.ext.extlinks",
"sphinx.ext.autodoc",
'sphinx.ext.duration',
'sphinx_inline_tabs',
"sphinx.ext.viewcode",
'myst_parser',
# "breathe",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates'
                    ]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['middleware']
                    
if tags.has("en"):
    master_doc = 'index'
    html_index = 'index.html'
    exclude_patterns.append("**README_zh*")
    exclude_patterns.append("**index_zh*")
elif tags.has("zh"):
    master_doc = 'index_zh'
    html_index = 'index_zh.html'
    exclude_patterns.append("**README.md")
    exclude_patterns.append("**index.md")
    
    
pygments_style = "sphinx"

# -- Options for HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'

html_theme_options = {
    "logo_only": True,
}

html_static_path = [f"{HPM_SDK_BASE}/doc/src/sdk_doc/_static/"]
html_logo = f"{HPM_SDK_BASE}/doc/src/sdk_doc/_static/logo.png"
html_show_sphinx = False

html_title = "HPM SDK Project Documentation"
html_domain_indices = False
html_split_index = True
html_show_sourcelink = False
html_last_updated_fmt = "%b %d, %Y"

html_theme_path =['_themes',]


is_release = tags.has("release") 
version_path = "internal"
if tags.has("publish"):  
    version_path = f"{version}" if is_release else "latest"
else:
    version = "internal"

if version_path == "latest":
    version = "latest"

html_context = {
    "show_license": True,
    "is_release": is_release,
    "current_version": version,
    "versions": (
        ("latest", f"/latest/html/{html_index}"),
    ),

    "reference_links": {
         "API DOC": f"{HPM_DOC_OUTPUT_API_BASE}/html/{html_index}",
    },
    "latest_doc_link": f"/latest/html"
}

path = HPM_DOC_OUTPUT_SDK_BASE
dirs = os.listdir(path)
dirs.sort(key=None, reverse=True)
for dir in dirs:
    if dir != "latest":
        if (dir != "internal") or (version == "internal"):
            html_context['versions']+=(dir, f"/{dir}/html/{html_index}"),

myst_heading_anchors = 2

myst_enable_extensions = [
'colon_fence',
'substitution',
]

myst_substitutions = {
#  "key1": "I'm a **substitution**"
}

myst_number_code_blocks = ['typescript']

suppress_warnings = ['toc.excluded',
                    'toc.not_readable',
                    'toc.secnum','toc.circular','epub.duplicated_toc_entry','autosectionlabel.*',
                    'app.add_source_parser']

def setup(app):
    # theme customizations
    app.add_css_file("css/custom.css")
    app.add_js_file("js/dark-mode-toggle.min.mjs", type="module")
    app.add_js_file("js/ga-tracker.js")
