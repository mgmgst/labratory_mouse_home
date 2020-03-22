import requests
import mysql.connector
from datetime import datetime
from time import sleep

def getalldatas():
    '''this function get all datas by one json code and store them into mysql database in each seprate tables and in one great table'''

    url = 'http://10.10.10.1/jsond'
    respon = requests.get(url)
    
    hum = respon.json()['humadity']
    temp = respon.json()['temperature']
    motion = respon.json()['motion']
    switch = respon.json()['switch']
    redled = respon.json()['redled']
    yellowled = respon.json()['yellowled']
    light = respon.json()['light']
    fans = respon.json()['fans']
    servostatus = respon.json()["servo"]
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    writing_dhtstatus_to_database(hum, temp, timestamp)
    writing_pirstatus_to_database(motion, timestamp)
    writing_irstatus_to_database(switch, timestamp)
    writing_ledstatus_to_database(redled, yellowled, timestamp)
    writing_relaystatus_to_database(light, fans, timestamp)
    writing_servostatus_to_database(servostatus, timestamp)

    writing_alldatas_to_database(hum, temp, motion, switch, redled, yellowled, light, fans, servostatus, timestamp)

    return hum, temp, motion, switch, redled, yellowled, light, fans, servostatus

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

def getirstatus():
    '''this function read food switch status in work place'''

    url = 'http://10.10.10.1/readirdata'
    respon = requests.get(url)
    switch = respon.json()['switch']
    
    timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    writing_irstatus_to_database(switch,timestamp)

    return switch

def ledcontrol(led,status):
    '''this function can control showing bar status leds'''

    if ledcheck(led,status):
        url = f'http://10.10.10.1/control_led?{led}={status}'
        respon = requests.get(url)
        redled = respon.json()['redled']
        yellowled = respon.json()['yellowled']
        
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        writing_ledstatus_to_database(redled,yellowled,timestamp)
        
        return redled, yellowled
    
    else:
        return 'your givin elements is not true'

def relaycontrol(relaypin, status):
    '''this function can control relays that control lights and fans'''
    
    if relaycheck(relaypin,status):
        url = f'http://10.10.10.1/control_relay?{relaypin}={status}'
        respon = requests.get(url)

        stuffs = {'light' : respon.json()['light'],
        'fans' : respon.json()['fans'],}
        
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        writing_relaystatus_to_database(stuffs['light'],stuffs['fans'],timestamp)

        return stuffs
    else:
        return 'your givin elements is not true'

def servocontrol(servo, status):
    '''this function can control servo that control food door'''
    
    if relaycheck(servo,status):
        url = f'http://10.10.10.1/control_servo?{servo}={status}'
        respon = requests.get(url)

        servostatus = respon.json()["servo"]
        
        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        
        writing_servostatus_to_database(servostatus,timestamp)

        return servostatus
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
    leds = ['ledred' , 'ledyellow' ,'all']
    statuss = ['on' , 'off']
    if led in leds and status in statuss:
        ret = True
        
    return ret

def relaycheck(relaypin, status):
    '''this function check that we send right data for controling relays that they control power stuff or not'''
    ''' in this function relay1 == light & relay2 == fans '''

    ret = False
    relays = ['light', 'fans']
    statuss = ['on', 'off']
    if relaypin in relays and status in statuss:
        ret = True
        
    return ret

def servocheck(servo, status):
    '''this function check that we send right data for controling servo that control food door'''

    ret = False
    servop = "servo"
    statuss = ['open', 'close']
    if servo == servop and status in statuss:
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

def reading_servostatus_from_database():
    '''this function read servostatus from mysql database from  past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM servostatus;")
    db.close()
    return cur.fetchall()

def reading_pirstatus_from_database():
    '''this function read pirstatuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM pirstatus;")
    db.close()
    return cur.fetchall()

def reading_irstatus_from_database():
    '''this function read irstatuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM irstatus;")
    db.close()
    return cur.fetchall()

def reading_dhtstatus_from_database():
    '''this function read dhtstatuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM dhtstatus;")
    db.close()
    return cur.fetchall()            

def reading_alldatas_to_database():
    '''this function read alldatas statuss from mysql database from past to today and return them'''

    db = connect_to_database()
    cur = db.cursor()
    cur.execute("SELECT * FROM alldatasstatus;")
    db.close()
    return cur.fetchall()     

def writing_ledstatus_to_database(redled, yellowled, timestamp):
    '''this function write collected ledstatuss to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO ledstatus VALUES ("{redled}","{yellowled}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()

def writing_relaystatus_to_database(light, fans, timestamp):
    '''this function write collected relaystatuss to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO relaystatus VALUES ("{light}","{fans}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()

def writing_servostatus_to_database(servostatus,timestamp):
    '''this function write collected servostatus to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO servostatus VALUES ("{servostatus}","{timestamp}");'
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

def writing_irstatus_to_database(switch,timestamp):
    ''' this function write collected irstatuss to mysql databse with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO irstatus VALUES ("{switch}","{timestamp}");'
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

def writing_alldatas_to_database(hum, temp, motion, switch, redled, yellowled, light, fans, servostatus, timestamp):
    '''this function write all collected datas to mysql database with timestamp'''

    db = connect_to_database()    
    cur = db.cursor()
    qury = f'INSERT INTO alldatasstatus VALUES ("{hum}","{temp}","{motion}","{switch}","{redled}","{yellowled}","{light}","{fans}","{servostatus}","{timestamp}");'
    cur.execute(qury)
    db.commit()
    db.close()
