#! /usr/bin/env python3
mydict={'A':65, 'B':66, 'C':67}
print(mydict)

mydict['D']=68 #添加一个新元素，如果指定的key已经存在，则只是更新对应元素的value
print(mydict)

mydict.update({'E':69, 'F':70, 'A':99}) #更新：把一组新的元素插入到字典中，如果有key相同元素，则该元素的value被更新！
print(mydict)

if 'A' in mydict: #检查字典中的某个key是否存在
    print("key 'A' found")
else:
    print("key 'A' not found")

key_check='X' in mydict
print(key_check)

if 'X' in mydict:
    print("key 'X' found")
else:
    print("key 'X' not found")

test=mydict.get('F') #检查字典中某个key是否存在，若不存在，返回None；若存在，返回该key对应的value
print("key F: "+str(test))

if None!=test:
    print('key 'F' found, it\'s value is:' + str(test))
    mydict.pop('F') # delate the item
print(mydict)

test=mydict.get('X') #检查字典中某个key是否存在，若不存在，返回None；若存在，返回该key对应的value
print("key X: "+str(test))

print(mydict)
if not 888==mydict.get('F', 888): #get函数中如果提供了第二个参数，表示当找不到对应key时该函数的返回值，也就是用来替换None的
    print("found the key!")
    del mydict['F'] #第二种删除一个元素的方法
else:
    print("not found the key 'F'")
print(mydict)

print(mydict)
if not 999==mydict.get('A', 999):
    print("found the key 'A'")
    del mydict['A']
else:
    print("not found the key 'A'")
print(mydict)

print("===================================================================")

mylist=[1,2,"A",'B',3.5,9]
my_set=set(mylist) #set的初始化必须用一个list，set的初始化像函数一样，用了一个关键字set
print(my_set) #打印出来你会看到set和dict一样，都是无序的！因为二者都是用key索引，set就像是没有值的dict

my_set.add('X')
print(my_set)

my_set.add('X') #添加一个已有的key是无效的，这一点和dict是相同的，当然dict中会用新值替换该key的旧值
print(my_set) 

my_set.remove('X') #delate a item from a set
print(my_set) 

set1=set([1,2,3,4,5,6,7,8,9])
set2=set([4,78,'w',"eee",5,6,9])
print('set1:' + str(set1) + '\n' + "set2:" + str(set2))

set3=set1 & set2 #获取两个set的交集
print("set3:%s" %(set3))

set4=set1 | set2 #获取两个set的并集
print("set4:%s" %(set4))

set5=set([1,2,3,4,5,6,7,8,9,(88,99)]) # a tuple can be a item of a set
# set6=set([1,2,3,4,5,6,7,8,9,[88,99]]) # a list can NOT be a item of a set

print(set5)
set5.add((88,99)) #添加了一个已经存在的tuple，这没有什么效果，因为python对于常量是hash的，这就意味着常量相同的常量拥有唯一的hash，而tuple就是常量，所以相同的tuple是被视为同一个值.这就是为什么tuple可以作为set中的key，而list不能作为set中的key的原因
print(set5)
set5.add((99,88))
print(set5)










