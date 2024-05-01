#### Benchmark for a [triangular mesh](data/bunny.ply) consisting of `69451` triangles
|   Method  |Samples|            Range                 |        Mean      |     Std Dev    |
|-----------|:-----:|:--------------------------------:|:----------------:|:--------------:|
|`Iterative`|  100  |**2.3333** _ms_ - **2.34581** _ms_| **2.33979** _ms_ |**31.7994** _µs_|
|`K-d Tree` |  100  |**1.12247** _µs_ - **1.8992** _µs_| **1.48112** _µs_ |**1.96807** _µs_|
