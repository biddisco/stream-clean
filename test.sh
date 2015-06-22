for threads in 1 2 3 4
do
    OMP_NUM_THREADS=$threads numactl --cpunodebind=0 ./stream.intel 10 | grep :
    echo
done
