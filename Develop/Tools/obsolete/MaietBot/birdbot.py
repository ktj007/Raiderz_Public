#!/usr/bin/env python
# *-* coding: utf-8 -*-

"""
This is a very simple bot to show how automation using msnlib could be done.
It's not quite useful as-is, but provides a good example.

If you play with it, please let me know.
"""


# sys, for getting the parameters
import sys

import os

import subprocess

# time, for sleeping
import time

# select to wait for events
import select

# socket, to catch errors
import socket

import string

# thread, for creating the worker thread
import thread

# and, of course, msnlib
import msnlib
import msncb

os.environ["LANG"] = "ko_KR.utf-8"

def null(s):
  "Null function, useful to void debug ones"
  pass


m = msnlib.msnd()
m.cb = msncb.cb()
m.encoding="UTF-8"

# message
def cb_msg(md, type, tid, params, sbd):
  t = string.split(tid)
  email = t[0]
  
  # messages from hotmail are only when we connect, and send things
  # regarding, aparently, hotmail issues. we ignore them (basically
  # because i couldn't care less; however if somebody has intrest in
  # these and provides some debug output i'll be happy to implement
  # parsing).
  if email == 'Hotmail':
    return
    
  # parse
  lines = string.split(params, '\n')
  headers = {}
  eoh = 1
  for i in lines:
    # end of headers
    if i == '\r':
      break
    tv = string.split(i, ':')
    type = tv[0]
    value = string.join(tv[1:], ':')
    value = string.strip(value)
    headers[type] = value
    
#    print 'type = %s' % type
    eoh += 1
  
#  if headers.has_key('Content-Type') and headers['Content-Type'] == 'text/x-msmsgscontrol':
    # the typing notices
#    print 'TYPING %s\n' % email
#  else:
    # messages
#    print 'MSG %d %d %s\n%s\n' % \
#      (len(lines), eoh, email, string.join(lines, '\n'))

  if lines[4] == 'build' or lines[4] == '빌드' or lines[4] == '빌드해':
    m.change_status("busy")
      
    buf = '알았어 오빠~~'    
    m.sendmsg(email,  buf)

    p = subprocess.Popen([r"E:/ForAutoCommit/Develop/!buildall_and_commit.bat"], shell=True,cwd="E:/ForAutoCommit/Develop",stdout=subprocess.PIPE)
    output = p.stdout.read()
    print output
    p.wait()

    buf = '끝났어 오빠..~'
    m.sendmsg(email,  buf)
      
    m.change_status("away")

  msncb.cb_msg(md, type, tid, params, sbd)
m.cb.msg = cb_msg

def do_work():
  """
  Here you do your stuff and send messages using m.sendmsg()
  This is the only place your code lives
  """
  
  # wait a bit for everything to settle down (sync taking efect
  # basically)
  time.sleep(15)
  
  print '-' * 20 + 'SEND 1'
  print m.sendmsg("zelinde@dreamwiz.com", "Message One")

  print '-' * 20 + 'SEND 2'
  print m.sendmsg("zelinde@dreamwiz.com", "two")

  # give time to send the messages
  time.sleep(5)

  # and then quit
  #quit()
  

# you shouldn't need to touch anything past here


# get the login email and password from the parameters
try:
  m.email = sys.argv[1]
  m.pwd = sys.argv[2]
except:
  print "Use: msnbot email password"
  sys.exit(1)

msnlib.debug = null
msncb.debug = null

print "Logging In"
m.login()

print "Sync"
# this makes the server send you the contact list, and it's recommended that
# you do it because you can get in trouble when getting certain events from
# people that are not on your list; and it's not that expensive anyway
m.sync()

print "Changing Status"
# any non-offline status will do, otherwise we'll get an error from msn when
# sending a message
m.change_status("away")
m.change_nick("보순이에용.. 빌드는 나에게 맡기셔요..ㅎㅎ")

def quit():
  try:
    m.disconnect()
  except:
    pass
  print "Exit"
  sys.exit(0)

# we start a thread to do the work. it's a thread because we want to share
# everything, and fork cow semantics cause problems here
#thread.start_new_thread(do_work, ())

# we loop over the network socket to get events
print "Loop"
while 1:
  # we get pollable fds
  t = m.pollable()
  infd = t[0]
  outfd = t[1]

  # we select, waiting for events
  try:
    fds = select.select(infd, outfd, [], 0)
  except:
    quit()
  
  for i in fds[0] + fds[1]:       # see msnlib.msnd.pollable.__doc__
    try:
      m.read(i)
    except ('SocketError', socket.error), err:
      if i != m:
        # user closed a connection
        # note that messages can be lost here
        m.close(i)
      else:
        # main socket closed
        quit()

  # sleep a bit so we don't take over the cpu
  time.sleep(0.1)



