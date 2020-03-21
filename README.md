# title : labratory_mouse_home

[![LICENSE](https://img.shields.io/badge/LICENSE-GPL--3.0-green)](https://github.com/jadijadi/sms_serial_verification/blob/master/LICENSE) 
[![Requirements](https://img.shields.io/badge/Requirements-See%20Here-orange)](https://github.com/jadijadi/sms_serial_verification/blob/master/requirements.txt)
[![Todo](https://img.shields.io/badge/Todo-See%20Here-success)](https://github.com/jadijadi/sms_serial_verification/blob/master/TODO.md)

assay subject : effect of electro magnetic fields on human and live creaters
authors : mohammad gharehbagh && sohil shoravarzi

## Todolist :
- [x] make comment and introduction in both code sites
- [x] write readme.mi and make TODOS on it
- [x] add this project on git 
- [x] make aparat link for project and add it to bottom aparat link
- [x] add my sql service to it and sava all of data on it and write the help for this in readme.md <{[id , {info}, timestamp , ... ]}>
- [x] add servo motor + ir distance sensor to all thing as shematic + code >> arduino and python and mysql service and ...
- [x] make data sheet for all ellectrical part of the work <(servo motor and ir distance sensor)>
- [ ] coninue makeing the electrical parts
- [ ] recording all (test) and (on way makeing and ...) for this project
- [ ] recordeing the voice of meets that i have with (sohil) for all parts of project
- [ ] make graphical user interface for showing results and charts desktop app or website web service
- [ ] writeing assay for result of electromagnetic fields on human and live creaters
- [ ] all works must be text and haveing some things from them
- [ ] write the book for it
- [ ] getting invention certificate for handmade animalkepping dvice that i make it
- [ ] send assay for ISI and so many other diferrent places
- [ ] getting help from mr.hosseini and mr.khaderian
- [ ] adding more professional persons to te project
- [ ] engining the all work and make the main pattern for all work
- [ ] makeing graphical pattern for done the project

This project is in way for writing assay for top subject. 

<div dir="rtl"> 
 در این پروژه از تکنولوژی های زیر استفاده می شه:

- پایتون
- هسته وای فای esp8266
- مای اسکوئل
- الکترونیک
- میکروکنترلر ها و میکرو پرسسور ها

کل ویدئوها رو می تونین از لینک های زیر ببینین.
</div>

Every single step of this project is screen captures and you can follow them [On Aparat](https://www.aparat.com/assaysohil). 

## How to run
1. Install python3, pip3, virtualenv, arduino+complete(librarys and bourds), MySQL in your system.
2. Clone the project `git clone https://github.com/mgmgst/labratory_mouse_home.git && cd labratory_mouse_home`
5. Create a virtualenv named venv using `virtualenv -p python3 venv`
6. Connect to virtualenv using `source venv/bin/activate`
7. From the project folder, install packages using `pip install -r requirements.txt`
8. Now environment is ready. 
9. تمام شما الان میتونید رو پروژه کار کنین

### make mysql server for project
1. run this comand in MYSQL database : `CREATE DATABASE alldatas;`

2. run this comand in MYSQL database : `CREATE USER 'alldatas'@'localhost' IDENTIFIED BY 'test';`

3. run this comand in MYSQL database : `GRANT ALL PRIVILEGES ON alldatas.* TO 'alldatas'@'localhost';`

4. run this comand in MYSQL database : `DROP TABLE IF EXISTS ledstatus;`

5. db configs are in config.py. Create the db and grant all access to the specified user with specified password, but you also need to add this table to the database manually: `CREATE TABLE ledstatus (redled VARCHAR(30),whiteled VARCHAR(30) , yellowled VARCHAR(30) , timestamp  TIMESTAMP);`

6. run this comand in MYSQL database : `DROP TABLE IF EXISTS relaystatus;`

7. db configs are in config.py. Create the db and grant all access to the specified user with specified password, but you also need to add this table to the database manually: `CREATE TABLE relaystatus (light VARCHAR(30),fans VARCHAR(30) ,timestamp  TIMESTAMP);`

8. run this comand in MYSQL database : `DROP TABLE IF EXISTS pirstatus;`

9. db configs are in config.py. Create the db and grant all access to the specified user with specified password, but you also need to add this table to the database manually: `CREATE TABLE pirstatus (motion VARCHAR(30), timestamp  TIMESTAMP);`

10. run this comand in MYSQL database : `DROP TABLE IF EXISTS irstatus;`

11. db configs are in config.py. Create the db and grant all access to the specified user with specified password, but you also need to add this table to the database manually: `CREATE TABLE irstatus (switch VARCHAR(30), timestamp  TIMESTAMP);`

12. run this comand in MYSQL database : `DROP TABLE IF EXISTS dhtstatus;`

13. db configs are in config.py. Create the db and grant all access to the specified user with specified password, but you also need to add this table to the database manually: `CREATE TABLE dhtstatus (hum VARCHAR(30),temp VARCHAR(30) , timestamp  TIMESTAMP);`

14. run this comand in MYSQL database : `DROP TABLE IF EXISTS servostatus;`

15. db configs are in config.py. Create the db and grant all access to the specified user with specified password, but you also need to add this table to the database manually: `CREATE TABLE servostatus (servostatus VARCHAR(30),timestamp  TIMESTAMP);`
