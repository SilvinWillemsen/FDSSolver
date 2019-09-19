#include <stdio.h>
void updateEq(double* uNext, double* u, double* uPrev, double* coeffs)
{

 for (int l = 2; l < 67; ++l)
 { 
    uNext[l] = - coeffs[0] * (u[l-2] + u[l+2]) - coeffs[1] * (u[l-1] + u[l+1]) - coeffs[2] * u[l] - coeffs[5] * (uPrev[l-2] + uPrev[l+2]) - coeffs[6] * (uPrev[l-1] + uPrev[l+1]) - coeffs[7] * uPrev[l]; 
 }
}