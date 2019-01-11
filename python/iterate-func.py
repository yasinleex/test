#! /usr/bin/env python3

#=================================================================
def print_list(L):
    for item in L:      #切记python中此处的item不是下标！！！
        print("item=", item)
    return

list_a=[1,3,5,7,9]
print_list(list_a)
#=================================================================
def fill_list(L):
    L=[x*x for x in range(6,10)]
    print(L)
    return

list_b=[2,3]
fill_list(list_b)
print(list_b)
#=================================================================
import os
print([d for d in os.listdir('../')])
#=================================================================

list_c=[a for a in range(1, 20) if a%2==0]
print(list_c)

list_d=[a+b for a in "ABCDE" for b in "abcde"]  #这是一个循环的嵌套！！！
print(list_d)

dict_a={"A":'10', "B":'11',"C":'12'}
print(dict_a)
dict_2_list=[k+"="+v for k,v in dict_a.items()]
print(dict_2_list)



#=================================================================
def findMinAndMax(L):
    if L==[]:
    #Wrong!!!   if L==None:
        return (None, None)
    max=L[0]
    min=L[0]

    for item in L:
        if item > max:
            max=item
        if item < min:
            min=item
    return (min, max)

# 测试
if findMinAndMax([]) != (None, None):
    print('1测试失败!')
elif findMinAndMax([7]) != (7, 7):
    print('2测试失败!')
elif findMinAndMax([7, 1]) != (1, 7):
    print('3测试失败!')
elif findMinAndMax([7, 1, 3, 9, 5]) != (1, 9):
    print('4测试失败!')
else:
    print('5测试成功!')
#=================================================================
