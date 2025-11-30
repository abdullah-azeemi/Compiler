
set -e

mkdir -p build
cd build
cmake -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm ..

make
echo "Generating IR..."
/opt/homebrew/opt/llvm/bin/clang -O1 -fno-unroll-loops -S -emit-llvm ../test.c -o test.ll

echo "Running Loop Interchange Pass..."
/opt/homebrew/opt/llvm/bin/opt -disable-verify -load-pass-plugin ./libLoopInterchangePass.so -passes='default<O1>,function(loop-interchange-pass)' -S test.ll -o transformed.ll 2>&1 | tee pass_output.txt

echo "Done! Output is in build/transformed.ll"
