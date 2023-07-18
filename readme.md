# Vapro

[![issue](https://img.shields.io/github/issues/thu-pacman/VAPRO)](https://github.com/thu-pacman/VAPRO/issues)
![license](https://img.shields.io/github/license/thu-pacman/vapro)

Vapro is a light-weight performance variance detection and diagnosis tool without requiring the source code of applications. It is practical for production-run parallel applications.

## Get started

### Dependencies

- MPI consistent with applications
- jsoncpp
- papi
- libunwind

### Build from source

1. modify `CPU_FREQ` to real value in `papi_wrap.cpp`
2. compile as
```
mkdir build && cd build
cmake ..
make
```

## Usage

We can use Vapro easily with two steps, profiling and analysis.

### Profiling target applications
We can enable Vapro by setting `LD_PRELOAD` to preload the Vapro library before applications start.
```
export LD_PRELOAD=<path_to_libpapicnt.so>
```
Then, we can run the applications dierectly. Alternatively, we can make `LD_PRELOAD` only effective on the applications by
```
LD_PRELOAD=<path_to_libpapicnt.so> mpirun ./application_command
```

### Analyzing results
Vapro saves results in the current working directory. There are four classes of files for different information.

   1. log0_*: all calculation events
   2. log1_*: all communication events
   3. log2_*: relative performance data of calculation
   4. log3_*: relative performance data of communication

   Asterisks in the filenames are corresponding MPI ranks. 


## Roadmap

- Supported CPU backends
  - ✔ PAPI
  - ✔ Linux perf
  - ⬜ [pmu-tools](https://github.com/andikleen/pmu-tools) for Intel CPU
- Supported GPU backends
  - ⬜ CUDA
- ⬜ Integrated visualization

## Contributor Guide

The development of Vapro is based on pull requests on Github. Before requesting for merging, a PR should satisfy the following requirements

1. Receive at least one approval from reviewers.
2. PR title should be concise since it is going to be the commit message in the main branch after merging and squashing.

## Reference

Please cite our papers in your publications if they help your research:

```plaintext
@article{zhai2022detecting,
  title={Detecting Performance Variance for Parallel Applications Without Source Code},
  author={Zhai, Jidong and Zheng, Liyan and Zhang, Feng and Tang, Xiongchao and Wang, Haojie and Yu, Teng and Jin, Yuyang and Song, Shuaiwen Leon and Chen, Wenguang},
  journal={IEEE Transactions on Parallel and Distributed Systems},
  volume={33},
  number={12},
  pages={4239--4255},
  year={2022},
  publisher={IEEE}
}

@inproceedings{zheng2022vapro,
  title={Vapro: Performance variance detection and diagnosis for production-run parallel applications},
  author={Zheng, Liyan and Zhai, Jidong and Tang, Xiongchao and Wang, Haojie and Yu, Teng and Jin, Yuyang and Song, Shuaiwen Leon and Chen, Wenguang},
  booktitle={Proceedings of the 27th ACM SIGPLAN Symposium on Principles and Practice of Parallel Programming},
  pages={150--162},
  year={2022}
}
```
