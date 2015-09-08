#!/usr/bin/python

from Tkinter import *
import ttk
import ctypes
import sys

from ROOT import *

if len(sys.argv)>1:
        port = int(sys.argv[1])
else:
        port = 9090

def Button1Pressed():
	sock = TSocket("localhost",port)
        sock.Send("TGRUTint::instance()->OpenFileDialog()")
        message = TMessage()
        bytes_received = sock.Recv(message)
        if bytes_received > 0:
                if message.What()==kMESS_STRING:
                        arr = ctypes.create_string_buffer(256)
                        message.ReadString(arr,256)
                        print arr.value
                elif message.What()==kMESS_OBJECT:
                        pass

def Button2Pressed():
	sock = TSocket("localhost",port)
        sock.Send("cout << \"hello from button 2!\" << endl")
	print 'button 2 pressed'

def Button3Pressed():
	sock = TSocket("localhost",port)
        sock.Send("cout << \"hello from button 3!\" << endl")
	print 'button 3 pressed'

def Button4Pressed():
	sock = TSocket("localhost",port)
        sock.Send("cout << \"hello from button 4!\" << endl")
	print 'button 4 pressed'

button6_visible = True
def Button5Pressed():
        global button6_visible
	if button6_visible:
                button6.pack_forget()
        else:
                button6.pack(fill=BOTH,expand=1)
        button6_visible = not button6_visible

def Button6Pressed():
        import IPython; IPython.embed()

mainwindow = Tk()
mainwindow.wm_title("grut command")

title = Label(mainwindow,text='Command Window',
                          fg="black", bg="blue",
                         font="Helvetica 24 bold")

button1 = Button(mainwindow,text='Load File',fg="black",bg="goldenrod",command=Button1Pressed)
button2 = Button(mainwindow,text='Button 2',fg="black",bg="goldenrod",command=Button2Pressed)
button3 = Button(mainwindow,text='Button 3',fg="black",bg="goldenrod",command=Button3Pressed)
button4 = Button(mainwindow,text='Button 4',fg="black",bg="goldenrod",command=Button4Pressed)
button5 = Button(mainwindow,text='Button 5',fg="black",bg="goldenrod",command=Button5Pressed)
button6 = Button(mainwindow,text='Button 6',fg="black",bg="goldenrod",command=Button6Pressed)

tree = ttk.Treeview(mainwindow)
tree.insert('','end','widgets', text="Some text here")
tree.insert('',0,'gallery',text="Some other text")
tree_id = tree.insert('','end',text="Event more text")
tree.insert('widgets','end',text="canvas")
tree.insert(tree_id,'end', text="Tree")


title.pack(fill=X)
button1.pack(fill=BOTH,expand=1)
button2.pack(fill=BOTH,expand=1)
button3.pack(fill=BOTH,expand=1)
button4.pack(fill=BOTH,expand=1)
button5.pack(fill=BOTH,expand=1)
button6.pack(fill=BOTH,expand=1)
tree.pack(fill=BOTH,expand=1)

mainwindow.mainloop()

print "hello!"

