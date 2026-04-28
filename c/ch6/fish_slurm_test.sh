#!/bin/bash
# Apptainer example submit script for Slurm.
#
# Replace <ACCOUNT> with your account name before submitting.
#
#SBATCH -A apam           # Set Account name
#SBATCH --job-name=fish  # The job name
#SBATCH -t 0-0:30              # Runtime in D-HH:MM
#SBATCH --mem-per-cpu=5gb      # Memory per cpu core
#SBATCH --ntasks=64
#SBATCH --ntasks-per-node=16
#SBATCH --time=120
#SBATCH --mail-user=mws6@columbia.edu
#SBATCH --mail-type=BEGIN
#SBATCH --mail-type=END
#SBATCH --mail-type=FAIL
#SBATCH --output=fish_test_16.o.%j

# load singularity module and copy paths for apptainer (ginsburg)
module load singularity 
export APPTAINER_TMPDIR=$SINGULARITY_TMPDIR
export APPTAINER_BINDPATH=$SINGULARITY_BINDPATH
module load openmpi/gcc/64/4.1.7a1

# set the container for firedrake
SIF=/burg/home/mws6/sifs/firedrake-ts.sif

# run the 3-Dpoisson pure petsc code fish.c

cd /burg/home/mws6/repos/github/p4pdes/c/ch6
echo $SLURM_NTASKS
mpiexec -np $SLURM_NTASKS apptainer exec  $SIF ./fish -da_refine 7 -fsh_dim 3 -ksp_monitor -ksp_type cg -pc_type gamg -log_view 



