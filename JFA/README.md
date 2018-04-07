# jfa-cvt
Jump Flood Algorithms for Centroidal Voronoi Tessellation

> Author: Bo Zhou, J CUBE Inc. Tokyo, Japan
>         bo@j-cube.jp


## Brief

This is a simple implementation of Jump Flood Algorithm (`JFA`) for Centralized Voronoi Tessellation (`CVT`), both CPU and GPU version available.

You could consider this as the basic playground to implement the more advanced features such as support for arbitrary shape in 2D space, or by radix-sort to restore the analytic shape of each Voronoi region, etc.

Another interesting application of `JFA` is the problem of 2D/3D level set reinitialization.

Here a couple of screenshot varying the n regions:

```n = 16```

  ![n16](https://github.com/j-cube/jfa-cvt/raw/master/img/16.png)

```n = 64```

  ![n64](https://github.com/j-cube/jfa-cvt/raw/master/img/64.png)
