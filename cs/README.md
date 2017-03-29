# CacheSpeed

CacheSpeed is a simple tool to measure certain parameters required for cache attacks. 

## Building CacheSpeed

CacheSpeed has no dependencies on any shared library or special system calls. It should be possible to compile it on any POSIX-compliant operating system. 

To build CacheSpeed, simply execute

    make
    
## Running CacheSpeed

To run CacheSpeed, simply execute

    ./cachespeed

The measurement might take several seconds and yields an output similar to this. 
```
                                (                            
   (                  )         )\ )                   (    
   )\      )       ( /(    (   (()/(          (    (   )\ ) 
 (((_)  ( /(   (   )\())  ))\   /(_))`  )    ))\  ))\ (()/( 
 )\___  )(_))  )\ ((_)\  /((_) (_))  /(/(   /((_)/((_) ((_))
((/ __|((_)_  ((_)| |(_)(_))   / __|((_)_\ (_)) (_))   _| | 
 | (__ / _` |/ _| | ' \ / -_)  \__ \| '_ \)/ -_)/ -_)/ _` | 
  \___|\__,_|\__| |_||_|\___|  |___/| .__/ \___|\___|\__,_| 
                                    |_|                     
[ * ] Measuring lfence overhead...
[ + ] Done.
[ * ] Measuring mfence overhead...
[ + ] Done.
[ * ] Measuring cpuid overhead...
[ + ] Done.
[ * ] Measuring L1 hit time...
[ + ] Done.
[ * ] Measuring L1 miss time...
[ + ] Done.
[ * ] Measuring L3 miss time...
[ + ] Done.


         | cycles    | + lfence  | + mfence  | + cpuid
---------+-----------+-----------+-----------+------------
 L1 Hit  |         8 |        64 |        96 |       102 
 L1 Miss |        16 |        72 |       104 |       110 
 L3 Miss |       180 |       236 |       268 |       274 
---------+-----------+-----------+-----------+------------
```

CacheSpeed measures how long it takes to access data which is stored in the L1 cache (L1 Hit), L2 cache (L1 MIss), and DRAM (L3 Miss). 
The first column (cycles) shows the access times. Columns "+ lfence", "+ mfence", and "+ cpuid" are the access times including the respective fence mechanisms. 

### Example: CJAG

CJAG exploits the timing differences in the last-level cache. 
The used fences are `mfence`. 
Thus, the value in line "L3 Miss" and column "+ mfence" is the correct threshold to use. 