Abacus - Simple mathematical expression parser with calculator interface and arbitrary precision
================================================================================================

License
-------

[3-Clause BSD License](https://github.com/LukasBanana/Abacus/blob/master/LICENSE.txt)

Status
------

**Beta**

Dependencies
------------

Arbitrary Precision Package
see http://www.hvks.com/Numerical/arbitrary_precision.html

Example
-------

```cpp
#include <Abacus/Abacus.h>
#include <iostream>

class Log : public Ac::Log
{
public:
    void Error(const std::string& s) override
    {
        std::cerr << s << std::endl;
    }
};

int main()
{
    Log log;
    Ac::ConstantsSet constantsSet;
    
    Ac::SetFloatPrecision(50);
    
    std::cout << Ac::Compute("x = 12", constantsSet, &log) << std::endl;
    std::cout << Ac::Compute("3 + 2*(pi^-e - sqrt(log2(x*5)))", constantsSet, &log) << std::endl;

    return 0;
}
```


