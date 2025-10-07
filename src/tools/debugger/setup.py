from setuptools import setup, find_packages

setup(
    name="phantomdb-debugger",
    version="1.0.0",
    description="PhantomDB Query Debugger and Profiler",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    author="PhantomDB Team",
    author_email="team@phantomdb.com",
    url="https://github.com/phantomdb/phantomdb",
    packages=find_packages(),
    install_requires=[
        "requests>=2.25.1",
    ],
    entry_points={
        "console_scripts": [
            "phantomdb-debugger=debugger.phantomdb_debugger:main",
        ],
    },
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Database",
        "Topic :: Software Development :: Debuggers",
    ],
    python_requires=">=3.7",
)