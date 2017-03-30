#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

const int MAX = 500;
const int n = 500, m = 500, p = 500;

void product(){
    int MAX = 500;

    int A [MAX][MAX], B [MAX][MAX], R [MAX][MAX];

    for (int i = 0; i < MAX ; i ++)
        for (int j = 0; j < MAX ; j ++){
            A [i][j] = rand() % 10 + 1;
            B [i][j] = rand() % 10 + 1;
        }

    ///— Initialize A and x, assign y = 0 —/
    ///— First pair of loops —/

    for (int i = 0; i < MAX ; i ++){
        for (int j = 0; j < MAX ; j ++){
            R[i][j]=0;
            for (int k=1; k<= MAX; k++)
                R[i][j] = R[i][j] + A[i][k]*B[k][j];
        }
    }
}

void product_block(int A [n][m], int B [m][p], int C [n][p]){

    int sum;
    int M = 1;
    int T = sqrt(M);
    for(int I = 0; I < n; I++){
        for(int J = 0; J < p; J++){
            for(int K = 0; K < m; K++){
                C[I][J] = A[I][K] * B[K][J];
                for(int i = I; i < min(I + T, n); i++){
                    for(int j = J; j < min(J+ T, p); j++){
                        sum = 0;
                        for(int k = K; k < min(K + T, m); k++){
                            sum = sum + A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }
    //return 0;
}


int main(){

    //product();

    const int MAX = 500;

    int A [MAX][MAX], B [MAX][MAX], C [MAX][MAX];

    for (int i = 0; i < MAX ; i ++)
        for (int j = 0; j < MAX ; j ++){
            A [i][j] = rand() % 10 + 1;
            B [i][j] = rand() % 10 + 1;
    }

    product_block(A, B, C);

    /*for (int i = 0; i < MAX ; i ++){
        for (int j = 0; j < MAX ; j ++)
            cout << R [i][j] << " ";
        cout << "\n";
    }*/

    return 0;

}