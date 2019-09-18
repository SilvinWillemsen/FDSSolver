#include <stdio.h>
void updateEq(double* uNext, double* u, double* uPrev, double* coeffs)
{
for (int l = 2; l < 106; ++l) { uNext[l] = - coeffs[0] * (u[l-1] + u[l+1]) - coeffs[1] * u[l] - coeffs[4] * uPrev[l]; }
}