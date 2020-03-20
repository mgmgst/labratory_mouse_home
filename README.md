# labratory_mouse_homesubject : effect of electro magnetic fields on human and live creaters

authors : mohammad gharehbagh && sohil shoravarzi

1.make comment and introduction in both code sites
2.engining the all work and make the main pattern for all work
3.makeing graphical pattern for done the project
5.write readme.mi and make TODOS on it
6.add my sql service to it and sava all of data on it and write the help for this in readme.md
7.make data sheet for all ellectrical part of the work
8.coninue makeing the electrical parts
9.add this project on git 
10.add movies that record from this project on youtube.com site and write the adress in in readme.md
11.recording all (test) and (on way makeing and ...) for this project
12.recordeing the voice of meets that i have with (sohil) for all parts of project
13.make graphical user interface for showing results and charts
14.writeing assay for result of electromagnetic fields on human and live creaters
15.all works must be text and haveing some things from them
16.write the book for it
17.getting invention certificate for handmade animalkepping dvice that i make it
18.send assay for ISI and so many other diferrent places
19.getting help from mr.hosseini and mr.khaderian
20.adding more professional persons to te project

# sms_verification

[![LICENSE](https://img.shields.io/badge/LICENSE-GPL--3.0-green)](https://github.com/jadijadi/sms_serial_verification/blob/master/LICENSE) 
[![Requirements](https://img.shields.io/badge/Requirements-See%20Here-orange)](https://github.com/jadijadi/sms_serial_verification/blob/master/requirements.txt)
[![Todo](https://img.shields.io/badge/Todo-See%20Here-success)](https://github.com/jadijadi/sms_serial_verification/blob/master/TODO.md)

This project is done for Altech (Schneider Electric Iran) as an educational series. 

<div dir="rtl"> 
این پروژه ای است به سفارش آلتک (اشنایدر الکتریک ایران) برای سنجش صحت شماره سریال ها با پیامک. من پروژه رو ازشون قبول کردم به این شرط که همه مراحلش رو ضبط و منتشر کنم تا نمونه ای باشه از انجام یک پروژه واقعی توسط یک فری لنسر. در این پروژه از تکنولوژی های زیر استفاده می شه:

- پایتون
- فلسک
- ای پی آی های دریافت و ارسال اسمس از درگاه پیامک کاوه نگار
- پاس فندق
- مای اسکوئل

کل ویدئوها رو می تونین از لینک های زیر ببینین.
</div>

Every single step of this project is screen captures and you can follow them [On youtube](https://www.youtube.com/playlist?list=PL-tKrPVkKKE1vAT_rgjnvL_RgFUI9oJ9a) or [On Aparat](https://www.aparat.com/v/fAZSV?playlist=288572). 

## How to run
1. Install python3, pip3, virtualenv, MySQL in your system.
2. Clone the project `git clone https://github.com/jadijadi/sms_serial_verification && cd sms_serial_verification`
3. in the app folder, rename the `config.py.sample` to `config.py` and do proper changes.
4. db configs are in config.py. Create the db and grant all access to the specified user with specified password.
5. Create a virtualenv named venv using `virtualenv -p python3 venv`
6. Connect to virtualenv using `source venv/bin/activate`
7. From the project folder, install packages using `pip install -r requirements.txt`
8. Now environment is ready. Run it by `python app/main.py`

## Example of creating db and granting access:

> Note: this is just a sample. You have to find your own systems commands.

```
CREATE DATABASE smsmysql;
USE smsmysql;
CREATE USER 'smsmysql'@'localhost' IDENTIFIED BY 'test' PASSWORD NEVER EXPIRE;
GRANT ALL PRIVILEGES ON smsmysql.* TO 'smsmysql'@'localhost';
```

