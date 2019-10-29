import requests
import re
#正则表达式库
#def本身是一个函数对象。也可以叫它“方法”。属于对象的函数，就是对象的属性。def定义了一个模块的变量，或者说是类的变量。
def getHTMLText(url):#get网页信息并作为文本输出函数
    try:
        r = requests.get(url, timeout=30)
        r.raise_for_status()
        r.encoding = r.apparent_encoding
        return r.text
    except:
        return "ERROR"
def parsePage(ilt, html):#分析页面函数
    try:
        plt = re.findall(r'\"view_price\"\:\"[\d\.]*\"',html)#寻找与括号中匹配的内容
        tlt = re.findall(r'\"raw_title\"\:\".*?\"',html)
        for i in range(len(plt)):
            price = eval(plt[i].split(':')[1])#将字符串str当成有效的表达式来求值并返回计算结果。
            title = eval(tlt[i].split(':')[1])
            ilt.append([price , title])#用于在列表末尾添加新的对象。
    except:
        print("ERROR")
def printGoodsList(ilt):#打印函数 
    tplt = "{:4}\t{:8}\t{:16}"#按照此格式输出
    print(tplt.format("序号", "价格", "商品名称"))#格式化函数
    count = 0
    for g in ilt:#循环
        count = count + 1
        print(tplt.format(count, g[0], g[1]))

def main():
    goods = '电脑'
    depth = 3
    start_url = 'https://s.taobao.com/search?q=' + goods
    infoList = []
    for i in range(depth):
        try:
            url = start_url + '&s=' + str(44*i)
            html = getHTMLText(url)
            parsePage(infoList, html)
        except:
            continue
    printGoodsList(infoList)

main()#调用main(),之后调用getHTMLText函数，之后调用parsePage函数，最后调用printGoodsList函数
input()#防止程序闪退