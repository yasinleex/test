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
def pingfanghe(*num): #这是可变参数,本质上，在函数内部，这个可变参数被视作一个tuple。
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
def key_word(name, age, **kw):
    print("Name:", name, "Age:", age, "Other:", kw)
    return


key_word("ZhangSan", 26)
key_word("LiSi", 28, Sex='M')


#===================================================
# 小结
# Python的函数具有非常灵活的参数形态，既可以实现简单的调用，又可以传入非常复杂的参数。
# 默认参数一定要用不可变对象，如果是可变对象，程序运行时会有逻辑错误！
# 要注意定义可变参数和关键字参数的语法：
# *args是可变参数，args接收的是一个tuple；
# **kw是关键字参数，kw接收的是一个dict。
# 以及调用函数时如何传入可变参数和关键字参数的语法：
# 可变参数既可以直接传入：func(1, 2, 3)，又可以先组装list或tuple，再通过*args传入：func(*(1, 2, 3))；
# 关键字参数既可以直接传入：func(a=1, b=2)，又可以先组装dict，再通过**kw传入：func(**{'a': 1, 'b': 2})。
# 使用*args和**kw是Python的习惯写法，当然也可以用其他参数名，但最好使用习惯用法。
# 命名的关键字参数是为了限制调用者可以传入的参数名，同时可以提供默认值。
# 定义命名的关键字参数在没有可变参数的情况下不要忘了写分隔符*，否则定义的将是位置参数。
#
#===================================================


def my_print(pattern, count):
    while count>0:
        print(pattern, end='') # 如何在输出后不自动换行！！！！
        count=count-1


def func(n):
    fn=0
    if n==1:
        my_print('>', n)
        print(n)
        fn=1

        my_print('<', n)
        print(" %d!=%d" %(n, fn))
        return fn
    else:
        my_print('>', n)
        print(n)
        fn=func(n-1)*n

        my_print('<', n)
        print(" %d!=%d" %(n, fn))
        return fn


print(func(1))
print(func(20))





#===================================================





