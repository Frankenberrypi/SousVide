import serial  # Used to communicate with the Arduino
import time    # Used to delay so you don't get email every 5 seconds
import smtplib # Used to send email

def send_email(Subject):  # Module to send email with subject as arg[1]
  TO = 'JeffreyJBeckman@gmail.com'
  gmailUser = 'Frankenduino@gmail.com'  # Frank Duino, the Frankenduino
  gmailPass = 'F&R6hlYy0v6*'            # SHHHH!!! It's a secret
  emailBody = 'The temperature is ' + values[1] + ' degrees F.' 
  
  #print("Sending Email")
  smtpserver = smtplib.SMTP("smtp.gmail.com",587)
  smtpserver.ehlo()
  smtpserver.starttls()
  smtpserver.ehlo
  smtpserver.login(gmailUser, gmailPass)
  header = 'To:' + TO + '\n' + 'From: ' + gmailUser
  header = header + '\n' + 'Subject:' + Subject + '\n'
  #print (header)
  msg = header + '\n' + emailBody + ' \n\n'
  smtpserver.sendmail(gmailUser, TO, msg)
  smtpserver.close()

ser = serial.Serial('COM7',9600)
delay = 1800 #delay in seconds
msgHot = "It's Hot"
msgCold = "It's Cold"
tempHot = 90
tempCold = 60

while True:
  output = ser.readline()
  line = str(output.strip())
  value = line.rstrip("'")
  value = value.lstrip("b'")
  #temp = float(value)
  print(value)
  outFile = open('temps.out', 'a')
  outFile.write(value + "\n")
  values = value.split(',')
  temp = float(values[1])
  delay += 1
  
  
  if temp > 150 and delay > 1800:
    send_email(msgHot)
    delay = 0
    '''
    ser.close()
    print ("waiting " + str(delay) + " seconds")
    time.sleep(delay/4)
    print (str(3*delay/4) + " seconds remaining")
    time.sleep(delay/4)
    print (str(2*delay/4) + " seconds remaining")
    time.sleep(delay/4)
    print (str(1*delay/4) + " seconds remaining")
    time.sleep(delay/4)
    ser = serial.Serial('COM7',9600)
    '''
  
  if temp < 60 and delay > 1800:
    send_email(msgCold)
    delay = 0
    '''
    ser.close()
    print ("waiting " + str(delay) + " seconds")
    time.sleep(delay/4)
    print (str(3*delay/4) + " seconds remaining")
    time.sleep(delay/4)
    print (str(2*delay/4) + " seconds remaining")
    time.sleep(delay/4)
    print (str(1*delay/4) + " seconds remaining")
    time.sleep(delay/4)
    ser = serial.Serial('COM7',9600)
    '''
    
ser.close()
