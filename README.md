# stream-clean
The stream benchmark is a mess, this is an attempt to make it a little less messy

The stream benchmark uses compile time loop limits to achieve peak performance for some compilers.
For example, the Intel compiler uses streaming stores when it knows that loop limits are large at compile time.
Streaming stores can give up to a 50% increase in bandwidth, however they are very rarely used in real life codes (and will lead to very poor performance of cache-aware implementations).

This benchmark makes streaming stores optional with the Intel compiler using directives (and uses run time array lengths).
I have also taken the liberty of making the benchmark a bit neater.

# cache bench
A simple implementation of the cachebench benchmark, which takes a couple of seconds instead of a couple of minutes to run.

# stream_papi
The stream-clean benchmark with papi counters via [papi-wrap](https://github.com/bcumming/papi-wrap).
