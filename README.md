# stream-clean
The stream benchmark is a mess, this is an attempt to make it a little less messy

The stream benchmark uses global static arrays and compile time loop limits to achieve peak performance for some compilers.
For example, the Intel compiler uses streaming stores with large compile time loop limits.
Streaming stores can give up to a 50% increase in bandwidth, however they are very rarely used in real life codes (and will lead to very poor performance of cache-aware implementations).

This benchmark makes streaming stores optional with the Intel compiler using directives (and uses run time array lengths).
I have also taken the liberty of making the benchmark a bit neater.
