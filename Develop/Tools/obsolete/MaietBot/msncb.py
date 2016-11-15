

import string
import urllib
import socket

# md5 is deprecated in favour of hashlib since python 2.5; hashlib was
# introduced in python 2.5
try:
	from hashlib import md5
except:
	from md5 import md5

import msnlib

"""
This is the home for the msn callback class and examples (that might move to
another file in the near future).

There are three types of callbacks: the error one (this is only one), the
server ones (handle connections, notifications, lists and stuff like that),
and the switchboard ones (which handle messaging).

All of them receive as their first argument an 'md' (msn descriptor) that is
the main connection object; you probably already know what it is.

The models are:
error: 		def cb_err(md, errno, params)
server:		def cb_def(md, type, tid, params)
switchboard:	def cb_usr(md, type, tid, params, sbd)

See below for more examples.

Probably you should base your own callbacks on these ones, at least they were
thought with that in mind, so you can use yours as wrappers that handle only
your app-specific code and forget about the protocol-specific mess.

		Alberto (albertogli@telpin.com.ar)
"""


# use the debug function from msnlib
debug = msnlib.debug


class cb:
	def __init__(self):
		self.unk = cb_unk	# unknown
		self.err = cb_err	# server error
		self.msg = cb_msg	# get a message
		self.chl = cb_chl	# challenge
		self.qry = cb_ign	# query response
		self.iln = cb_iln	# status notification
		self.chg = cb_ign	# status change
		self.nln = cb_nln	# status notification
		self.fln = cb_fln	# status offline
		self.out = cb_out	# disconnect
		self.blp = cb_ign	# privacy mode change
		self.lst = cb_lst	# list requests
		self.bpr = cb_bpr	# user info
		self.gtc = cb_ign	# add notification
		self.syn = cb_syn	# list sync confirmation
		self.prp = cb_prp	# private info
		self.lsg = cb_lsg	# group list
		self.add = cb_add	# user add
		self.rem = cb_rem	# user remove
		self.adg = cb_adg	# group add
		self.rmg = cb_rmg	# group del
		self.reg = cb_reg	# group rename
		self.rea = cb_rea	# nick change
		self.rng = cb_rng	# switchboard invitation
		self.iro = cb_iro	# multi-user chat
		self.ans = cb_ans	# answer confirmation
		self.xfr = cb_xfr	# switchboard request
		self.usr = cb_usr	# sb request initial identification
		self.cal = cb_ign	# call confirmation
		self.joi = cb_joi	# session join
		self.ack = cb_ack	# message acknowledge
		self.nak = cb_nak	# message negative acknowledge
		self.bye = cb_bye	# switchboard user disconnect



error_table = {
	-10: 'Local error',
	200: 'Syntax error',
	201: 'Invalid parameter',
	205: 'Invalid user',
	206: 'Domain name missing',
	207: 'Already logged in',
	208: 'Invalid username',
	209: 'Invalid fusername',
	210: 'User list full',
	215: 'User already there',
	216: 'User already on list',
	217: 'User not online',
	218: 'Already in mode',
	219: 'User is in the opposite list',
	280: 'Switchboard failed',
	281: 'Transfer to switchboard failed',
	300: 'Required field missing',
	302: 'Not logged in',
	500: 'Internal server error',
	501: 'Database server error',
	510: 'File operation failed',
	520: 'Memory allocation failed',
	600: 'Server is busy',
	601: 'Server is unavaliable',
	602: 'Peer nameserver is down',
	603: 'Database connection failed',
	604: 'Server is going down',
	707: 'Could not create connection',
	711: 'Write is blocking',
	712: 'Session is overloaded',
	713: 'Too many active users',
	714: 'Too many sessions',
	715: 'Not expected',
	717: 'Bad friend file',
	911: 'Authentication failed',
	913: 'Not allowed when offline',
	920: 'Not accepting new users',
}

def cb_err(md, errno, params):
	"Handle server errors"
	if not error_table.has_key(errno):
		desc = 'Unknown error %d' % errno
	else:
		desc = error_table[errno]

	debug('SERVER ERROR %d: %s - %s' % (errno, desc, params))


def cb_def(md, type, tid, params):
	"Default callback. It just prints the args"
	debug('DEFAULT type: ' + type + ' :: Params: ' + str(params))


def cb_unk(md, type, tid, params):
	"Handles the unknown types"
	debug('Error! unknown event type "%s"' % type)
	debug('params: ' + str(params))


def cb_chl(md, type, tid, params):
	"Handles the challenges"
	if type != 'CHL': raise 'CallbackMess', (md, type, params)
	hash = params + 'VT6PX?UQTM4WM%YR' # magic from www.hypothetic.org
	hash = md5(hash).hexdigest()
	md._send('QRY', 'PROD0038W!61ZTF9 32')
	md.fd.send(hash)


def cb_ign(md, type, tid, params, nd = None):
	"Ignores"
	pass


def cb_out(md, type, tid, params):
	"Server disconnected us"
	debug('!!! Server closed the connection: ' + params)


def cb_iln(md, type, tid, params):
	"Handles a friend status change"
	t = params.split(' ')
	status = t[0]
	email = t[1]
	if len(params) > 2: nick = urllib.unquote(t[2])
	else: nick = ''

	md.users[email].status = status
	md.users[email].realnick = nick
	debug('FRIEND %s (%s) changed status to :%s:' % (nick, email, status))


def cb_fln(md, type, tid, params):
	"Handles a friend disconnection"
	email = tid
	debug('FRIEND %s disconnected (%s)' % (email, type))
	md.users[email].status = type


def cb_nln(md, type, tid, params):
	"Handles a friend status change"
	status = tid
	t = params.split(' ')
	email = t[0]
	if len(t) > 1: nick = urllib.unquote(t[1])
	else: nick = ''

	md.users[email].status = status
	md.users[email].realnick = nick
	debug('FRIEND %s (%s) changed status to :%s:' % (nick, email, status))


def cb_bpr(md, type, tid, params):
	"Update friend info"
	# the email is deduced from the last lst we got; if it's None it means
	# that we come from an add (the protocol behaves different if coming
	# from SYN or ADD)
	email = md._last_lst
	if email:
		# we come from SYN
		type = tid
		param = urllib.unquote(params)
	else:
		# we come from ADD
		t = params.split(' ')
		email = t[0]
		type = t[1]
		if len(t) >= 3:
			param = urllib.unquote(t[2])
		else:
			param = ''

	if not md.users.has_key(email): return

	if   type == 'PHH': md.users[email].homep = param
	elif type == 'PHW': md.users[email].workp = param
	elif type == 'PHM': md.users[email].mobilep = param
	else: pass


def cb_syn(md, type, tid, params):
	"Receive a SYN notification"
	t = params.split()
	if len(t) != 3:
		raise "SYNError"

	lver = int(t[0])
	total = int(t[1])
	ngroups = int(t[2])

	md.syn_lver = lver
	md.syn_total = total
	md.syn_ngroups = ngroups


def cb_lst(md, type, tid, params):
	p = params.split(' ')
	email = tid
	nick = urllib.unquote(p[0])
	listmask = int(p[1])
	if len(p) == 3:
		groups = p[2]
	else:
		groups = '0'

	# we only use one main group id
	gid = groups.split(',')[0]

	if email in md.users.keys():
		user = md.users[email]
	else:
		user = msnlib.user(email, nick, gid)

	# the list mask is a bitmask, composed of:
	# FL: 1
	# AL: 2
	# BL: 4
	# RL: 8

	# in forward
	if listmask & 1:
		user.lists.append('F')
		md.users[email] = user

	# in reverse
	if listmask & 8:
		user.lists.append('R')
		md.reverse[email] = user

	# in allow
	if listmask & 2:
		user.lists.append('A')

	# in block
	if listmask & 4:
		user.lists.append('B')

	md.lst_total += 1

	# save in the global last_lst the email, because BPRs might need it
	md._last_lst = email

def cb_lsg(md, type, tid, params):
	"Handles group list"
	p = params.split(' ')
	gid = tid
	name, unk = p[0:]
	# if we get the group 0, start from scratch
	if gid == '0':
		md.groups = {}
	name = urllib.unquote(name)
	md.groups[gid] = name


def cb_prp(md, type, tid, params):
	"Handles private info"
	t = params.split(' ')
	type = t[0]
	if len(t) > 1: param = urllib.unquote(t[1])
	else: param = ''

	if   type == 'PHH': md.homep = param
	elif type == 'PHW': md.workp = param
	elif type == 'PHM': md.mobilep = param
	else: pass


def cb_add(md, type, tid, params):
	"Handles a user add; both you adding a user and a user adding you"
	t = params.split(' ')
	type = t[0]
	if type == 'RL':
		email = t[2]
		nick = urllib.unquote(t[3])
		debug('ADD: %s (%s) added you' % (nick, email))
	elif type == 'FL':
		email = t[2]
		nick = urllib.unquote(t[3])
		gid = t[4]
		md.users[email] = msnlib.user(email, nick, gid)
		# put None in last_lst so BPRs know it's not coming from sync
		md._last_lst = None
		debug('ADD: adding %s (%s)' % (email, nick))
	else:
		pass

def cb_rem(md, type, tid, params):
	"""Handles a user del.
	Only make something in the case of a user removing you"""
	t = params.split(' ')
	type = t[0]
	if type == 'RL':
		email = t[2]
		debug('REM: %s removed you' % email)
	elif type == 'FL':
		email = t[2]
		if md.users[email].sbd:
			md.close(md.users[email].sbd)
		del(md.users[email])
		debug('REM: removing %s' % email)
	else:
		pass

def cb_adg(md, type, tid, params):
	"Handle a group add"
	t = params.split(' ')
	lver, name, gid = t[0:3]
	md.groups[gid] = name
	debug('ADG: group %s (%s) added' % (name, gid))

def cb_rmg(md, type, tid, params):
	"Handle a group del"
	t = params.split(' ')
	lver, gid = t[0:2]
	for e in md.users.keys():
		if md.users[e].gid == gid:
			if md.users[e].sbd:
				md.close(md.users[e].sbd)
			del(md.users[e])
	del(md.groups[gid])
	debug('RMG: group %s removed' % gid)

def cb_reg(md, type, tid, params):
	"Handle a group rename"
	t = params.split(' ')
	gid = t[1]
	name = t[2]
	md.groups[gid] = name
	debug('REG: group %s renamed to %s' % (name, gid))

def cb_rea(md, type, tid, params):
	"Handles our info change"
	t = params.split(' ')
	email = t[1]
	nick = urllib.unquote(t[2])
	if email != md.email:
		md.users[email].nick = nick
	else:
		md.nick = nick
	debug('NICK CHANGE: email %s - nick %s' % (email, nick))


def cb_rng(md, type, tid, params):
	"Handles switchboard invitations."
	t = params.split(' ')
	sid = tid
	ip, port = t[0].split(':')
	port = int(port)
	hash = t[2]
	email = t[3]

	fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# we set the socket nonblocking so we don't block (duh!) on connect();
	# it will be picked up later from the select loop and handled via the
	# main read() call, which you will have to see to find out the rest.
	fd.setblocking(0)
	fd.connect_ex((ip, port))

	sbd = msnlib.sbd()
	sbd.fd = fd
	sbd.block = 0
	sbd.state = 'cp'
	sbd.type = 'answer'
	sbd.endpoint = (ip, port)
	sbd.emails.append(email)
	sbd.hash = hash
	sbd.session_id = sid
	md.submit_sbd(sbd) 		# it has the connect pending


def cb_xfr(md, type, tid, params):
	"Handles switchboard requests"
	t = params.split(' ')
	ip, port = t[1].split(':')
	port = int(port)
	hash = t[3]

	fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	fd.setblocking(0)		# see cb_rng
	fd.connect_ex((ip, port))

	# look for the sbd, matching the tid
	sbd = None
	for i in md.sb_fds:
		if i.state == 'xf' and i.orig_tid == tid:
			sbd = i
			break
	if not sbd:
		debug('AIEEE: XFR without sbd!')
		raise 'XFRError', (type, tid, params)

	sbd.fd = fd
	sbd.block = 0
	sbd.state = 'cp'
	sbd.endpoint = (ip, port)
	sbd.hash = hash

def cb_iro(md, type, tid, params, sbd):
	"Handles the switchboard participant list"
	p = params.split(' ')
	uid, ucount, email, nick = p
	if ucount == '1':
		# do nothing if we only have one participant
		return
	else:
		if email not in md.users.keys():
			md.users[email] = msnlib.user(email)
		if email not in sbd.emails:
			sbd.emails.append(email)
		debug("FRIEND %s joined chat with %s" % (email, sbd.emails[0]))

def cb_usr(md, type, tid, params, sbd):
	"Handles switchboard requests initial identification"
	email = sbd.emails[0]
	md._send('CAL', email, sbd)
	sbd.state = 'ca'


def cb_joi(md, type, tid, params, sbd):
	"Handles a switchboard join, and sends the pending messages"
	email = tid
	# if it's a multi-user chat, just append it to the list
	if sbd.emails and email != sbd.emails[0]:
		sbd.emails.append(email)
		if email not in md.users.keys():
			md.users[email] = msnlib.user(email)
		debug('CALL: user %s joined chat with %s' % \
			(email, sbd.emails[0]))
	# otherwise (common path) set up the sbd and flush the messages
	else:
		sbd.state = 'es'
		debug('CALL: user %s replied your chat request; flushing' % email)
		md.sendmsg(email)
		debug('CALL: message queue for %s flushed' % email)


def cb_ans(md, type, tid, params, sbd):
	"""Answer confirmation to an invitation, replied after the connect()
	ending by read()"""
	sbd.state = 'es'


def cb_msg(md, type, tid, params, sbd):
	"Get a message"
	debug('MESSAGE\n+++ Header: %s\n%s\n\n' % (str(tid), str(params)))


def cb_ack(md, type, tid, params, sbd):
	"Get a message acknowledge"
	debug('ACK: tid:%s' % tid)


def cb_nak(md, type, tid, params, sbd):
	"Get a message negative acknowledge"
	debug('NAK: tid:%s' % tid)


def cb_bye(md, type, tid, params, sbd):
	"Handles a user sb disconnect"
	email = tid
	if email != sbd.emails[0]:
		debug('BYE: user %s leaving sbd' % email)
		if email in sbd.emails:
			sbd.emails.remove(email)
	else:
		debug('BYE: closing %s' % str(sbd))
		md.close(sbd)

