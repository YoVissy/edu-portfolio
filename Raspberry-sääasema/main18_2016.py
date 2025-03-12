#
#      ICT kurssin Raspi koodi  6.1.2016  KN
#

import paho.mqtt.client as mqtt
import time, datetime, threading
import urllib2, sys

from flask import Flask, render_template , request


#   
#
#      0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 
#      0,1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0,1,2,3,4,5,6,7,8,9,0,1,2 3 4 5           mittaustiedot taulukko
mit = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];

#
#
#  Lahetys ajastus looppi  lahetys 2 minut valein
#
def lahetys_dbeen():

   
        
    ltime = time.localtime(time.time())
    print 'TIME =:',ltime.tm_hour,':',ltime.tm_min,'.',ltime.tm_sec,"\r",
    
    sys.stdout.flush()
    
    sek=int(ltime.tm_sec)
    sek_t=sek;
    minu=int(ltime.tm_min)
    minu=minu % 2             # parilliset ja parittomat minuutit

    sek_t=sek_t % 30


    ############################## kanavat 1-10 alku ##########################
    
    
    if ((minu == 1) & (sek == 40 )):    # paril minuuttu ja sekuntti = 40
    #if sek == 1:                       # nopeasykli testukseen

     print "\n"
     print ' Mittaukset 1-12 Lahetys'

     
     global mit     

     st1="%04.2f" % float(mit[1]/100.0) #tuulen nopeus 1
     st2="%01d" % float(mit[2]/100.0)	#tuulen suunta 1
     st3="%04.1f" % float(mit[3]/100.0)	#lampotila 1	
     st4="%04.1f" % float(mit[4]/100.0) #sademaara 1
     st5="%04.1f" % float(mit[5]/100.0)	#kosteus sisa
     st6="%04.1f" % float(mit[6]/100.0)	#kosteus ulko
     
     st7="%04.2f" % float(mit[7]/100.0) #tuulen nopeus 2
     st8="%01d" % float(mit[8]/100.0)	#tuulen suunta 2
     st9="%04.1f" % float(mit[9]/100.0)	#lampotila 2	
     st10="%04.1f" % float(mit[10]/100)	#sademaara 2
     st11="%04.1f" % float(mit[11]/100)	#ilmanpaine 1
     st12="%04.1f" % float(mit[12]/100)	#ilmanpaine 2    

     print 'T-nop 1=',st1,
     print ' T-suun 1=',st2,
     print ' Lamp 1=',st3,
     print ' Sade 1=',st4,   
     print ' Kost S=',st5,
     print ' Kost U=',st6
     
     print 'T-nop 2=',st7,
     print ' T-suun 2=',st8,
     print ' Lamp 2=',st9,
     print ' Sade 2=',st10,
     print ' ilmp 1=',st11,
     print ' ilmp 2=',st12
     print

     

     ######################### kanavat 1-12 loppu #####################
     ######################### kanavat 21-32 alku #####################

     
    if ((minu == 1) & (sek == 50 )):    # paril minuuttu ja sekuntti = 40
                           
     
     print "\n"
     print ' Mittaukset 21-32 Lahetys'
         

     st21="%04.2f" % float(mit[21]/100.0)   #tuulen nopeus 1
     st22="%01d" % float(mit[22]/100.0)	    #tuulen suunta 1
     st23="%04.1f" % float(mit[23]/100.0)   #lampotila 1	
     st24="%04.1f" % float(mit[24]/100.0)   #sademaara 1
     st25="%04.1f" % float(mit[25]/100.0)   #kosteus sisa
     st26="%04.1f" % float(mit[26]/100.0)   #kosteus ulko
     
     st27="%04.2f" % float(mit[27]/100.0)   #tuulen nopeus 2
     st28="%01d" % float(mit[28]/100.0)	    #tuulen suunta 2
     st29="%04.1f" % float(mit[29]/100.0)   #lampotila 2	
     st30="%04.1f" % float(mit[30]/100)	    #sademaara 2
     st31="%04.1f" % float(mit[31]/100)	    #ilmanpaine 1
     st32="%04.1f" % float(mit[32]/100)	    #ilmanpaine 2

     print 'T-nop 1=',st21,
     print ' T-suun 1=',st22,
     print ' Lamp 1=',st23,
     print ' Sade 1=',st24,   
     print ' Kost S=',st25,
     print ' Kost U=',st26
     
     print 'T-nop 2=',st27,
     print ' T-suun 2=',st28,
     print ' Lamp 2=',st29,
     print ' Sade 2=',st30,
     print ' ilmp 1=',st31,
     print ' ilmp 2=',st32
     print

   

     ######################### kanavat 21-32 loppu ####################

    threading.Timer(1, lahetys_dbeen).start()

lahetys_dbeen()


#######################################################################
####   MQTT lukeminen
#######################################################################

def on_connect(mosq, obj, rc):
    print("rc: " + str(rc))

###############################
###    MQTT sanoma tullut
##############################    


def on_message(mqttc, obj, msg):
    global message_in
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload)),
    message_in = msg.payload


    ch=0
    m1=0
    m=0

    ch = ch + 10 * (ord (message_in[0])-48) + (ord(message_in[1])-48)

    print " CH :",
    
    print ch,
    
    if ((ch<34) & (ch>0) & (len(message_in)>8)):

        #
        #       Rakennetaan mittausarvo luvuksi MQTT sanomasta
        #
        print " Mit= ",
        m1=int (message_in[3])
        m=m1 * 100000
        m1=int (message_in[4])
        m=m+ 10000 * m1
        m1=int (message_in[5])
        m=m+ 1000 * m1
        m1=int (message_in[6])
        m=m+ 100 * m1
        m1=int (message_in[7])
        m=m+ 10 * m1
        m1=int (message_in[8])
        m=m+ m1       

        #        Tulosta mittausarvo lukuna
        print m
        mit[ch]=m


def on_publish(mosq, obj, mid):
    global message
    #print("mid: " + str(mid))


mqttc = mqtt.Client()
mqttc.on_publish = on_publish

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(mosq, obj, level, string):
    print(string)
                   


# Assign event callbacks
mqttc.on_message = on_message

mqttc.on_connect = on_connect

mqttc.on_subscribe = on_subscribe

# Connect
mqttc.connect("localhost", 1883,60)

# Start subscribe, with QoS level 0
mqttc.subscribe("outTopic", 0)

# Publish a message
mqttc.publish("outTopic", "my message")


# Continue the network loop, exit when an error occurs

mqttc.loop_start()



app = Flask(__name__)


@app.route("/")                 
 
 
@app.route("/", methods=['POST'])  
 
def mymenu():

	return 1 
 
     
if __name__ == "__main__":
   app.run(host='0.0.0.0', port=80, debug=False)       #Run flask app !




        
         
