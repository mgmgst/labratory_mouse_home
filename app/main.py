import requests
import mysql.connector
from datetime import datetime

def getroomstatus():
    '''this function read room temperature and humadity'''

    url = 'http://10.10.10.1/readroomddata'
    respon = requests.get(url)
    hum = respon.json()['humadity']
    temp = respon.json()['temperature']

    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    writing_dhtstatus_to_database(hum, temp, timestamp)

    return temp, hum

def getpirstatus():
    '''this function read human motion detect in work place'''

    url = 'http://10.10.10.1/readpirdata'
    respon = requests.get(url)
    motion = respon.json()['motion']
    
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    writing_pirstatus_to_database(motion,timestamp)

    return motion

def ledcontrol(led,status):
    '''this function can control showing bar status leds'''

    if ledcheck(led,status):
        url = f'http://10.10.10.1/control_led?{led}={status}'
        respon = requests.get(url)
        redled = respon.json()['redled']
        whiteled = respon.json()['whiteled']
        yellowled = respon.json()['yellowled']
        
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        writing_ledstatus_to_database(redled,whiteled,yellowled,timestamp)
        
        return redled, whiteled, yellowled
    
    else:
        return 'your givin elements is not true'

def relaycontrol(relaypin, status):
    '''this function can control relay [<exept for now can control relay1 that can control big panel light>] that control lights and power stuffs'''
    
    if relaycheck(relaypin,status):
        url = f'http://10.10.10.1/control_relay?{relaypin}={status}'
        respon = requests.get(url)

        stuffs = {'light' : respon.json()['relay1_Status'],
        'fan(in)' : respon.json()['relay2_Status'],
        'fan(out)' : respon.json()['relay3_Status'],}
        
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        writing_relaystatus_to_database(stuffs['light'],stuffs['fan(in)'],stuffs['fan(out)'],timestamp)

        return stuffs
    else:
        return 'your givin elements is not true'

def logout():
    '''this function logout from server'''

    url = "http://10.10.10.1/login?DISCONNECT=YES"    
    respon = requests.get(url)

    return respon
    
def login():
    '''this function login into server'''

    username = 'mgmgst'
    password = '1051154731'
    url = f'http://10.10.10.1/login?USERNAME={username}&PASSWORD={password}'
    respon = requests.get(url)

    return respon
            
def ledcheck(led,status):
    '''this function check that we send right data for controling status led light bars or not'''

    ret = False
    leds = ['ledred' , 'ledyellow' , 'ledwhite','all']
    statuss = ['on' , 'off']
    if led in leds and status in statuss:
        ret = True
        
    return ret

def relaycheck(relaypin, status):
    '''this function check that we send right data for controling [<exept for now can check relay1 that can control big panel light>] relays that they control power stuff or not'''
    ''' in this function relay1 == light & relay2 == fan(in) & relay3 == fan(out) '''

    ret = False
    relays = ['relay1', 'relay2', 'relay3']
    statuss = ['on', 'off']
    if relaypin in relays and status in statuss:
        ret = True
        
    return ret

def connect_to_database():
    '''this function make connection to mysql service'''

    db = mysql.connector.connect(host="localhost",
                       user="alldatas",
                       port=3306,
                       password="test",
                       db="alldatas",
                       charset="utf8")
    return db  

def reading_ledstatus_from_database():
    '''this function read ledstatuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM ledstatus;")
    db.close()
    return cur.fetchall()

def reading_relaystatus_from_database():
    '''this function read relaystatuss from mysql database from  past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM relaystatus;")
    db.close()
    return cur.fetchall()

def reading_pirstatus_from_database():
    '''this function read pirstatuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM pirstatus;")
    db.close()
    return cur.fetchall()

def reading_dhtstatus_from_database():
    '''this function read dhtstatuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM dhtstatus;")
    db.close()
    return cur.fetchall()            

def writing_ledstatus_to_database(redled, whiteled, yellowled, timestamp):
    '''this function write collected ledstatuss to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO ledstatus VALUES ("{redled}","{whiteled}","{yellowled}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()

def writing_relaystatus_to_database(light, fanin, fanout, timestamp):
    '''this function write collected relaystatuss to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO relaystatus VALUES ("{light}","{fanin}","{fanout}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()

def writing_pirstatus_to_database(motion,timestamp):
    '''this function write collected pirstatuss to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO pirstatus VALUES ("{motion}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()

def writing_dhtstatus_to_database(hum, temp, timestamp):
    '''this function write collected dhtstatuss to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO dhtstatus VALUES ("{hum}","{temp}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()    
