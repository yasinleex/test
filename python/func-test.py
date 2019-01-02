#!/usr/bin/env python3

#===================================================
def power(x, n=2): #参数有默认值
    s=1;
    while n>0:
        s=s*x
        n=n-1
    return s

a=power(3,4)
b=power(3) # 此处参数2为空，故适用默认值2作为参数2的值
print("a=%d, b=%d" %(a, b))
#===================================================
def test_list(the_list=[]): # 参数为list，传入的list可以在函数内部被改变，这更像传入了数组的指针，即会影响到外部。
    the_list.append("END")
    return the_list

my_list=test_list([1,2,3])
print(my_list)
my_list=test_list([1,2,3])
print(my_list)

list1=[4,5,6]

list2=test_list(list1)
print(list1)
print(list2)

list2=test_list(list1)
print(list1)
print(list2)
#===================================================
def pingfanghe(*num): #这是可变参数
    sum=0
    for n in num:
        sum=sum+(n*n)
    return sum

sum=pingfanghe(1,2,3) #可以传入三个参数
print("sum=%d" %sum)

sum=pingfanghe(1,2,3,4) #可以传入四个参数
print("sum=%d" %sum)

sum=pingfanghe() #可以传入0个参数
print("sum=%d" %sum)

list3=[2,3,4]
sum=pingfanghe(*list3) #可以传入一个list，但是要在list前面加上*，这表示把该list中的元素当作参数传进去
print("sum=%d" %sum)

tuple1=(2,3,4,5)
sum=pingfanghe(*tuple1) #传入一个tuple和list是同样的
print("sum=%d" %sum)

sum=pingfanghe(*[1,2,3]) #直接传入list常量作为参数，也是要加个*号的
print("sum=%d" %sum)

#===================================================



#===================================================
#===================================================
#===================================================





