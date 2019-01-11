#!/usr/bin/env python3
from collections import Iterable

d={"aaa":1,"bbb":2,"ccc":3,"ddd":4}

if isinstance(d, Iterable):     # check if the Istance is Iterable
    print("d is Iterable")
else:
    print("d is Not Iterable")

for value in d.values():
    print("print the value of a dict: ", value)

for key in d.keys():
    print("print the key of a dict: ", key)

for k_and_v in d.items():
    print("print both of the key and the value of a dict: ", k_and_v)

list_a=[(1,2),(4,5),(7,8),(10,11),(13,14)]
print(list_a)
for x,y in list_a:
    print("x=%d, y=%d" %(x,y))

print("#######################################################################")

list_b=list(range(1,10))   # 简单列表生成式
print('list_b=', list_b)

list_c=list(range(3,10))
print('list_c=', list_c)

list_d=[x*x+1 for x in range(1, 8)]  # 复杂列表生成式
print('list_d=', list_d)

list_e=[x*x for x in range(1, 8) if x%2==0]  # 复杂列表生成式
print('list_e=', list_e)

a="asdfeiruytlkj"
list_f=[x for x in a]
print('list_f=', list_f)

list_g=[str(x)+'='+str(y) for x,y in d.items()]
print('list_g=', list_g)

list_h=[s.upper() for s in list_g]
print('list_h=', list_h)

list_i=[s.lower() for s in list_h]
print('list_i=', list_i)







