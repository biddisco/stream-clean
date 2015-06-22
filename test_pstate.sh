for freq in `cat $1`
do
    printf "%-10d" $freq
    for threads in 1 2 3 4 6 8 10 12
    do
        OMP_NUM_THREADS=$threads aprun -cc numa_node --p-state $freq ./stream.gcc 22 | grep Triad: | awk '{printf("%6.1f", $2)}'
    done
    printf "\n"
done

