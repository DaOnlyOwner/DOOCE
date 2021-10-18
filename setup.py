import sys, os, io
from setuptools import find_packages


# import skbuild and throw a meaningful error if it's not installed
try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n"
        " or you need to install the PEP 518 requirements in pyproject.toml yourself",
        file=sys.stderr,
    )
    raise


# define configurable variables
PACKAGE_NAME = "dooce"
PACKAGE_VERSION = "1.0.0"
DESCRIPTION = "chess engine"
PROJECT_URL = "https://github.com/DaOnlyOwner/DOOCE"
AUTHOR = "DaOnlyOwner"
AUTHOR_EMAIL = ""
PYTHON_VERSION = '>=3.0.0'
DEPENDENCIES = []


def load_readme_description():

    long_description = None
    here = os.path.abspath(os.path.dirname(__file__))

    try:
        with io.open(os.path.join(here, 'README.md'), encoding='utf-8') as f:
            long_description = '\n' + f.read()
    except FileNotFoundError:
        long_description = DESCRIPTION

    return long_description


# compile the CMake project to a python wheel
setup(
    name = PACKAGE_NAME,
    version = PACKAGE_VERSION,
    description = DESCRIPTION,
    long_description = load_readme_description(),
    long_description_content_type = 'text/markdown',
    url = PROJECT_URL,
    author = AUTHOR,
    author_email = AUTHOR_EMAIL,
    python_requires = PYTHON_VERSION,
    install_requires = DEPENDENCIES,
    packages=find_packages(where = 'src'),
    package_dir={"": "src"},
    cmake_install_dir="src/dooce",
    #extras_require={"test": ["pytest"]}, # TODO: add this for end-to-end unit tests
)
