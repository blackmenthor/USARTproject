#!/usr/bin/python3
import serial
import timeit

PORT = 'COM1' #Isi dengan nama port USB
ser = serial.Serial(port = PORT)	

def main():
	while(True):
		msg = input('Masukan pesan: ')
		ser.write(msg)
		ACK = ""
		start = timeit.timeit()
		while(ACK==""):
			ACK=ser.readline()
		end = timeit.timeit()
		print('[+]Time: ' +  str(end-start))

if __name__ == "__main__":
    main()
