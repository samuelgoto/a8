# Copyright 2012 the V8 project authors. All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of Google Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

{
  'variables': {
    'v8_code': 1,
    'console%': '',
    # Enable support for Intel VTune. Supported on ia32/x64 only
    'v8_enable_vtunejit%': 0,
    'v8_enable_i18n_support%': 0,
  },
  'includes': ['../build/toolchain.gypi', '../build/features.gypi'],
  'target_defaults':
    {
      'dependencies': [
        '../tools/gyp/v8.gyp:v8',
      ],
      # Generated source files need this explicitly:
      'include_dirs': [
        '../src',
        '/usr/lib/jvm/java-6-openjdk-amd64/include/',
      ],
      'cflags': [
        '-Wno-long-long -fPIC',
      ],
      'sources': [
        'helloworld.cc',
      ],
      'conditions': [
        [ 'console=="readline"', {
          'libraries': [ '-lreadline', ],
          'sources': [ 'd8-readline.cc' ],
        }],
        ['v8_enable_vtunejit==1', {
          'dependencies': [
            '../src/third_party/vtune/v8vtune.gyp:v8_vtune',
          ],
        }],
        ['v8_enable_i18n_support==1', {
          'dependencies': [
            '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
            '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
          ],
        }],
        ['OS=="win" and v8_enable_i18n_support==1', {
          'dependencies': [
            '<(DEPTH)/third_party/icu/icu.gyp:icudata',
          ],
        }],
      ],
    },
  'conditions': [
     ['OS=="linux"', {
        'targets': [
          {
            'target_name': 'helloworld',
            'type': 'executable'
          },
        ],
      }],
     ['OS=="android"', {
        'targets': [
          {
            'target_name': 'helloworld',
            'type': 'shared_library'
          },
        ],
      }],
  ],
   #,
}
