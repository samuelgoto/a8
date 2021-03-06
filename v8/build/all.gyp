# Copyright 2011 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        '../preparser/preparser.gyp:*',
        '../samples/samples.gyp:*',
        '../src/d8.gyp:d8',
        '../src/helloworld.gyp:helloworld',
        '../test/cctest/cctest.gyp:*',
      ],
    }
  ]
}

