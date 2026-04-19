// Switchable-precision interface template for cufinufft. Used by cufinufft.h
// Internal use only: users should link to cufinufft.h

#if (!defined(__CUFINUFFT_H__) && !defined(SINGLE)) || \
  (!defined(__CUFINUFFTF_H__) && defined(SINGLE))
// (note we entered one level of conditional until the end of this header)
// Make sure we don't include double or single headers more than once...

#include <cstdlib>
#include <cufft.h>
#include <assert.h>
#include <cuda_runtime.h>
#include "cufinufft_opts.h"
#include "../source/precision_independent.h"
#include "cufinufft_errors.h"

#include "../contrib/utils.h"
#include "../contrib/dataTypes.h"
#include "../contrib/spreadinterp.h"
#include "../contrib/utils_fp.h"


#ifndef SINGLE
	#define __CUFINUFFT_H__
#else
	#define __CUFINUFFTF_H__
#endif


/* Undefine things so we don't get warnings/errors later */
#undef CUFINUFFT_DEFAULT_OPTS
#undef CUFINUFFT_MAKEPLAN
#undef CUFINUFFT_SETPTS
#undef CUFINUFFT_EXECUTE
#undef CUFINUFFT_DESTROY
#undef CUFINUFFT1D1_EXEC
#undef CUFINUFFT1D2_EXEC
#undef CUFINUFFT2D1_EXEC
#undef CUFINUFFT2D2_EXEC
#undef CUFINUFFT3D1_EXEC
#undef CUFINUFFT3D2_EXEC
#undef SETUP_BINSIZE
/* memtransfer.h */
#undef ALLOCGPUMEM1D_PLAN
#undef ALLOCGPUMEM1D_NUPTS
#undef FREEGPUMEMORY1D
#undef ALLOCGPUMEM2D_PLAN
#undef ALLOCGPUMEM2D_NUPTS
#undef FREEGPUMEMORY2D
#undef ALLOCGPUMEM3D_PLAN
#undef ALLOCGPUMEM3D_NUPTS
#undef FREEGPUMEMORY3D
/* spreading and interp only*/
#undef CUFINUFFT_SPREAD1D
#undef CUFINUFFT_SPREAD2D
#undef CUFINUFFT_SPREAD3D
#undef CUFINUFFT_INTERP1D
#undef CUFINUFFT_INTERP2D
#undef CUFINUFFT_INTERP3D
/* spreading 1D */
#undef CUSPREAD1D
#undef CUSPREAD1D_NUPTSDRIVEN_PROP
#undef CUSPREAD1D_NUPTSDRIVEN
#undef CUSPREAD1D_SUBPROB_PROP
#undef CUSPREAD1D_SUBPROB
/* spreading 2D */
#undef CUSPREAD2D
#undef CUSPREAD2D_NUPTSDRIVEN_PROP
#undef CUSPREAD2D_NUPTSDRIVEN
#undef CUSPREAD2D_SUBPROB_PROP
#undef CUSPREAD2D_SUBPROB
#undef CUSPREAD2D_PAUL
#undef CUSPREAD2D_PAUL_PROP
/* spreading 3d */
#undef CUSPREAD3D
#undef CUSPREAD3D_NUPTSDRIVEN_PROP
#undef CUSPREAD3D_NUPTSDRIVEN
#undef CUSPREAD3D_BLOCKGATHER_PROP
#undef CUSPREAD3D_BLOCKGATHER
#undef CUSPREAD3D_SUBPROB_PROP
#undef CUSPREAD3D_SUBPROB
/* interp */
#undef CUINTERP1D
#undef CUINTERP2D
#undef CUINTERP3D
#undef CUINTERP1D_NUPTSDRIVEN
#undef CUINTERP2D_NUPTSDRIVEN
#undef CUINTERP2D_SUBPROB
#undef CUINTERP3D_NUPTSDRIVEN
#undef CUINTERP3D_SUBPROB
/* deconvolve */
#undef CUDECONVOLVE1D
#undef CUDECONVOLVE2D
#undef CUDECONVOLVE3D
/* structs */
#undef CUFINUFFT_PLAN_S
#undef CUFINUFFT_PLAN
/* fseries kernel */
#undef CUFSERIESKERNELCOMPUTE

#ifdef SINGLE

#define CUFINUFFT_DEFAULT_OPTS cufinufftf_default_opts
#define CUFINUFFT_MAKEPLAN cufinufftf_makeplan
#define CUFINUFFT_SETPTS cufinufftf_setpts
#define CUFINUFFT_EXECUTE cufinufftf_execute
#define CUFINUFFT_DESTROY cufinufftf_destroy
#define CUFINUFFT1D1_EXEC cufinufftf1d1_exec
#define CUFINUFFT1D2_EXEC cufinufftf1d2_exec

#define CUFINUFFT2D1_EXEC cufinufftf2d1_exec
#define CUFINUFFT2D2_EXEC cufinufftf2d2_exec
#define CUFINUFFT3D1_EXEC cufinufftf3d1_exec
#define CUFINUFFT3D2_EXEC cufinufftf3d2_exec
#define SETUP_BINSIZE setup_binsizef
/* memtransfer.h */
#define ALLOCGPUMEM1D_PLAN allocgpumem1df_plan
#define ALLOCGPUMEM1D_NUPTS allocgpumem1df_nupts
#define FREEGPUMEMORY1D freegpumemory1df
#define ALLOCGPUMEM2D_PLAN allocgpumem2df_plan
#define ALLOCGPUMEM2D_NUPTS allocgpumem2df_nupts
#define FREEGPUMEMORY2D freegpumemory2df
#define ALLOCGPUMEM3D_PLAN allocgpumem3df_plan
#define ALLOCGPUMEM3D_NUPTS allocgpumem3df_nupts
#define FREEGPUMEMORY3D freegpumemory3df
/* spreading and interp only*/
#define CUFINUFFT_SPREAD1D cufinufft_spread1df
#define CUFINUFFT_SPREAD2D cufinufft_spread2df
#define CUFINUFFT_SPREAD3D cufinufft_spread3df
#define CUFINUFFT_INTERP1D cufinufft_interp1df
#define CUFINUFFT_INTERP2D cufinufft_interp2df
#define CUFINUFFT_INTERP3D cufinufft_interp3df
/* spreading 1D */
#define CUSPREAD1D cuspread1df
#define CUSPREAD1D_NUPTSDRIVEN_PROP cuspread1df_nuptsdriven_prop
#define CUSPREAD1D_NUPTSDRIVEN cuspread1df_nuptsdriven
#define CUSPREAD1D_SUBPROB_PROP cuspread1df_subprob_prop
#define CUSPREAD1D_SUBPROB cuspread1df_subprob
/* spreading 2D */
#define CUSPREAD2D cuspread2df
#define CUSPREAD2D_NUPTSDRIVEN_PROP cuspread2df_nuptsdriven_prop
#define CUSPREAD2D_NUPTSDRIVEN cuspread2df_nuptsdriven
#define CUSPREAD2D_SUBPROB_PROP cuspread2df_subprob_prop
#define CUSPREAD2D_SUBPROB cuspread2df_subprob
#define CUSPREAD2D_PAUL cuspread2df_paul
#define CUSPREAD2D_PAUL_PROP cuspread2df_paul_prop
/* spreading 3d */
#define CUSPREAD3D cuspread3df
#define CUSPREAD3D_NUPTSDRIVEN_PROP cuspread3df_nuptsdriven_prop
#define CUSPREAD3D_NUPTSDRIVEN cuspread3df_nuptsdriven
#define CUSPREAD3D_BLOCKGATHER_PROP cuspread3df_blockgather_prop
#define CUSPREAD3D_BLOCKGATHER cuspread3df_blockgather
#define CUSPREAD3D_SUBPROB_PROP cuspread3df_subprob_prop
#define CUSPREAD3D_SUBPROB cuspread3df_subprob
/* interp */
#define CUINTERP1D cuinterp1df
#define CUINTERP2D cuinterp2df
#define CUINTERP3D cuinterp3df
#define CUINTERP1D_NUPTSDRIVEN cuinterp1df_nuptsdriven
#define CUINTERP2D_NUPTSDRIVEN cuinterp2df_nuptsdriven
#define CUINTERP2D_SUBPROB cuinterp2df_subprob
#define CUINTERP3D_NUPTSDRIVEN cuinterp3df_nuptsdriven
#define CUINTERP3D_SUBPROB cuinterp3df_subprob
/* deconvolve */
#define CUDECONVOLVE1D cudeconvolve1df
#define CUDECONVOLVE2D cudeconvolve2df
#define CUDECONVOLVE3D cudeconvolve3df
/* structs */
#define CUFINUFFT_PLAN_S cufinufftf_plan_s
#define CUFINUFFT_PLAN cufinufftf_plan
/* fseries kernel */
#define CUFSERIESKERNELCOMPUTE cufserieskernelcompute_f
#endif

typedef struct cufinufftf_plan_s {
	cufinufft_opts  opts;
	SPREAD_OPTS     spopts;

	int type;
	int dim;
	int M;
	int nf1;
	int nf2;
	int nf3;
	int ms;
	int mt;
	int mu;
	int ntransf;
	int maxbatchsize;
	int iflag;

	int totalnumsubprob;
	int byte_now;
	FLT *fwkerhalf1;
	FLT *fwkerhalf2;
	FLT *fwkerhalf3;

	FLT *kx;
	FLT *ky;
	FLT *kz;
	CUCPX *c;
	CUCPX *fw;
	CUCPX *fk;

	// Arrays that used in subprob method
	int *idxnupts;    //length: #nupts, index of the nupts in the bin-sorted order
	int *sortidx;     //length: #nupts, order inside the bin the nupt belongs to
	int *numsubprob;  //length: #bins,  number of subproblems in each bin
	int *binsize;     //length: #bins, number of nonuniform ponits in each bin
	int *binstartpts; //length: #bins, exclusive scan of array binsize
	int *subprob_to_bin;//length: #subproblems, the bin the subproblem works on 
	int *subprobstartpts;//length: #bins, exclusive scan of array numsubprob
	// Extra arrays for Paul's method
	int *finegridsize;
	int *fgstartpts;
	// Arrays for 3d (need to sort out)
	int *numnupts;
	int *subprob_to_nupts;

	cufftHandle fftplan;
	cudaStream_t *streams;

} cufinufftf_plan_s;

//The plan that is passed around is a pointer to a struct.
//makeplan will utilize a double pointer.
//This encourages bindings to treat the struct as opaque.
typedef struct cufinufftf_plan_s* cufinufftf_plan;


/* We include common.h here because it depends on SPREAD_OPTS and
   CUFINUFFT_PLAN_S structs being completely defined first. */
#include "../contrib/common.h"

#define checkCufftErrors(call)

#ifdef __cplusplus
extern "C" {
#endif
	int cufinufftf_default_opts(int type, int dim, cufinufft_opts* opts);
	int cufinufftf_makeplan(int type, int dim, int* n_modes, int iflag, int ntransf, float tol, int maxbatchsize, cufinufftf_plan* d_plan_ptr, cufinufft_opts* opts);
	int cufinufftf_setpts(int M, float* h_kx, float* h_ky, float* h_kz, int N, float* h_s, float* h_t, float* h_u, cufinufftf_plan d_plan);
	int cufinufftf_execute(CUCPX* h_c, CUCPX* h_fk, cufinufftf_plan d_plan);
	int cufinufftf_destroy(cufinufftf_plan d_plan);
#ifdef __cplusplus
}
#endif


// 1d
int cufinufftf1d1_exec(CUCPX* d_c, CUCPX* d_fk, CUFINUFFT_PLAN d_plan);
int cufinufftf1d2_exec(CUCPX* d_c, CUCPX* d_fk, CUFINUFFT_PLAN d_plan);

// 2d
int CUFINUFFT2D1_EXEC(CUCPX* d_c, CUCPX* d_fk, CUFINUFFT_PLAN d_plan);
int CUFINUFFT2D2_EXEC(CUCPX* d_c, CUCPX* d_fk, CUFINUFFT_PLAN d_plan);

// 3d
int CUFINUFFT3D1_EXEC(CUCPX* d_c, CUCPX* d_fk, CUFINUFFT_PLAN d_plan);
int CUFINUFFT3D2_EXEC(CUCPX* d_c, CUCPX* d_fk, CUFINUFFT_PLAN d_plan);

#endif
