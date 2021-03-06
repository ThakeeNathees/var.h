LICENSE = '''\
//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------
'''

USAGE = '''
#ifdef ___________________USAGE___________________
        -- ENTER THE USAGE HERE --
#endif // ___________________USAGE___________________
'''

TARGET_FILE   = 'single_header.h'
ROOT_HEADER   = 'my_lib.h'
LIB_NAME      = 'YOUR_LIB_NAME'
EXCLUDE_FILES = ['test.cpp']

####################################################

import os, re
src     = dict() ## file_name : source
headers = dict() ## file_name : [source, included?]
		
for dir in os.listdir('.'):
    if os.path.isfile(dir):
        if dir.endswith('.cpp'):
            with open(dir, 'r') as f:
                src[dir] = f.read()
        elif dir.endswith('.h') or dir.endswith('.inc'):
            with open(dir, 'r') as f:
                headers[dir] = [f.read(), False]

gen = headers[ROOT_HEADER][0] ## generated source
done = False
while not done:
    done = True
    for include in re.findall(r'#include ".+"', gen):
        done = False
        file = include[include.find('"')+1:-1]
        if not headers[file][1]:
            gen = gen.replace(include, headers[file][0], 1)
            headers[file][1] = True;
        else:
            gen = gen.replace(include, include.replace('#', '//'))
            
gen += '\n\n//--------------- IMPLEMENTATION -------------------\n\n'

gen += '#if defined(%s_IMPLEMENTATION)\n' % LIB_NAME
for file in src:
    if file in EXCLUDE_FILES: continue
    gen += src[file] + '\n'
gen += '\n#endif //  %s_IMPLEMENTATION' % LIB_NAME

for include in re.findall(r'#include ".+"', gen):
    gen = gen.replace(include, include.replace('#', '//'))
gen = USAGE + '\n' + gen
gen = LICENSE + gen.replace(LICENSE, '')
            
with open(TARGET_FILE, 'w') as f:
    f.write(gen)
print('%s generated' % TARGET_FILE)
