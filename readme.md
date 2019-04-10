## Compilation

1. modify `CPU_FREQ` to real value in `papi_wrap.cpp`
2. compile as

```
mkdir build && cd build
cmake -DCMAKE_CXX_FLAGS="DCPU_FREQ=<TSC frequency>" ..
make
```



## Usage

1. run target programs as

```
export LD_PRELOAD=<path to libpapicnt.so>
run target program directly
```

2. Results will be store in the working directory. They are

   1. log0_*: all calculation events
   2. log1_*: all communication events
   3. log2_*: relative performance data of calculation
   4. log3_*: relative performance data of communication

   Asterisks in filename are corresponding MPI ranks. 

