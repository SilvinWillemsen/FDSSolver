#include <stdio.h>
void updateEq(double* uNext, double* u, double* uPrev, double* coeffs, int Nx)
{

 for (int l = 2; l < 17; ++l) 
 {for (int m = 2; m < 17; ++m)
 { 
    uNext[l + m * Nx] = - coeffs[6] * (u[l-1 + (m-1) * Nx] + u[l+1 + (m+1) * Nx] + u[l+1 + (m-1) * Nx] + u[l-1 + (m+1) * Nx]) - coeffs[2] * (u[l + (m-2) * Nx] + u[l + (m+2) * Nx] + u[l+2 + m * Nx] + u[l-2 + m * Nx]) - coeffs[7] * (u[l + (m-1) * Nx] + u[l + (m+1) * Nx] + u[l+1 + m * Nx] + u[l-1 + m * Nx]) - coeffs[12] * u[l + m * Nx] - coeffs[37] * uPrev[l + m * Nx]; 
 } 
 }
}