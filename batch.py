#!/usr/bin/env python
# -*- coding:utf-8 -*-
import redis
import os
import pandas as pd
from pandas import DataFrame
import numpy as np

def decorator( fn):
    def call(self, Txtfilename):
        file = open(Txtfilename,'r')
        values=[]
        while True:
            line=file.readline()
            if not line:
                break
            p_tmp= [str(i) for i in line.split(',')]
            values.append(p_tmp)

        for i in range(len(values)):
            try:
                phoneNumbers = values[i][3]
                time = int(values[i][0]) * 1000000 + int(values[i][1])
                points = values[i][4] + "," + values[i][5] + "," + values[i][6]
                fn(self, phoneNumbers, time, points)
            except ValueError as e:
                print("{}line {}   {}  ",i,values[i][0],values[i][1])
                print('ValueError:', e)

            except IndexError as e:
                print("{}line  total{} ", i,len(values))
                print('ValueError:', e)


        return self

    return call



#解析gps原始数据包
def newdecorator( fn):
    def call(self, Txtfilename):
        # try:
        #
        # except  ValueError:
        #     return self
        values = []
        try:
            file = open(Txtfilename, 'r')
            line = file.readline()
            while True:
                trajectoryPoint = []
                # try:
                # 第一行：  #BESTVELA  速度
                line = file.readline()
                if not line:
                    break
                p_tmp = [str(i) for i in line.split(',')]
                if p_tmp[0] != "#BESTVELA":
                    continue
                trajectoryPoint.append(float(p_tmp[13]) * 3600 / 1000)

                # 第二行：$GPGGA,  经纬度
                line = file.readline()
                if not line:
                    break
                p_tmp = [str(i) for i in line.split(',')]
                if p_tmp[0] != "$GPGGA":
                    continue
                y = float(p_tmp[2])
                x = float(p_tmp[4])
                longitudeD = int(x / 100)
                latitudeD = int(y / 100)
                longitudeD = longitudeD + (x - longitudeD * 100) / 60
                latitudeD = latitudeD + (y - latitudeD * 100) / 60

                trajectoryPoint.append(longitudeD)
                trajectoryPoint.append(latitudeD)

                # 第三行、第四行：$GPGST、$GPVTG
                file.readline()
                file.readline()

                # 第五行：$GPZDA   时间
                line = file.readline()
                if not line:
                    break
                p_tmp = [str(i) for i in line.split(',')]
                if p_tmp[0] != "$GPZDA":
                    continue
                trajectoryPoint.append(float(p_tmp[1]))

                # 第六行：#HEADINGA
                file.readline()

                # 存入数组
                values.append(trajectoryPoint)
        except UnicodeDecodeError:
            print("UnicodeDecodeError")
            # file = open(Txtfilename, 'rb')
            # # file = open(Txtfilename, 'r')
            # while True:
            #     trajectoryPoint = []
            #     # try:
            #     # 第一行：  #BESTVELA  速度
            #     line = file.readline()
            #     if not line:
            #         break
            #     p_tmp = [str(i) for i in line.decode().split(',')]
            #     if p_tmp[0] != "#BESTVELA":
            #         continue
            #     trajectoryPoint.append(float(p_tmp[13]) * 3600 / 1000)
            #
            #     # 第二行：$GPGGA,  经纬度
            #     line = file.readline()
            #     if not line:
            #         break
            #     p_tmp = [str(i) for i in line.decode("gb2312").split(',')]
            #     if p_tmp[0] != "$GPGGA":
            #         continue
            #     y = float(p_tmp[2])
            #     x = float(p_tmp[4])
            #     longitudeD = int(x / 100)
            #     latitudeD = int(y / 100)
            #     longitudeD = longitudeD + (x - longitudeD * 100) / 60
            #     latitudeD = latitudeD + (y - latitudeD * 100) / 60
            #
            #     trajectoryPoint.append(longitudeD)
            #     trajectoryPoint.append(latitudeD)
            #
            #     # 第三行、第四行：$GPGST、$GPVTG
            #     file.readline()
            #     file.readline()
            #
            #     # 第五行：$GPZDA   时间
            #     line = file.readline()
            #     if not line:
            #         break
            #     p_tmp = [str(i) for i in line.decode("gb2312").split(',')]
            #     if p_tmp[0] != "$GPZDA":
            #         continue
            #     trajectoryPoint.append(float(p_tmp[1]))
            #
            #     # 第六行：#HEADINGA
            #     file.readline()
            #
            #     # 存入数组
            #     values.append(trajectoryPoint)

        key = Txtfilename.split("GPS\\")[1].replace("\\", "_").split(".")[0]
        for i in range(len(values)):
            try:
                # phoneNumbers = values[i][3]
                time = int(values[i][3])
                if (values[i][1] > 135 or values[i][1] < 100 or values[i][2] > 35 or values[i][2] < 25):
                    raise ValueError
                points = str(values[i][1]) + "," + str(values[i][2]) + "," + str(values[i][0])
                fn(self, key, time, points)
            except ValueError as e:
                print("{}line {}   {}  ", i, values[i][1], values[i][2])
                print('ValueError:', e)

            except IndexError as e:
                print("{}line  total{} ", i, len(values))
                print('ValueError:', e)

        return self


    return call

def all_files(file_dir):
    L=[]
    for root, dirs, files in os.walk(file_dir):
        for file in files:
            if os.path.splitext(file)[1] == '.gps':
                L.append(os.path.join(root, file))
    return L

class TrajectoryInputer :
    # 连接池方式
    def __init__(self):
        self.pool = redis.ConnectionPool(host='127.0.0.1', port=6379)
        self.r = redis.Redis(connection_pool=self.pool)


    @decorator
    def input(self,phoneNumbers,time,points):
        self.r.zadd(phoneNumbers,{points:time} )

    @newdecorator
    def inputnewdata(self, phoneNumbers, time, points):  #yhy的数据
        self.r.zadd(phoneNumbers, {points: time})


    def readgps(self, Txtfilename):
        file = open(Txtfilename,'r')
        values=[]
        while True:
            trajectoryPoint = []

            #第一行：  #BESTVELA  速度
            line=file.readline()
            if not line:
                break
            p_tmp= [str(i) for i in line.split(',')]
            if p_tmp[0]!="#BESTVELA" :
                continue
            trajectoryPoint.append(float(p_tmp[13])*3600/1000)

            #第二行：$GPGGA,  经纬度
            line = file.readline()
            if not line:
                break
            p_tmp = [str(i) for i in line.split(',')]
            if p_tmp[0]!="$GPGGA" :
                continue
            y=float(p_tmp[2])
            x=float(p_tmp[4])
            longitudeD=int(x/100)
            latitudeD=int(y/100)
            longitudeD=longitudeD+(x-longitudeD*100)/60
            latitudeD = latitudeD + (y - latitudeD * 100) / 60

            trajectoryPoint.append( longitudeD)
            trajectoryPoint.append( latitudeD)

            #第三行、第四行：$GPGST、$GPVTG
            file.readline()
            file.readline()

            #第五行：$GPZDA   时间
            line = file.readline()
            if not line:
                break
            p_tmp = [str(i) for i in line.split(',')]
            if p_tmp[0]!="$GPZDA" :
                continue
            trajectoryPoint.append(float(p_tmp[1]))

            # 第六行：#HEADINGA
            file.readline()

            #存入数组
            values.append(trajectoryPoint)

        trajectoryPoints=[]
        for i in range(len(values)):
            try:
                # phoneNumbers = values[i][3]
                time = int(values[i][3])
                if(values[i][1]>135 or values[i][1]<100 or values[i][2]>35 or values[i][2]<25 ):
                    raise ValueError
                points = str(values[i][1]) + "," + str(values[i][2]) + "," + str(values[i][0])
                trajectoryPoints.append(values[i])
            except ValueError as e:
                print("{}line {}   {}  ",i,values[i][1],values[i][2])
                print('ValueError:', e)

            except IndexError as e:
                print("{}line  total{} ", i,len(values))
                print('ValueError:', e)

        return trajectoryPoints

    def write2txt(self, GPSfilename,txtfilepath):
        trajectoryPoints=self.readgps(GPSfilename)
        npPoints=np.array(trajectoryPoints)
        data = DataFrame(npPoints, columns=['velocity', 'longitude', 'latitude', 'time'])
        data.to_csv(txtfilepath)

if __name__ =="__main__":
    inputer=TrajectoryInputer()
    # for i in range(2,23):
    #     filepath="C:\\学习\\13数据\\3浮动车数据\\"+str(i)+".txt"
    #     inputer.input(filepath)

    paths=all_files("C:\\学习\\13数据\\GPS-yhy\\GPS\\")
    filepath = "C:\\学习\\13数据\\GPS-yhy\\GPS\\gps1.gps"
    for path in paths:
        inputer.inputnewdata(path)

    # filepath = "C:\\学习\\0研究\\轨迹简化\\0951.gps"
    # inputer.write2txt(filepath,"C:\\学习\\0研究\\轨迹简化\\0951table.txt")