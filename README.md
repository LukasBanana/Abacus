Abacus - A simple mathematical expression parser with calculator interface
==========================================================================

License
-------

[3-Clause BSD License](https://github.com/LukasBanana/Abacus/blob/master/LICENSE.txt)

Status
------

**Alpha**

Dependencies
------------

Arbitrary Precision Package
see http://www.hvks.com/Numerical/arbitrary_precision.html

Example
-------

```cpp
#include <Abacus/Abacus.h>
#include <iostream>

int main()
{
    Ac::ComputeState state;
    
    std::cout << Ac::Compute("x = 12", &state) << std::endl;
    std::cout << Ac::Compute("3 + 2*(pi^-e - sqrt(log2(x*5)))", &state) << std::endl;

    return 0;
}
```


