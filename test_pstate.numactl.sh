for freq in `cat $1`
do
    printf "%-10d" $freq
    ./set_frequency.greina.sh 1200000 $freq
    #for threads in 1 2 3 4 6 8 10 12
    for threads in 1 4 12
    do
        OMP_NUM_THREADS=$threads numactl --cpunodebind=0 ./stream.intel 23 | grep Triad: | awk '{printf("%6.1f", $2)}'
    done
    printf "\n"
done

