..  Copyright (c) 2023,    

   Distributed under the terms of the MIT license.  

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

General Build Options
---------------------

Building the xeus-karilang library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``xeus-karilang`` build supports the following options:

- ``KARILANG_BUILD_SHARED``: Build the ``xeus-karilang`` shared library. **Enabled by default**.
- ``KARILANG_BUILD_STATIC``: Build the ``xeus-karilang`` static library. **Enabled by default**.


- ``KARILANG_USE_SHARED_XEUS``: Link with a `xeus` shared library (instead of the static library). **Enabled by default**.

Building the kernel
~~~~~~~~~~~~~~~~~~~

The package includes two options for producing a kernel: an executable ``xkarilang`` and a Python extension module, which is used to launch a kernel from Python.

- ``KARILANG_BUILD_EXECUTABLE``: Build the ``xkarilang``  executable. **Enabled by default**.


If ``KARILANG_USE_SHARED_KARILANG`` is disabled, xkarilang  will be linked statically with ``xeus-karilang``.

Building the Tests
~~~~~~~~~~~~~~~~~~

- ``KARILANG_BUILD_TESTS ``: enables the tets  **Disabled by default**.

