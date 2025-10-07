from setuptools import setup, find_packages

with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

with open("requirements.txt", "r", encoding="utf-8") as fh:
    requirements = [line.strip() for line in fh if line.strip() and not line.startswith("#")]

setup(
    name="phantomdb-sqlalchemy",
    version="1.0.0",
    author="PhantomDB Team",
    author_email="phantomdb@example.com",
    description="SQLAlchemy dialect for PhantomDB",
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
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Framework :: SQLAlchemy",
        "Topic :: Database",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.7",
    install_requires=requirements,
    extras_require={
        "dev": [
            "pytest>=6.2.5",
            "pytest-cov>=3.0.0",
            "black>=21.9b0",
            "flake8>=4.0.1",
        ],
    },
)