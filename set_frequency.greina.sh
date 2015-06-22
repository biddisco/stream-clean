freqmin=$1
freqmax=$2
for cpu in 0 1 2 3
do
    sudo cpufreq-set --cpu $cpu --min $freqmin --max $freqmax
done
