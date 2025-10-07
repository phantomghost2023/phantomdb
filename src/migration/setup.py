"""
Setup script for PhantomDB Migration and Schema Management Utilities
"""

from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

setup(
    name="phantomdb-migration",
    version="1.0.0",
    author="PhantomDB Team",
    author_email="info@phantomdb.com",
    description="Migration and schema management utilities for PhantomDB",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/PhantomDB/phantomdb",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
    ],
    python_requires=">=3.8",
    install_requires=[
        # Dependencies will be added as needed
    ],
    entry_points={
        "console_scripts": [
            "phantomdb-migrate=cli:main",
        ],
    },
)