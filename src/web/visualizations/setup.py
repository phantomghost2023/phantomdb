from setuptools import setup, find_packages

setup(
    name="phantomdb-visualizations",
    version="1.0.0",
    description="PhantomDB Data Visualization Tools",
    long_description=open("README.md").read() if open("README.md").read() else "PhantomDB Data Visualization Tools",
    author="PhantomDB Team",
    author_email="team@phantomdb.com",
    url="https://github.com/phantomdb/phantomdb",
    packages=find_packages(),
    install_requires=[
        "flask>=2.0.0",
        "pandas>=1.3.0",
        "numpy>=1.21.0",
    ],
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
        "Topic :: Scientific/Engineering :: Visualization",
    ],
    python_requires=">=3.7",
)