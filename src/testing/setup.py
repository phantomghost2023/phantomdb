from setuptools import setup, find_packages

setup(
    name="phantomdb-testing",
    version="1.0.0",
    description="PhantomDB Testing Framework",
    long_description=open("README.md").read() if open("README.md").read() else "PhantomDB Testing Framework",
    author="PhantomDB Team",
    author_email="team@phantomdb.com",
    url="https://github.com/phantomdb/phantomdb",
    packages=find_packages(),
    install_requires=[
        "phantomdb-python>=1.0.0",
        "pytest>=6.0.0",
        "unittest2>=1.1.0",
    ],
    extras_require={
        "async": ["asyncio>=3.4.3"],
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
        "Topic :: Software Development :: Testing",
    ],
    python_requires=">=3.7",
)