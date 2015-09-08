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

def run_command(command):
    sock = TSocket("localhost",port)
    sock.Send(command)
    message = TMessage()
    bytes_received = sock.Recv(message)
    if bytes_received <= 0:
        return None

    if message.What()==kMESS_STRING:
        arr = ctypes.create_string_buffer(256)
        message.ReadString(arr,256)
        return arr.value
    elif message.What()==kMESS_OBJECT:
        obj = message.ReadObject(message.GetClass())
        return obj
    else:
        return None

def Button1Pressed():
    run_command('TGRUTint::instance()->OpenFileDialog()')

def Button2Pressed():
    res = run_command('gResponse = new TObjString("my string here")')
    res.Print('')

def Button3Pressed():
    res = run_command('{TH1* hist = new TH1F("hist","hist",10,0,10); hist->Fill(5); gResponse = hist;}')
    res.Draw()

def Button4Pressed():
    run_command('cout << "hello from button 4!" << endl')

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
