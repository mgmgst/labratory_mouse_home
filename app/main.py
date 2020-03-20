import requests

def getroomstatus():
    '''this function read room temperature and humadity'''
    url = 'http://10.10.10.1/readroomddata'
    respon = requests.get(url)
    hum = respon.json()['humadity']
    temp = respon.json()['temperature']
    
    return temp, hum

def getpirstatus():
    '''this function read human motion detect in work place'''
    url = 'http://10.10.10.1/readpirdata'
    respon = requests.get(url)
    motion = respon.json()['motion']

    return motion

def ledcontrol(led,status):
    '''this function can control showing bar status leds'''
    if ledcheck(led,status):
        url = f'http://10.10.10.1/control_led?{led}={status}'
        respon = requests.get(url)
        redled = respon.json()['redled']
        whiteled = respon.json()['whiteled']
        yellowled = respon.json()['yellowled']
        
        return redled, whiteled, yellowled
    
    else:
        print('your givin elements is not true')

def relaycontrol(relaypin, status):
    '''this function can control relay [<exept for now can control relay1 that can control big panel light>] that control lights and power stuffs'''
    if relaycheck(relaypin,status):
        url = f'http://10.10.10.1/control_relay?{relaypin}={status}'
        respon = requests.get(url)
        relay_status = respon.json()['relay_Status']
        
        return relay_status
    else:
        print('your givin elements is not true')

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
    ret = False
    relay = 'relay1'
    statuss = ['on', 'off']
    if relaypin == relay and status in statuss:
        ret = True
        
    return ret
ledcontrol("ledred","off")
