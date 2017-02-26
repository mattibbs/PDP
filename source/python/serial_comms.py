import serial, time
import xml.etree.ElementTree as ET
ser = serial.Serial("COM6",9600,timeout=1)
while 1 :
	params = ser.readline().decode("utf-8").strip().split(":")
	print(params)
	if len(params[0]) > 0 :
		tdb = time.strftime('%d/%m/%Y')
		tdc = time.strftime('%H:%M:%S')		
		print(params)
		tree = ET.parse("history.xml")
		root = tree.getroot()
		print(root)
		a = ET.Element("EVENT")
		b = ET.SubElement(a, "BYTE0")
		b.text = params[0]
		b = ET.SubElement(a, "BYTE1")
		b.text = params[1]
		b = ET.SubElement(a, "BYTE2")
		b.text = params[2]
		b = ET.SubElement(a, "BYTE3")
		b.text = params[3]
		b = ET.SubElement(a, "ACT")
		b.text = params[4]
		b = ET.SubElement(a, "DATETIME")
		b.text = tdb + ' ' + tdc
		root.append(a)
		tree.write("history.xml")
