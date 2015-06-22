for freq in `cat $1`
do
    printf "%-10d" $freq
    ./set.sh 1200000 $freq
    #for threads in 1 2 3 4 6 8 10 12
    for threads in 1 2 3 4
    do
        OMP_NUM_THREADS=$threads numactl --cpunodebind=0 ./stream.intel 22 | grep Triad: | awk '{printf("%6.1f", $2)}'
    done
    printf "\n"
done

