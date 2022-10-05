import RPi.GPIO as GPIO
import time
 
ledPin = 22
 
def setup():
        GPIO.setmode(GPIO.BOARD) # GPIO Numbering of Pins
        GPIO.setup(ledPin, GPIO.OUT) # Set ledPin as output
        GPIO.output(ledPin, GPIO.LOW) # Set ledPin to LOW to turn Off the LED
 
def loop():
        while True:
                print 'LED on'
                GPIO.output(ledPin, GPIO.HIGH) # LED On
                time.sleep(1.0) # wait 1 sec
                print 'LED off'
                GPIO.output(ledPin, GPIO.LOW) # LED Off
                time.sleep(1.0) # wait 1 sec
def endprogram():
 
        GPIO.output(ledPin, GPIO.LOW) # LED Off
        GPIO.cleanup() # Release resources
 
if name == 'main':
        setup()
        try:
                loop()
        except KeyboardInterrupt:
                endprogram()
