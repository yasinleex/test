#!/usr/bin/env python3
print("hello, world\n","second string")
a = ord('A')
print("a=",a)
print("b=",ord('B'))
b = chr(69)
print('b=',b)
c = "ABC中文"
print(c.encode('utf-8'))
#print(c.encode('ascii'))
list_a=[1,]
for x in list_a:
    if x<=10:
        print('x=%d' %(x))
        list_a.append(x+1)
    else:
        print("reach the max value:%d" %(x))
print("exit")

sum=0
for x in range(6):
    sum=x+sum
print("sum=%d" %(sum))


d={123:11,456:22,'aa':'bb'}
print(d)
print(d['aa'])
print(d[456])
d[123]="uuuuuuu"
print(d[123])

a=(1,2,3)
d={a}
print(d)

def my_abs(x):
    if not isinstance(x, (int, float)):
        raise TypeError("the parameter must be int or float")
    if x>=0:
        return x
    else:
        return -x


a=-8
print(my_abs(a))
a=9
print(my_abs(a))
print(my_abs('c'))




