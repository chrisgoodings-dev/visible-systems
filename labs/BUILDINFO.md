## Build Info
=============

-----------------------------------------------------
| Build (Release is important for performance labs) |
-----------------------------------------------------

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

-----------------------------------------------------
|                    Run the Labs                   |
-----------------------------------------------------

./build/labs/memory/vs_mem_allocation 200000 30
./build/labs/memory/vs_mem_layout 5000000 20
./build/labs/memory/vs_mem_access 32000000 12 16 12345

-----------------------------------------------------
|                  Build the UB Demos               |
-----------------------------------------------------

cmake -S . -B build-ub -DCMAKE_BUILD_TYPE=Release -DVS_ENABLE_UB_DEMOS=ON
cmake --build build-ub -j
./build-ub/labs/memory/vs_mem_ub

-----------------------------------------------------
|     MAXIMUM VISIBILITY: Compile UB Demos with     | 
|           Sanitizers locally (Clang/ GCC)         |
-----------------------------------------------------

cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=RelWithDebInfo -DVS_ENABLE_UB_DEMOS=ON \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer"
cmake --build build-asan -j
./build-asan/labs/memory/vs_mem_ub

