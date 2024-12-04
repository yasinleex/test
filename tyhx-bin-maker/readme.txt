tyhx-bin-maker功能：
1.将寄存器配置列表对应的文本文件转换成二进制的firmware文件
2.将二进制firmware文件还原成对应的寄存器配置列表(文本文件)
参数：
-t 后跟文本文件名，用于将文本文件转换为firmware文件
-b 后跟firmware文件名，用于将firmware文件转换为文本文件
-o 后跟生成的文件名
-v 在将配置文件转换为firmware文件时可用此参数插入一个8位的版本号字段fw_ver(0~255),这是可选参数，如果没有此参数则生成的firmware中的fw_ver字段为0
-c 在将配置文件转换为firmware文件时可用此参数插入一个芯片标识字段chip_type,用来区分hx9023s和hx9031as,规定该值为0时表示hx9023s，为1时表示hx9031as，这是可选参数，如果没有此参数则生成的firmware中的chip_type字段为0，表示hx9023s；该字段用于将驱动切换为适配hx9031as，为1时表示驱动走hx9031as的逻辑，为0时表示驱动走hx9023s的逻辑
示例：
1.生成my-cfg.txt对应的firmware文件并保存为my-firmware.bin
tyhx-bin-maker -t my-cfg.txt -o my-firmware.bin

2.生成my-cfg.txt对应的firmware文件并保存为my-firmware.bin，并在该bin文件中插入版本号0x55
tyhx-bin-maker -t my-cfg.txt -o my-firmware.bin -v 0x55

3.生成my-cfg.txt对应的firmware文件并保存为my-firmware.bin，并在该bin文件中插入版本号0x55，并配置chip_type字段设为1(表示将驱动切换为适配hx9031as)
tyhx-bin-maker -t my-cfg.txt -o my-firmware.bin -v 0x55 -c 0x01

4.解析出my-firmware.bin中的参数配置信息并将该配置内容保存到文本文件my-cfg.txt中供查阅
tyhx-bin-maker -b my-firmware.bin -o my-cfg.txt

注意：
配置文件有严格的格式要求！
配置文件内容示例：
0x01,0x22
0x02,0x33
0x03,0xcC
0x05,0x59

要求：
1.每行有且只有两个用0x开头的十六进制数，这两个数用逗号隔开，逗号是半角字符！也就是英文的逗号！！！确切说配置文件的内容全部都是半角字符！！！
2.不要在任何位置插入任何其他无关字符！
3.十六进制数中的字母不区分大小写，但是0x的x必须小写！
4.每行的两个数字中第一个表示寄存器地址，第二个表示寄存器值，不要搞反了！
5.寄存器不要重复配置，除非你确定要这么做！该程序不会检查是否有重复配置项！
6.寄存器地址和值的范围都是0x00~0xFF，可配置的寄存器最多256个，也就是配置文件最多有256行！
7.查看程序执行输出的log，没有任何报错的情况下生成的文件才是可信的！

