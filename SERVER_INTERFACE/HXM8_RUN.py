from socket import*
import binascii 
import unicodedata
import codecs
import time

#f = open("demofile.txt", "w")

def convert_to_command(identifier, data):
    """determine the device that is communicating to the server"""
    if(identifier & 0x4):
        device = 1
    elif(identifier & 0x8):
        device = 2
    elif(identifier & 0x10):
        device = 3
    elif(identifier & 0x20):
        device = 4
    elif(identifier & 0x40):
        device = 5
    elif(identifier & 0x80):
        device = 6
    elif(identifier & 0x100):
        device = 7
    elif(identifier & 0x200):
        device = 8
    elif(identifier & 0x400):
        device = 9
    
    """determine the type of message being sent"""
    if(identifier & 0x1):
        message = "W"
    elif(identifier & 0x2):
        message = "E"
    elif(identifier & 0x3):
        message = "CE"
    else:
        message = "I"

    """configure the message being sent in latin-1 type encoding"""
    control_message = str(hex(data))
    control_message = control_message[2:]
    control_message = codecs.decode(codecs.decode(control_message,'hex'), 'latin-1')

    """configure time locally message was received"""
    time_now = time.localtime()

    log_msg = (message + " (" + str(time_now.tm_year) + str(time_now.tm_mon) + str(time_now.tm_mday) + " | " + str(time_now.tm_hour) + str(time_now.tm_min) + ":" + str(time_now.tm_sec) + ") DEVICE " + str(device) + ": " + str(control_message) + "\n")
    
    print(log_msg)
    f = open("demofile.txt", "a")
    f.write(log_msg)
    f.close()
 





def main():
    PORT = 25250
    MSG_RECEIVE = ''

    i = 0
    data_top = 0
    data_bottom = 0
    identifier = 0

    print('Application Ready')    

    serverSocket = socket(AF_INET,SOCK_STREAM)
    serverSocket.bind(('',PORT))
    serverSocket.listen(1)
    connectionSocket, addr = serverSocket.accept()


    while(1):
        MSG_RECEIVE = connectionSocket.recv(1024) #wait for device to send messages to server
        MSG_RECEIVE = MSG_RECEIVE
        MSG_RECEIVE = binascii.hexlify(MSG_RECEIVE).decode() #turn message to proper hexadecimal format
        MSG_RECEIVE = bytearray.fromhex(MSG_RECEIVE)    #turn message into a byte arra
        MSG_RECEIVE.reverse()   #reverse the message format
        MSG_RECEIVE = int(binascii.hexlify(MSG_RECEIVE).decode(), 16) #turn the message into an integer hexadecimal form
        
        #used in order to receive the identifier, and databytes from the ESP8266
        if(i == 0):
            identifier = MSG_RECEIVE    
        elif(i == 1):
            data_top = MSG_RECEIVE
        elif(i == 2):
            data_bottom = MSG_RECEIVE
            data = (data_top << 32) | (data_bottom)
            convert_to_command(identifier, data)#used to convert into text that will be displayed on the terminal and written to a log file.

        response = 'RECEIVED'

        connectionSocket.send(response.encode())

        i+=1

        if((i > 2) or (data_top > 0xFFFFFFFF)):
            i = 0
            identifier = 0
            data_top = 0
            data_bottom = 0

if __name__ == "__main__":
    main()