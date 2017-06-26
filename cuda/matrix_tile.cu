#include <stdio.h>
#include <math.h>

#define TILE_WIDTH 2

__global__
void MatrixMulKernel(float *d_M , float *d_N , float *d_P , int Width) {
	__shared__ float Mds[TILE_WIDTH][TILE_WIDTH];
	__shared__ float Nds[TILE_WIDTH][TILE_WIDTH];

	int bx = blockIdx.x; int by = blockIdx.y;
	int tx = threadIdx.x; int ty = threadIdx.y;

	// Identify the row and column of the d_P element to work on
	int Row = by * TILE_WIDTH + ty;
	int Col = bx * TILE_WIDTH + tx;

	float Pvalue = 0;
	// Loop over the d_M and d_N tiles required to compute d_P element
	// ph indicate number of phase
	for (int ph = 0; ph < ceil(Width/(float)TILE_WIDTH); ++ph) {
		
		// Collaborative loading of d_M and d_N tiles into shared memory
		if ((Row < Width) && ((ph*TILE_WIDTH + tx) < Width))
			Mds[ty][tx] = d_M[Row*Width + ph*TILE_WIDTH + tx];
		else Mds[ty][tx] = 0.0;
		if (((ph*TILE_WIDTH + ty) < Width) && (Col < Width))
			Nds[ty][tx] = d_N[(ph*TILE_WIDTH + ty)*Width + Col];
		else Nds[ty][tx] = 0.0;
		
		__syncthreads(); // for syncronizeing the threads

		for (int k = 0; k < TILE_WIDTH; ++k) {
			Pvalue += Mds[ty][k] * Nds[k][tx];
		}
		__syncthreads(); // for syncronizeing the threads
	}
	if ((Row < Width) && (Col < Width))
		d_P[Row*Width + Col] = Pvalue;
}

int main()
{
	const int Width = 6;
	float h_Ma[Width][Width], h_Mb[Width][Width], h_Mc[Width][Width];
	float *d_Ma, *d_Mb, *d_Mc; // device array
	int i, j;

	/*h_Ma = (float**) malloc((Width)*sizeof(float));
	h_Mb = (float**) malloc((Width)*sizeof(float));
	h_Mc = (float**) malloc((Width)*sizeof(float));
	for (i = 0; i < Width; i++) {
		h_Ma[i] = (float*) malloc((Width)*sizeof(float));
		h_Mb[i] = (float*) malloc((Width)*sizeof(float));
		h_Mc[i] = (float*) malloc((Width)*sizeof(float));
	}*/
	
	//input in host array
	for (i = 0; i < Width; i++)	{
		for (j = 0; j < Width; j++) {
			h_Ma[i][j] = 1 ;
			h_Mb[i][j] = 2 ;
		}
	}

	int size = (Width*Width)*sizeof(float);

	//create device array cudaMalloc ( (void **)&array_name, sizeofmatrixinbytes);
	cudaMalloc((void **) &d_Ma, size);
	cudaMalloc((void **) &d_Mb, size);

	//copy host array to device array; cudaMemcpy ( dest , source , WIDTH , direction )
	cudaMemcpy(d_Ma, h_Ma, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_Mb, h_Mb, size, cudaMemcpyHostToDevice);

	//allocating memory for resultent device array
	cudaMalloc((void **) &d_Mc, size);

	//calling kernal
	dim3 dimGrid(Width/TILE_WIDTH, Width/TILE_WIDTH, 1);
	dim3 dimBlock(TILE_WIDTH, TILE_WIDTH, 1);

	MatrixMulKernel<<<dimGrid,dimBlock>>>(d_Ma, d_Mb, d_Mc, Width);

	// all gpu function blocked till kernel is working
	//copy back result_array_d to result_array_h
	cudaMemcpy(h_Mc, d_Mc, size, cudaMemcpyDeviceToHost);

	//printf the result array
	for (i = 0; i < Width; i++) {
		for (j = 0; j < Width; j++) {
			printf("%f   ", h_Mc[i][j]);
		}
		printf("\n");
	}

	cudaFree(d_Ma);
	cudaFree(d_Mb);
	cudaFree(d_Mc);
	
	//system("pause") ;
	return 0;
}