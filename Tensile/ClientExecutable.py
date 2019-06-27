################################################################################
# Copyright (C) 2019 Advanced Micro Devices, Inc. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell cop-
# ies of the Software, and to permit persons to whom the Software is furnished
# to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IM-
# PLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNE-
# CTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
################################################################################

import itertools
import os
import subprocess
import sys

from . import Common
from .Common import globalParameters

class CMakeEnvironment:
    def __init__(self, sourceDir, buildDir, **options):
        self.sourceDir = sourceDir
        self.buildDir  = buildDir
        self.options = options

    def generate(self):
        args = ['cmake']
        args += itertools.chain.from_iterable([ ['-D', '{}={}'.format(key, value)] for key,value in self.options.items()])
        args += [self.sourceDir]

        Common.print2(' '.join(args))
        subprocess.check_call(args, cwd=Common.ensurePath(self.buildDir))

    def build(self):
        args = ['make', '-j']
        Common.print2(' '.join(args))
        subprocess.check_call(args, cwd=self.buildDir)

    def builtPath(self, path, *paths):
        return os.path.join(self.buildDir, path, *paths)

def clientExecutableEnvironment():
    sourceDir = globalParameters["ClientSourcePath"]
    buildDir = Common.ensurePath(os.path.join(globalParameters["OutputPath"], globalParameters["ClientBuildPath"]))

    options = {'CMAKE_BUILD_TYPE': 'Debug',#globalParameters["CMakeBuildType"],
               'TENSILE_DISABLE_TESTS': 'ON',
               'CMAKE_CXX_COMPILER': '/opt/rocm/bin/hcc'}

    return CMakeEnvironment(sourceDir, buildDir, **options)


buildEnv = None

def getClientExecutable():
    global buildEnv

    if buildEnv is None:
        buildEnv = clientExecutableEnvironment()
        buildEnv.generate()
        buildEnv.build()

    return buildEnv.builtPath("lib/client/tensile_client")
