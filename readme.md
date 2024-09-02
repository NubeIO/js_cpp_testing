# Benchmarks

QuickJS 
```
qjs test.js
Complex Computation took 158 milliseconds
Result: -681.7596643536854
```

QuickJS Bytecode
```
 qjsc -o test_compiled test.js
./test_compiled
Complex Computation took 157 milliseconds
Result: -681.7596643536854
```

V8 -cpp17 version
```
Complex Computation took 109 milliseconds
Result: -681.7596643536841
Total time (including parsing) with no JIT: 152 ms

Complex Computation took 22 milliseconds
Result: -681.7596643536841
Total time (including compilation) with JIT: 22 ms
```

Chrome
```
Complex Computation took 20.40000009536743 milliseconds
complex_test.html:31 Result: -681.7596643536885
```

Cpp
```
Complex Computation took 9.85891 milliseconds
Result: -681.76
```

# QuickJS

- Update package list
```
sudo apt update
```

- Install build essentials and CMake
```
sudo apt install -y build-essential cmake
```

- Clone QuickJS repository
```
git clone https://github.com/bellard/quickjs.git
cd quickjs
```
- Build QuickJS
```
make
```

- Install QuickJS
```
sudo make install
```

# V8

- depot_tools
```
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
echo 'export PATH="$PATH:$HOME/depot_tools"' >> ~/.bashrc
source ~/.bashrc
```

```
mkdir v8
cd v8
fetch v8
cd v8

git checkout main
gclient sync
tools/dev/v8gen.py x64.release.sample
ninja -C out.gn/x64.release.sample v8_monolith
```