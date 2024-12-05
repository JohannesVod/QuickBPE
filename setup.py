from setuptools import setup, find_packages

setup(
    name='QuickBPE',         
    version='1.8.6',           
    license='MIT',           
    description='A fast BPE implementation in C',
    author='Johannes Voderholzer',
    author_email='invenis2@gmail.com',
    url='https://github.com/JohannesVod/QuickBPE',
    download_url='https://github.com/JohannesVod/QuickBPE/archive/refs/tags/v1.8.1.tar.gz',
    keywords=['BPE', 'LLM', 'tokenization'], 
    install_requires=[
        'numpy',
        'tiktoken',
        'regex',
    ],
    packages=find_packages(),  # Automatically include subpackages
    include_package_data=True, # Include non-Python files specified in MANIFEST.in
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
    ],
)
