'''
增加音效
'''
import sys
import peiqi

import plane
import tkinter as tk
import time
import serial
import keyboard
from tkinter import messagebox
from PIL import Image, ImageTk

import pygame
import threading
import queue
import serial
import serial.tools.list_ports
# 导入模块
import pygame, time, random
from pygame.sprite import Sprite

import pygame
from time import sleep
import random
from pygame.sprite import collide_rect

# 设置通用属性
BG_COLOR = pygame.Color(0, 0, 0)
SCREEN_WIDTH = 700
SCREEN_HEIGHT = 500
TEXT_COLOR = pygame.Color(255, 0, 0)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)


class Tank:
    '''
    坦克类
    '''

    def __init__(self) -> None:
        self.live = True
        # 记录坦克原来的位置
        self.old_left = 0
        self.old_top = 0
        self.hp = 0

    def display_tank(self) -> None:
        '''
        显示坦克
        '''
        # 获取最新坦克的朝向位置图片
        self.image = self.images.get(self.direction)
        MainGame.window.blit(self.image, self.rect)

    def move(self) -> None:
        '''
        坦克的移动
        '''
        # 记录坦克原来的位置,为了方便还原碰撞后的位置
        self.old_left = self.rect.left
        self.old_top = self.rect.top
        if self.direction == "L":
            # 判断坦克的位置是否已左边界
            if self.rect.left > 0:
                # 修改坦克的位置 离左边的距离  - 操作
                self.rect.left = self.rect.left - self.speed
        elif self.direction == "R":
            # 判断坦克的位置是否已右边界
            if self.rect.left + self.rect.width < SCREEN_WIDTH:
                # 修改坦克的位置 离左边的距离  + 操作
                self.rect.left = self.rect.left + self.speed
        elif self.direction == "U":
            # 判断坦克的位置是否已上边界
            if self.rect.top > 0:
                # 修改坦克的位置 离上边的距离  - 操作
                self.rect.top = self.rect.top - self.speed
        elif self.direction == "D":
            # 判断坦克的位置是否已下边界
            if self.rect.top + self.rect.height < SCREEN_HEIGHT:
                # 修改坦克的位置 离上边的距离  + 操作
                self.rect.top = self.rect.top + self.speed

    def shot(self) -> None:
        '''
        坦克的射击
        '''
        pass

    def tank_hit_wall(self) -> None:
        '''
        坦克和墙壁的碰撞
        '''
        for wall in MainGame.wall_list:
            # 检测当前坦克是否能和墙壁发生碰撞
            if pygame.sprite.collide_rect(self, wall):
                # 将位置还原到碰撞前的位置
                self.rect.left = self.old_left
                self.rect.top = self.old_top

    def tank_collide_tank(self, tank):
        '''
            检测2个坦克是否碰撞
        '''
        # 判断是否都存活
        if self and tank and self.live and tank.live:
            if pygame.sprite.collide_rect(self, tank):
                # 将位置还原到碰撞前的位置
                self.rect.left = self.old_left
                self.rect.top = self.old_top


class MyTank(Tank):
    '''
    我方坦克类
    '''

    def __init__(self, left: int, top: int) -> None:
        super(MyTank, self).__init__()
        self.hp = 4
        # 设置我方坦克的图片资源
        self.images = {
            'U': pygame.image.load('./img/p1tankU.gif'),
            'D': pygame.image.load('./img/p1tankD.gif'),
            'L': pygame.image.load('./img/p1tankL.gif'),
            'R': pygame.image.load('./img/p1tankR.gif'),
        }
        # 设置我方坦克的方向
        self.direction = 'L'
        # 获取图片信息
        self.image = self.images.get(self.direction)
        # 获取图片的矩形
        self.rect = self.image.get_rect()
        # 设置我方坦克位置
        self.rect.left = left
        self.rect.top = top
        # 设置移动速度
        self.speed = 2
        # 设置移动开关, False 表示不移动, True 表示移动
        self.remove = False


class EnemyTank(Tank):
    '''
    敌方坦克类
    '''

    def __init__(self, left, top, speed, hp) -> None:
        super(EnemyTank, self).__init__()
        self.hp = 2
        self.images = {
            'U': pygame.image.load('./img/enemy1U.gif'),
            'D': pygame.image.load('./img/enemy1D.gif'),
            'L': pygame.image.load('./img/enemy1L.gif'),
            'R': pygame.image.load('./img/enemy1R.gif'),
        }
        # 设置敌方坦克的方向
        self.direction = self.rand_direction()
        # 获取图片信息
        self.image = self.images.get(self.direction)
        # 获取图片的矩形
        self.rect = self.image.get_rect()
        # 设置敌方坦克位置
        self.rect.left = left
        self.rect.top = top
        # 设置移动速度
        self.speed = 1
        # 设置移动的步长
        self.step = 1

    def rand_direction(self) -> str:
        '''
        生成随机方向
        '''

        choice = random.randint(1, 4)
        if choice == 1:
            return 'U'
        elif choice == 2:
            return 'D'
        elif choice == 3:
            return 'L'
        elif choice == 4:
            return 'R'

    def rand_move(self):
        '''
        随机移动
        '''
        # 判断步长是否为0
        if self.step <= 0:
            # 如果小于0,更换方向
            self.direction = self.rand_direction()
            # 重置步长
            self.step = 20
        else:
            # 如果大于0,移动
            self.move()
            # 步长减1
            self.step -= 1

    def shot(self):
        '''
        敌方坦克的射击
        '''
        num = random.randint(1, 80)
        #敌人坦克同时发送的子弹
        if num < 2:
            return Bullet(self)


class Bullet():
    '''
    子弹类
    '''

    def __init__(self, tank) -> None:
        # 加载图片
        self.image = pygame.image.load('./img/Mymissile.gif')
        # 获取子弹的方向
        self.direction = tank.direction
        # 获取子弹的图形
        self.rect = self.image.get_rect()
        # 设置子弹的位置
        if self.direction == "L":
            # 子弹的位置 = 坦克的位置 - 子弹的宽度
            self.rect.left = tank.rect.left - self.rect.width
            # 子弹的位置 = 坦克的位置 + 坦克的高度/2 - 子弹的高度/2
            self.rect.top = tank.rect.top + tank.rect.height / 2 - self.rect.height / 2
        elif self.direction == "R":
            # 子弹的位置 = 坦克的位置 + 坦克的宽度
            self.rect.left = tank.rect.left + tank.rect.width
            # 子弹的位置 = 坦克的位置 + 坦克的高度/2 - 子弹的高度/2
            self.rect.top = tank.rect.top + tank.rect.height / 2 - self.rect.height / 2
        elif self.direction == "U":
            # 子弹的位置 = 坦克的位置 + 坦克的宽度/2 - 子弹的宽度/2
            self.rect.left = tank.rect.left + tank.rect.width / 2 - self.rect.width / 2
            # 子弹的位置 = 坦克的位置 - 子弹的高度
            self.rect.top = tank.rect.top - self.rect.height
        elif self.direction == "D":
            # 子弹的位置 = 坦克的位置 + 坦克的宽度/2 - 子弹的宽度/2
            self.rect.left = tank.rect.left + tank.rect.width / 2 - self.rect.width / 2
            # 子弹的位置 = 坦克的位置 + 坦克的高度
            self.rect.top = tank.rect.top + tank.rect.height
        # 设置子弹的速度
        self.speed = 10
        # 设置子弹的状态
        self.live = True

    def display_bullet(self) -> None:
        '''
        显示子弹
        '''
        MainGame.window.blit(self.image, self.rect)

    def move(self) -> None:
        '''
        子弹的移动
        '''
        # 根据子弹生成的方向来的移动
        if self.direction == "L":
            # 判断子弹是否超出屏幕
            if self.rect.left > 0:
                self.rect.left -= self.speed
            else:
                self.live = False
        elif self.direction == "R":
            # 判断子弹是否超出屏幕
            if self.rect.left + self.rect.width < SCREEN_WIDTH:
                self.rect.left += self.speed
            else:
                self.live = False
        elif self.direction == "U":
            # 判断子弹是否超出屏幕
            if self.rect.top > 0:
                self.rect.top -= self.speed
            else:
                self.live = False
        elif self.direction == "D":
            # 判断子弹是否超出屏幕
            if self.rect.top + self.rect.height < SCREEN_HEIGHT:
                self.rect.top += self.speed
            else:
                self.live = False

    def hit_enemy_tank(self):
        for e_tank in MainGame.enemy_tank_list:
            # 判断子弹是否击中坦克
            if collide_rect(self, e_tank):
                # 爆炸效果
                explode = Explode(e_tank)
                MainGame.explode_list.append(explode)
                # 修改子弹的状态
                self.live = False
                # ser.print(1)
                # -= 1
                # if MainGame.my_tank.hp <= 0:
                #     MainGame.my_tank.live = False
                e_tank.live = False

    def hit_my_tank(self):
        # 判断我方坦克是否活着
        if MainGame.my_tank and MainGame.my_tank.live:
            # 判断字段是否击中我方坦克
            if collide_rect(self, MainGame.my_tank):
                # 爆炸效果
                explode = Explode(MainGame.my_tank)
                MainGame.explode_list.append(explode)
                # 修改子弹的状态
                self.live = False
                # 扣血

                MainGame.my_tank.hp -= 1
                # MainGame.ser.write(1)
                # 修复了hp <=1 的bug
                if MainGame.my_tank.hp < 1:
                    MainGame.my_tank.live = False

    def hit_wall(self):
        '''
        碰撞墙壁
        '''
        for wall in MainGame.wall_list:
            # 判断是否碰撞
            if collide_rect(self, wall):
                #  修改子弹的状态
                self.live = False
                # 修改墙壁的生命值
                wall.hp -= 1
                # 判断墙壁是否依然显示
                if wall.hp <= 0:
                    wall.live = False
                # 创建攻击强的音效
                music = Music('./img/hit.wav')
                music.play_music()

    # class Mybullet(Bullet):
    def __init__(self, tank) -> None:
        #
        #
        self.image = pygame.image.load('./img/Mymissile.gif')
        #

        # 获取子弹的方向
        self.direction = tank.direction
        # 获取子弹的图形
        self.rect = self.image.get_rect()
        # 设置子弹的位置
        if self.direction == "L":
            # 子弹的位置 = 坦克的位置 - 子弹的宽度
            self.rect.left = tank.rect.left - self.rect.width
            # 子弹的位置 = 坦克的位置 + 坦克的高度/2 - 子弹的高度/2
            self.rect.top = tank.rect.top + tank.rect.height / 2 - self.rect.height / 2
        elif self.direction == "R":
            # 子弹的位置 = 坦克的位置 + 坦克的宽度
            self.rect.left = tank.rect.left + tank.rect.width
            # 子弹的位置 = 坦克的位置 + 坦克的高度/2 - 子弹的高度/2
            self.rect.top = tank.rect.top + tank.rect.height / 2 - self.rect.height / 2
        elif self.direction == "U":
            # 子弹的位置 = 坦克的位置 + 坦克的宽度/2 - 子弹的宽度/2
            self.rect.left = tank.rect.left + tank.rect.width / 2 - self.rect.width / 2
            # 子弹的位置 = 坦克的位置 - 子弹的高度
            self.rect.top = tank.rect.top - self.rect.height
        elif self.direction == "D":
            # 子弹的位置 = 坦克的位置 + 坦克的宽度/2 - 子弹的宽度/2
            self.rect.left = tank.rect.left + tank.rect.width / 2 - self.rect.width / 2
            # 子弹的位置 = 坦克的位置 + 坦克的高度
            self.rect.top = tank.rect.top + tank.rect.height
        # 设置子弹的速度
        self.speed = 10
        # 设置子弹的状态
        self.live = True


class Wall:
    '''
    墙壁类
    '''

    def __init__(self, left, top) -> None:
        # 加载图片
        self.image = pygame.image.load('./img/steels.gif')
        # 获取墙壁的图形
        self.rect = self.image.get_rect()
        # 设置墙壁的位置
        self.rect.left = left
        self.rect.top = top
        # 设置墙壁的生命值
        self.hp = 0
        # 设置墙壁的状态
        self.live = True

    def display_wall(self) -> None:
        '''
        显示墙壁
        '''
        MainGame.window.blit(self.image, self.rect)


class Explode:
    '''
    爆炸效果类
    '''

    def __init__(self, tank: Tank) -> None:
        # 加载爆炸效果的图片
        self.images = [
            pygame.image.load('./img/blast0.gif'),
            pygame.image.load('./img/blast1.gif'),
            pygame.image.load('./img/blast2.gif'),
            pygame.image.load('./img/blast3.gif'),
            pygame.image.load('./img/blast4.gif'),
        ]
        # 设置爆炸效果的位置
        self.rect = tank.rect
        # 设置爆炸效果的索引
        self.step = 0
        # 获取需要渲染的图像
        self.image = self.images[self.step]
        # 设置爆炸的状态
        self.live = True

    def display_explode(self) -> None:
        '''
        显示爆炸效果
        '''
        # 判断当前爆照的效果是否播放完毕
        if self.step < len(self.images):
            # 获取当前爆炸效果的图像
            self.image = self.images[self.step]
            # 获取下一张爆炸效果的图像的索引
            self.step += 1
            # 绘制爆炸效果
            MainGame.window.blit(self.image, self.rect)
        else:
            # 初始化爆炸效果的索引
            self.step = 0
            # 设置爆炸效果的状态，代表爆炸过了
            self.live = False


class Music:
    '''
    音效类
    '''
    pygame.mixer.init()

    def __init__(self, filename: str) -> None:
        # 创建音乐文件
        pygame.mixer.music.load(filename)

    def play_music(self) -> None:
        '''
        播放音效
        '''
        pygame.mixer.music.play()


class MainGame:
    '''
    游戏主窗口类
    '''
    # 游戏主窗口对象
    data_queue = queue.Queue()
    window = None
    # 设置我放坦克
    my_tank = None
    # 存储敌方坦克的列表
    enemy_tank_list = []
    # 设置敌方坦克的数量
    enemy_tank_count = 6
    # 存储我方子弹的列表
    my_bullet_list = []
    # 存储敌方子弹的列表
    enemy_bullet_list = []
    # 存储爆炸效果的列表
    explode_list = []
    # 存储墙壁的列表
    wall_list = []

    def __init__(self) -> None:
        try:
            # 获取所有可用的串口设备
            ports = list(serial.tools.list_ports.comports())
            if len(ports) == 0:
                raise Exception("未检测到串口设备")

            # 自动连接第一个检测到的串口设备
            self.ser = serial.Serial(ports[0].device, 9600, timeout=None)

            # 启动后台线程监听串口数据
            self.read_thread = threading.Thread(target=self.read_from_serial)
            self.read_thread.daemon = True
            self.read_thread.start()

        except Exception as e:
            messagebox.showinfo("提示:坦克大战", str(e))
            print("无法连接串口设备")
            sys.exit()

    def send_serial(self):
        if MainGame.my_tank and MainGame.my_tank.live:
            if MainGame.my_tank.hp == 4:
                self.ser.write(b'\x0F')  # 发送 0F
            elif MainGame.my_tank.hp == 3:
                self.ser.write(b'\x07')  # 发送 07
            elif MainGame.my_tank.hp == 2:
                self.ser.write(b'\x03')  # 发送 03
            elif MainGame.my_tank.hp == 1:
                self.ser.write(b'\x01')  # 发送 01
            # elif MainGame.my_tank.hp == 1:
        else:
            self.ser.write(b'\x00')  # 发送 01

    def read_from_serial(self):
        while True:
            try:
                data = self.ser.read(1)  # 读取1个字节（假设每次读取1个字节表示一个方向）
                print(data)
                if data:
                    # 将数据转换为整数
                    number = int.from_bytes(data, byteorder='big')
                    # 存入队列
                    self.data_queue.put(number)
                else:
                    time.sleep(0.1)

            except Exception as e:
                print(f"Error: {e}")
                break

    def get_direction_from_data(self, timeout=0):
        try:
            # 等待队列中有数据，超时后会抛出 queue.Empty 异常
            number = self.data_queue.get(block=True, timeout=timeout)
            # 根据数据映射方向
            if number == 1:
                direction = 'U'
            elif number == 2:
                direction = 'D'
            elif number == 3:
                direction = 'L'
            elif number == 4:
                direction = 'R'
            elif number == 5:
                direction = 'S'
            elif number == 6:
                direction = 'F'

            return direction

        except queue.Empty:
            # 如果超时，返回 None 或处理超时的逻辑
            return None

    pygame.display.quit()



    def start_game(self) -> None:
        '''
        开始游戏
        '''
        # 初始化游戏窗口
        pygame.display.init()
        # 创建一个窗口
        MainGame.window = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        # 设置窗口标题
        pygame.display.set_caption('坦克大战V1.10')
        # 创建一个我方 坦克
        self.create_my_tank()
        # 创建敌方坦克
        self.create_enemy_tank()
        # 创建墙壁
        self.create_wall()
        # 刷新窗口
        while True:
            sleep(0.02)
            # 给窗口设置填充色
            MainGame.window.fill(BG_COLOR)
            # 增加提示文字
            # 1.要增加文字内容
            # num = 6
            text = self.get_text_surface(f'敌方坦克剩余数量{len(MainGame.enemy_tank_list)}')
            # 2.如何把文字加上
            MainGame.window.blit(text, (10, 10))
            # 增加事件
            self.get_event()
            # 判断我方坦克是否死亡
            if MainGame.my_tank and MainGame.my_tank.live:
                # 显示 我方坦克
                MainGame.my_tank.display_tank()
            else:
                MainGame.my_tank = None
            if len(MainGame.enemy_tank_list) == 0:
                print("You win!")
                peiqi.paintingpeiqi()
                messagebox.showinfo("提示", "You win!")
                sys.exit()
            # 显示敌方坦克
            self.display_enemy_tank()
            # 判断我方坦克是否死亡
            if MainGame.my_tank and MainGame.my_tank.live:
                # 移动坦克
                if MainGame.my_tank.remove:
                    MainGame.my_tank.move()
                    # 检测我方坦克是否与墙壁发生碰撞
                    MainGame.my_tank.tank_hit_wall()
                    # 检测我方坦克是否与敌方坦克发生碰撞
                    for enemy in MainGame.enemy_tank_list:
                        MainGame.my_tank.tank_collide_tank(enemy)

            # 显示我方子弹
            self.display_my_bullet()
            # 显示敌方子弹
            self.display_enemy_bullet()
            # 显示爆炸效果
            self.display_explode()
            # 显示墙壁
            self.display_wall()
            pygame.display.update()

    def create_wall(self) -> None:
        '''
        创建墙壁
        '''
        top = 200
        for i in range(6):
            # 创建墙壁
            wall = Wall(i * 128, top)
            # 添加到墙壁列表中
            MainGame.wall_list.append(wall)

    def display_wall(self) -> None:
        '''
        显示墙壁
        '''
        for wall in MainGame.wall_list:
            if wall.live:
                # 显示墙壁
                wall.display_wall()
            else:
                # 从列表中移除
                MainGame.wall_list.remove(wall)

    def create_my_tank(self) -> None:
        '''
        创建我方坦克
        '''
        MainGame.my_tank = MyTank(350, 300)
        # 创建音乐对象
        music = Music('./bgmusic/hezbollah1.mp3')
        # 播放音乐
        music.play_music()

    def display_explode(self) -> None:
        '''
        显示爆炸效果
        '''
        for explode in MainGame.explode_list:
            # 判断是否活着
            if explode.live:
                # 显示爆炸效果
                explode.display_explode()
            else:
                # 从列表中移除
                MainGame.explode_list.remove(explode)

    def display_my_bullet(self) -> None:
        '''
        显示我方子弹
        '''
        for my_bullet in MainGame.my_bullet_list:
            # 判断子弹是否存活
            if my_bullet.live:
                # 显示我方子弹
                my_bullet.display_bullet()
                # 移动我方子弹
                my_bullet.move()
                # 判断我方子弹是否击中敌方坦克
                my_bullet.hit_enemy_tank()
                # 判断我方子弹是否击中墙壁
                my_bullet.hit_wall()
            else:
                # 从列表中移除
                MainGame.my_bullet_list.remove(my_bullet)

    def create_enemy_tank(self) -> None:
        '''
        创建敌方坦克
        '''
        self.enemy_top = 100
        self.enemy_speed = 3
        self.hp = 2
        for i in range(self.enemy_tank_count):
            # 生成tank的位置
            left = random.randint(0, 600)
            # 创建敌方坦克
            e_tank = EnemyTank(left, self.enemy_top, self.enemy_speed, self.hp)
            # 将敌方坦克添加到列表中
            self.enemy_tank_list.append(e_tank)

    def display_enemy_tank(self) -> None:
        '''
        显示敌方坦克
        '''
        for e_tank in self.enemy_tank_list:
            #  判断敌方坦克是否存活
            if e_tank.live:
                # 显示敌方坦克
                e_tank.display_tank()
                # 移动敌方坦克
                e_tank.rand_move()
                # 判断是否与墙壁发生碰撞
                e_tank.tank_hit_wall()
                # 判断是否与我方坦克发生碰撞
                e_tank.tank_collide_tank(MainGame.my_tank)
                # 发射子弹
                e_bullet = e_tank.shot()
                # 判断是否有子弹
                if e_bullet:
                    # 将子弹增加到列表中
                    MainGame.enemy_bullet_list.append(e_bullet)
            else:
                # 从列表中移除
                self.enemy_tank_list.remove(e_tank)

    def display_enemy_bullet(self) -> None:
        '''
        显示敌方子弹
        '''
        for e_bullet in MainGame.enemy_bullet_list:
            # 显示子弹
            if e_bullet.live:
                # 如果子弹存活，显示子弹
                e_bullet.display_bullet()
                e_bullet.move()
                # 判断是否击中我方坦克
                e_bullet.hit_my_tank()
                # 判断是否击中墙壁
                e_bullet.hit_wall()
            else:
                # 如果子弹不存活，从列表中移除
                MainGame.enemy_bullet_list.remove(e_bullet)

    def get_text_surface(self, text: str) -> None:
        '''
        获取文字的图片
        '''
        # 初始化字体模块
        pygame.font.init()
        # 获取可以使用的字体
        # print(pygame.font.get_fonts())
        # 创建字体
        font = pygame.font.SysFont('kaiti', 18)
        # 绘制文字信息
        text_surface = font.render(text, True, TEXT_COLOR)
        # 将绘制的文字信息返回
        return text_surface

    def get_event(self) -> None:
        '''
        获取事件
        '''
        # 获取所有事件
        '''
        获取事件
        '''
        event_list = pygame.event.get()
        # 遍历事件
        for event in event_list:
            # 判断是什么事件，然后做出相应的处理
            if event.type == pygame.QUIT:
                # 点击关闭按钮
                self.end_game()
            if event.type == pygame.KEYDOWN:
                # 如果我方坦克死亡，按下esc键，重新生成我方坦克
                if not MainGame.my_tank and event.key == pygame.K_ESCAPE:
                    print('重新生成我方坦克')
                    # 按下esc键，重新生成我方坦克
                    self.create_my_tank()
        self.send_serial()
        if MainGame.my_tank and MainGame.my_tank.live:
            # 获取所有事件
            direction = self.get_direction_from_data()
            print("direction:", direction)
            # event_list = pygame.event.get()
            # 遍历事件
            if MainGame.my_tank and MainGame.my_tank.live:
                if direction and direction != 'F' and direction != 'S':
                    # 设置坦克的方向并移动
                    MainGame.my_tank.direction = direction
                    MainGame.my_tank.remove = True
                    MainGame.my_tank.move()
                    print(f'Tank moved {direction}')
                elif direction == 'F':
                    if len(MainGame.my_bullet_list) < 100:
                        # 发射子弹
                        print('发射子弹')
                        # 创建子弹
                        m_bullet = Bullet(MainGame.my_tank)
                        # 将子弹添加到列表中
                        MainGame.my_bullet_list.append(m_bullet)
                        # 播放发射子弹的音乐
                        music = Music('./img/fire.wav')
                        music.play_music()
                    # MainGame.my_tank.remove = False
                elif direction == 'S':
                    MainGame.my_tank.remove = False

                #显示血量

    def end_game(self) -> None:
        '''
        结束游戏
        '''
        print('谢谢使用，欢迎再次使用')
        exit()


def show_menu():
    '''
           显示菜单，等待用户点击“确定”后进入游戏
           '''

    # 创建主窗口
    light_queue = queue.Queue()
    window = tk.Tk()
    window.title("HNU 基于STC-B的智能双模经典电子游戏平台V2.0 作者：张钟瑞、张壹铭、王梓晗、唐山 --- 菜单")
    window.geometry(f"{SCREEN_WIDTH}x{SCREEN_HEIGHT}")
    window.configure(bg="lightyellow")  # 设置整个窗口背景为浅黄色

    try:
        ports = list(serial.tools.list_ports.comports())
        if len(ports) == 0:
            raise Exception("未检测到串口设备")

        # 自动连接第一个检测到的串口设备
        ser = serial.Serial(ports[0].device, 9600, timeout=None)
        # ser = serial.Serial("COM4", 9600)  # 替换成实际的串口号和波特率
        if ser.isOpen():
            print('串口已经打开')
        messagebox.showinfo("已连接串口", ports[0].device)
        # print("已连接",ports[0].device)

    except Exception as e:
        messagebox.showinfo("提示:坦克大战", str(e))
        print("未检测到串口设备")
        sys.exit()
    #判断串口是否已经打开

    # read_thread = threading.Thread(target = read_from_serial1(),args=ser)
    read_thread = threading.Thread(target=read_from_serial1, args=(ser, light_queue))
    read_thread.daemon = True
    read_thread.start()
    # ser.read()
    light = get_light(light_queue)
    if light == None:
        print("None")
    print("light=", light)
    # while 1:
    #     # 定义字节对象
    #     byte_obj = ser.read()
    #
    #     # 将字节对象转换为十进制
    #     decimal_value = int.from_bytes(byte_obj, 'big')
    #     if decimal_value < 10:
    #         print("亮度过低，请调整游戏环境")
    #     elif decimal_value > 50:
    #         print("亮度过高，请调整环境")
    #     print(decimal_value)  # 输出结果

    # 菜单提示文字
    label0 = tk.Label(window, text="按下K2后，点击【确定】按钮进入游戏", font=("SimHei", 24), bg="lightyellow", fg="black")
    label = tk.Label(window, text=f" 光照值: {light}", font=("SimHei", 24),
                     bg="lightyellow", fg="black")
    label.place(x=SCREEN_WIDTH // 2 - 250, y=SCREEN_HEIGHT // 2 - 100)  # 居中
    label0.place(x=SCREEN_WIDTH // 2 - 250, y=SCREEN_HEIGHT // 2 - 300)  # 居中
    label.pack()
    label0.pack()

    #label2==亮度提示
    if light < 10:
        label2 = tk.Label(window, text="亮度过低，请调整游戏环境", font=("SimHei", 24), bg="lightyellow", fg="black")
    elif light > 50:
        label2 = tk.Label(window, text="亮度过高，请调整游戏环境", font=("SimHei", 24), bg="lightyellow", fg="black")
    else:
        label2 = tk.Label(window, text="游戏环境合适", font=("SimHei", 24), bg="lightyellow", fg="black")
    label2.pack()

    # display hnu image
    img = Image.open("./img/hnu1.jpg")  # 替换为你的JPG图片路径
    img = img.resize((150, 150))  # 根据需要调整图片大小
    img_tk = ImageTk.PhotoImage(img)

    img_label = tk.Label(window, image=img_tk, bg="lightyellow")
    img_label.image = img_tk  # 防止图片被垃圾回收
    img_label.place(x=SCREEN_WIDTH // 2 - 80, y=125)  # 调整图片位置

    # 健康游戏忠告
    advice_text = (
        "健康游戏忠告：\n\n"
        "抵制不良游戏，拒绝盗版游戏。\n"
        "注意自我保护，谨防受骗上当。\n"
        "适度游戏益脑，沉迷游戏伤身。\n"
        "合理安排时间，享受健康生活。"
    )
    advice_label = tk.Label(window, text=advice_text, font=("SimHei", 14), bg="lightyellow", fg="black",
                            justify="left")
    advice_label.place(x=SCREEN_WIDTH // 2 - 150, y=SCREEN_HEIGHT // 2 + 50)  # 居中显示忠告

    # "确定"按钮
    button = tk.Button(window, text="确定", font=("SimHei", 20), bg="black", fg="white", command=window.quit)
    button.place(x=SCREEN_WIDTH // 2 - 50, y=SCREEN_HEIGHT // 2 + 200)

    # 进入事件循环，等待用户点击按钮
    window.mainloop()
    ser.close()
    time.sleep(1)
    # 当窗口关闭后返回，退出菜单
    return


def read_from_serial1(ser, queue):
    while True:
        try:
            data = ser.read(1)  # 读取1个字节
            if data:
                # 将数据转换为整数
                number = int.from_bytes(data, byteorder='big')
                # 存入队列
                queue.put(number)
                print("当前亮度", number)
            else:
                time.sleep(0.1)  # 如果没有数据，稍作等待
        except Exception as e:
            print("Error reading from serial:", e)
            break  # 如果发生错误，退出循环


def get_light(queue_name):
    try:
        # 等待队列中有数据，超时后会抛出 queue.Empty 异常
        number = queue_name.get(block=True, timeout=0.5)
        print("number = ", number)
        # 根据数据映射方向
        return number

    except queue.Empty:
        # 如果超时，返回 None 或处理超时的逻辑
        return None




if __name__ == "__main__":
    # 调用MainGame类中的start_game方法,开始游戏
    # mode = input("输入游戏模式")
    # print("mode",mode)

    show_menu()
    MainGame().start_game()
    # elif mode == 1:
    #     plane.game_start()
