#ifndef __CUDAHELP_CUH
#define __CUDAHELP_CUH
// CUDA runtime
#include <cuda_runtime.h>
// Includes, Project
#include <cufft.h>
#include <helper_cuda.h>
#include <helper_functions.h>  // helper utility functions

void cudaAssert(cudaError_t code, const char* file, int line);
void cufftAssert(cufftResult err, const char* file, const int line);

#define cudaErrchk(ans) { cudaAssert((ans), __FILE__, __LINE__); }
#define cufftErrchk(err) { cufftAssert((err), __FILE__, __LINE__); }
#endif // !__CUDAHELP_H






