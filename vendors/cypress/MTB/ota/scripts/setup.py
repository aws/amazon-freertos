import setuptools
from imgtool import imgtool_version

setuptools.setup(
    name="imgtool",
    version=imgtool_version,
    author="The MCUboot committers",
    author_email="dev-mcuboot@lists.runtime.co",
    description=("MCUboot's image signing and key management"),
    license="Apache Software License",
    url="http://github.com/JuulLabs-OSS/mcuboot",
    packages=setuptools.find_packages(),
    install_requires=[
        'cryptography>=2.4.2',
        'intelhex>=2.2.1',
        'click',
    ],
    entry_points={
        "console_scripts": ["imgtool=imgtool.main:imgtool"]
    },
    classifiers=[
        "Programming Language :: Python :: 3",
        "Development Status :: 4 - Beta",
        "Topic :: Software Development :: Build Tools",
        "License :: OSI Approved :: Apache Software License",
    ],
)
