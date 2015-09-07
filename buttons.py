#!/usr/bin/python

from Tkinter import *
from ROOT    import *

def Button1Pressed():
	sock = TSocket("localhost",9090)
        sock.Send("cout << \"hello from button 1!\" << endl")
	print 'button 1 pressed'

def Button2Pressed():
	sock = TSocket("localhost",9090)
        sock.Send("cout << \"hello from button 2!\" << endl")
	print 'button 2 pressed'

def Button3Pressed():
	sock = TSocket("localhost",9090)
        sock.Send("cout << \"hello from button 3!\" << endl")
	print 'button 3 pressed'

def Button4Pressed():
	sock = TSocket("localhost",9090)
        sock.Send("cout << \"hello from button 4!\" << endl")
	print 'button 4 pressed'

def Button5Pressed():
	sock = TSocket("localhost",9090)
        sock.Send("cout << \"hello from button 5!\" << endl")
	print 'button 5 pressed'

def Button6Pressed():
	sock = TSocket("localhost",9090)
        sock.Send("cout << \"hello from button 6!\" << endl")
	print 'button 6 pressed'

mainwindow = Tk()
mainwindow.wm_title("grut command")

title = Label(mainwindow,text='Command Window',
                         fg="black",bg="blue",
                         font="Helvetica 24 bold")

button1 = Button(mainwindow,text='Button 1',fg="black",bg="goldenrod",command=Button1Pressed)
button2 = Button(mainwindow,text='Button 2',fg="black",bg="goldenrod",command=Button2Pressed)
button3 = Button(mainwindow,text='Button 3',fg="black",bg="goldenrod",command=Button3Pressed)
button4 = Button(mainwindow,text='Button 4',fg="black",bg="goldenrod",command=Button4Pressed)
button5 = Button(mainwindow,text='Button 5',fg="black",bg="goldenrod",command=Button5Pressed)
button6 = Button(mainwindow,text='Button 6',fg="black",bg="goldenrod",command=Button6Pressed)

title.pack(fill=X)
button1.pack(fill=BOTH,expand=1)
button2.pack(fill=BOTH,expand=1)
button3.pack(fill=BOTH,expand=1)
button4.pack(fill=BOTH,expand=1)
button5.pack(fill=BOTH,expand=1)
button6.pack(fill=BOTH,expand=1)

mainwindow.mainloop()

print "hello!"

