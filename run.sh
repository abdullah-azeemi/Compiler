
set -e

mkdir -p build
cd build
cmake -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm ..

make
echo "Generating IR..."
/opt/homebrew/opt/llvm/bin/clang -O0 -S -emit-llvm ../test.c -o test.ll

echo "Running Loop Interchange Pass..."
/opt/homebrew/opt/llvm/bin/opt -load-pass-plugin ./libLoopInterchangePass.so -passes=loop-interchange-pass -S test.ll -o transformed.ll

echo "Done! Output is in build/transformed.ll"
