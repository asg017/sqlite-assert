from setuptools import setup

version = {}
with open("datasette_sqlite_assert/version.py") as fp:
    exec(fp.read(), version)

VERSION = version['__version__']


setup(
    name="datasette-sqlite-assert",
    description="",
    long_description="",
    long_description_content_type="text/markdown",
    author="Alex Garcia",
    url="https://github.com/asg017/sqlite-assert",
    project_urls={
        "Issues": "https://github.com/asg017/sqlite-assert/issues",
        "CI": "https://github.com/asg017/sqlite-assert/actions",
        "Changelog": "https://github.com/asg017/sqlite-assert/releases",
    },
    license="MIT License, Apache License, Version 2.0",
    version=VERSION,
    packages=["datasette_sqlite_assert"],
    entry_points={"datasette": ["sqlite_assert = datasette_sqlite_assert"]},
    install_requires=["datasette", "sqlite-assert"],
    extras_require={"test": ["pytest"]},
    python_requires=">=3.6",
)
