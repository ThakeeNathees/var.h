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
// USAGE:
//     Note that you must define VAR_IMPLEMENTATION in exactly one source file. Use
//     UNDEF_VAR_DEFINES to undef var.h macros (like DEBUG_BREAK, DEBUG_PRINT...)
//     if you don't want them.

#define VAR_IMPLEMENTATION
#include "var.h"
using namespace varh;

// SAMPLES:
#include <iostream>
#define print(x) std::cout << (x) << std::endl
int main() {
	var v;
	v = 3.14;           print(v); // prints 3.14 float
	v = "hello world!"; print(v); // prints the String

	v = String("string"); v = Vect2f(1.2, 3.4); v = Vect2i(1, 2);
	v = Map(); v = Array(1, 2.3, "hello world!", Array(4, 5, 6));

	class Aclass : public Object {
	public: String to_string() const { return "Aclass"; }
	};
	v = newptr<Aclass>(); print(v); // prints Aclass
}

#endif // ___________________USAGE___________________
'''

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

gen = headers['_var.h'][0] ## generated source
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
            
gen += '\n\n//--------------- VAR_IMPLEMENTATION -------------------\n\n'

gen += '#if defined(VAR_IMPLEMENTATION)\n'
for file in src:
    if file == 'main.cpp': continue
    gen += src[file] + '\n'
gen += '\n#endif // VAR_IMPLEMENTATION'

for include in re.findall(r'#include ".+"', gen):
    gen = gen.replace(include, include.replace('#', '//'))
gen = USAGE + '\n#define VAR_H_HEADER_ONLY\n' + gen
gen = LICENSE + gen.replace(LICENSE, '')
            
with open('var.h', 'w') as f:
    f.write(gen)
print('var.h generated')
