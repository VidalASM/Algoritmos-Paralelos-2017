#include <stdio.h>

//Matrix multiplication kernel - thread specification
__global__ void MatrixMulKernel(float *d_M , float *d_N , float *d_P , int Width) {
    //2D Thread ID
    int tx = threadIdx.x;
    int ty = threadIdx.y;

    //Pvalue stores the Pd element that is computed by the thread
    float Pvalue = 0.0;

    for(int k = 0; k < Width ; ++k) {
        float Mdelement = d_M[ty*Width + k];
        float Ndelement = d_N[k*Width + tx];
        Pvalue += (Mdelement*Ndelement);
    }

    d_P[ty*Width + tx] = Pvalue;
}

int main(void) {
    const int Width = 8;
    float h_Ma[Width][Width], h_Mb[Width][Width], h_Mc[Width][Width];
    float *Md, *Nd, *Pd;
    int i, j;
    
    //input in host array
    for (i = 0; i < Width; i++) {
        for (j = 0; j < Width; j++) {
            h_Ma[i][j] = 1 ;
            h_Mb[i][j] = 2 ;
        }
    }

    //MatrixMultiplication(h_Ma, h_Mb, h_Mc, Width);
    int size = (Width*Width)*sizeof(float);

    //Transfer M and N to device memory
    cudaMalloc((void**)&Md, size);
    cudaMemcpy(Md, h_Ma, size, cudaMemcpyHostToDevice);
    cudaMalloc((void**)&Nd, size);
    cudaMemcpy(Nd, h_Mb, size, cudaMemcpyHostToDevice);

    //Allocate P on the device
    cudaMalloc((void**)&Pd,size);

    //Setup the execution configuration
    dim3 dimBlock(Width,Width);
    dim3 dimGrid(1,1);

    //Launch the device computation threads!
    MatrixMulKernel<<<dimGrid,dimBlock>>>(Md,Nd,Pd,Width);

    //Transfer P from device to host
    cudaMemcpy(h_Mc,Pd,size,cudaMemcpyDeviceToHost);

    //Free device matrices
    cudaFree(Md);
    cudaFree(Nd);
    cudaFree(Pd);
    
    //printf the result array
    for (i = 0; i < Width; i++) {
        for (j = 0; j < Width; j++) {
            printf("%f   ", h_Mc[i][j]);
        }
        printf("\n");
    }

    return 0;
}