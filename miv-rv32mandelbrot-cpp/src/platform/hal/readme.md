# hal folder


The HAL folder hardware abstraction for the bare metal drivers for the fabric 
IP cores. The HAL folder contains files using a combination of C and assembly 
source code. This layer allows the fabric ip drivers to be used with any of the
soft processors or the MSS hardened processors.

The hal folder should be included in a your project under the platform directory. 
See location in the drawing below.

The hal folder contains:

* register access functions
* assert macros

### Project directory strucutre, showing where hal folder sits.

   +---------+      +-----------+
   | src     +----->|application|
   +---------+  |   +-----------+
                |
                |   +-----------+
                +-->|middleware |
                |   +-----------+
                |
                |   +-----------+     +---------+
                +-->|platform   +---->|drivers  |
                    +-----------+  |  +---------+
                                   |
                                   |  +---------+
                                   +->|hal  	|
                                   |  +---------+
                                   |
                                   |  +---------+
                                   +->|mpfs_hal |
                                      +---------+
                         