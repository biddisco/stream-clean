freqmin=$1
freqmax=$2
#for cpu in 0 1 2 3
for cpu in 0 1 2 3 4 5 6 7 8 9 10 11
do
    sudo cpufreq-set --cpu $cpu --min $freqmin --max $freqmax
done
