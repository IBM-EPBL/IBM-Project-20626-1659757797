import random 

def randValueGenerator():
    temperature= random.randint(10,200)
    humidity= random.randint(0,50)
    return [temperature,humidity]

while(1):
    temp= randValueGenerator()[0]
    hum= randValueGenerator()[1]
    if temp>50 or hum>=65:
        print("Alarm ON")
    else:
        print("Alarm OFF")
