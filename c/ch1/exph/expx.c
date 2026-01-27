#include <petsc.h>

// accumulate the local contribution to exp(x) using nested iteration for n in [n_start, n_end]
static double horner_exp_terms(double x, int n_start, int n_end)
{
    if (n_end < n_start)
        return 0.0;

    // Start with the highest term: x^N/N!
    double result = 1.0;
    for (int i = n_end; i > n_start; i--)
    {
        result = 1.0 + result * x / i;
    }

    // Multiply by x^k/k! at the end for k = n_start
    for (int i = 0; i < n_start; i++)
    {
        result *= x / (i + 1);
    }

    return result;
}

int main(int argc, char **argv)
{
    PetscMPIInt rank;
    PetscInt    N = 100 /* total number of terms to use */;
    PetscInt    P /* number of processes */;
    PetscInt    n_start, n_end;
    PetscReal   x = 1.0, localval, globalsum, rel_error;
    PetscBool   is_negative = PETSC_FALSE;

    PetscCall(PetscInitialize(&argc, &argv, NULL,
                              "Compute exp(x) in parallel with PETSc.\n\n"));
    PetscCall(MPI_Comm_rank(PETSC_COMM_WORLD, &rank));
    PetscCall(MPI_Comm_size(PETSC_COMM_WORLD, &P));

    // read option
    PetscOptionsBegin(PETSC_COMM_WORLD, "", "options for expx", "");
    PetscCall(PetscOptionsReal("-x", "input to exp(x) function", NULL, x, &x, NULL));
    PetscCall(PetscOptionsInt("-n", "number of terms to use", NULL, N, &N, NULL));
    PetscOptionsEnd();

    // check for negative x
    if (x < 0.0)
    {
        is_negative = PETSC_TRUE;
    }

    // determine the range of terms for this process
    n_start = rank * (N / P);
    if (rank == P - 1)
    {
        n_end = N;
    }
    else
    {
        n_end = (rank + 1) * (N / P);
    }
    // compute local contribution
    localval = horner_exp_terms(PetscAbsReal(x), n_start, n_end - 1);

    // sum the contributions over all processes
    PetscCall(MPI_Allreduce(&localval, &globalsum, 1, MPIU_REAL, MPIU_SUM,
                            PETSC_COMM_WORLD));

    if (is_negative)
    {
        globalsum = 1.0 / globalsum;
    }

    rel_error = PetscAbsReal(globalsum - PetscExpReal(x)) / 
                PetscAbsReal(PetscExpReal(x)) / PETSC_MACHINE_EPSILON;
    // output estimate and report on work from each process
    PetscCall(PetscPrintf(PETSC_COMM_WORLD,
                          "exp(%.16g) is about %.16g with relative error %.16g eps \n", x, globalsum, rel_error));

    PetscCall(PetscFinalize());
    return 0;
}

