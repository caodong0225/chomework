#!/usr/bin/env python
# coding: utf-8

# In[3]:


import requests
file=open("title.txt","w")
try:
    a=input("请输入三个中文关键词(用英文逗号分隔):")
    b=input("请输入长度:")
    data = requests.get("http://localhost:8999/data?input="+a+","+b)
    file.write(data.text)
except:
    print("参数错误，请返回重试！")
file.close()

