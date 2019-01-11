#! /usr/bin/env python3

list_a=[1,2,3,4,5]
list_b=list_a[1:3]
print(list_a)
print(list_b)
list_c=list_a[-3:-1]
print(list_c)
print("=============")



list_a=[x for x in range(1, 10)]
print(list_a)


generator_a=(x for x in range(1, 10))
for x in generator_a:
    print(x)

def yield_test(max):
    x=1
    while max:
        max=max-1
        x=x+3
        print("x=", x)
        yield (x)
    return



for i in yield_test(3):
    print(i)
    
    
