

import sys
import string
import socket
import urllib

"""
MSN Messenger Client Library
by Alberto Bertogli (albertogli@telpin.com.ar)
"""

# constants
VERSION = 0x0306
LOGIN_HOST = 'messenger.hotmail.com'
LOGIN_PORT = 1863

status_table = {
	'online':       'NLN',
	'away':         'AWY',
	'busy':         'BSY',
	'brb':          'BRB',
	'phone':        'PHN',
	'lunch':        'LUN',
	'invisible':    'HDN',
	'idle':         'IDL',
	'offline':      'FLN',
}

reverse_status = {
	'NLN':		'online',
	'AWY':		'away',
	'BSY':		'busy',
	'BRB':		'brb',
	'PHN':		'phone',
	'LUN':		'lunch',
	'HDN':		'invisible',
	'IDL':		'idle',
	'FLN':		'offline',
}


def debug(s):
	sys.stderr.write('\r' + str(s) + '\n')
	sys.stderr.flush()

def nickquote(nick):
	"""Quotes a nick the way the server likes it: replacing spaces with
	'%20' but leaving extender characters alone, as they get sent UTF-8
	encoded."""
	nick = nick.replace(' ', '%20')
	return nick

class user:
	"""User class, used to store your 'friends'"""
	def __init__(self, email = '', nick = '', gid = None):
		self.email = email
		self.nick = nick
		self.realnick = ''
		self.status = 'FLN'
		self.online = 0
		self.gid = gid
		self.homep = None
		self.workp = None
		self.mobilep = None
		self.sbd = None
		self.priv = {}
		self.lists = []

	def __repr__(self):
		return '<user email:%s nick:"%s" gid:%s>' % (self.email,
				self.nick, self.gid)


class sbd:
	"""SwitchBoard Descriptor
	Used as a pseudo-fd to store per-switchboard connection information.
	The state is either one of (too many):

	[answer]
	cp	connect pending (just came from rng)
	re	ready (just came from connect)
	an	waiting for answer reply

	[invite]
	xf	waiting for xfr response (not even connected yet)
	us	waiting for usr response
	ca	waiting for cal response
	jo	waiting for a join response

	es	established (waiting in boredom)

	You will find more information in the doc directory.
	"""

	def __init__(self):
		self.fd = None		# connection fd
		self.state = None	# connection's state (see doc above)
		self.emails = []	# emails we talk to through
		self.msgqueue = []	# outgoing message queue
		self.hash = None	# server-sent hash
		self.session_id = None	# server-sent sid
		self.endpoint = ()	# remote end (ip, port)
		self.type = None	# either 'answer' or 'invite'
		self.tid = 1		# the transaction id, it needs to be
					# unique for consistency
		self.block = 1		# blocking state
		self.orig_tid = None	# tid of the original XFR

	def __repr__(self):
		return '<sbd: emails:%s state:%s fd:%d endpoint:%s>' % \
			(str(self.emails), self.state, \
			self.fileno(), self.endpoint)

	def fileno(self):
		return self.fd.fileno()

	def get_tid(self):
		"Returns a valid tid as string"
		self.tid = self.tid + 1
		return str(self.tid - 1)



class msnd:
	"""MSN Descriptor
	This is the main and most important class; it represents a msn
	instance.

	It's, afaik, nonblocking (not through setblocking() but mainly because
	it forces a select() i/o model (which you would probably have used
	anyway, unless you think async/signal io worths the mess for a stupid
	messenger protocol, or you are a thread freak)), then the reads should
	always succed. Note that we sanely assume that writes do not block.

	Yes yes, you can use poll() too =)

	The only blocking call is the login() which is in charge of doing the
	initial connection and setup, all the rest are cpu bound.

	Once you have created an instance you should assign an email and a
	password at least, then do the login and i recommend you to call sync
	after that (and everyonce in a while doesn't hurt either). Finally you
	change your status and you're ready to idle.

	Oh, and don't forget to set the callbacks: they are the most important
	part, they are the ones which allow you to control the protocol and
	make this useful.

	They are completely asyncronous, are driven by the read method, and
	never block. A special care should be taken if you use threads (which
	you shouldn't need, that was the whole idea behind this), because
	there is not a single lock on these lines, and it will remain that way.

	There is an example (a very bad one, but you'll see how it would work)
	that should have come with this file; also the callback file has good
	working code.
	"""

	def __init__(self):
		self.fd = None			# socket fd
		self.sb_fds = []		# switchboard fds
		self.tid = 1			# transaction id

		self.email = None		# login email
		self.pwd = None			# login pwd
		self.nick = None		# nick

		self.homep = None		# home phone
		self.workp = None		# work phone
		self.mobilep = None		# mobile phone

		self.status = 'FLN'		# status
		self.encoding = 'iso-8859-1'	# local encoding

		self.lhost = LOGIN_HOST
		self.lport = LOGIN_PORT
		self.ns = (None, None)		# notification server
		self.hash = None		# hash used to authenticate

		self.syn_lver = 0		# user list version
		self.syn_total = 10000		# qty. of users from SYN
		self.syn_ngroups = 0		# qty. of groups from SYN

		self.lst_total = 0		# qty. of LSTs got

		self.cb = None			# callbacks

		self.users = {}			# forward user list
		self.reverse = {}		# reverse user list
		self.groups = {}		# group list


	def __repr__(self):
		return '<msnd object, fd:%s, email:%s, tid:%s>' % (self.fd,
			self.email, self.tid)

	def fileno(self):
		"Useful for select()"
		return self.fd.fileno()

	def encode(self, s):
		"Encodes a string from local encoding to utf8"
		try:
			return s.decode(self.encoding).encode('utf-8')
		except:
			return s

	def decode(self, s):
		"Decodes a string from utf8 to local encoding"
		try:
			return s.decode('utf-8').encode(self.encoding)
		except:
			return s


	def pollable(self):
		"""Return a pair of lists of poll()/select()ables network
		descriptors (ie. they are not fds, but actually classes that
		implement fileno() methods, like this one and the sbd). We do
		it this way because then it's simpler to read().

		The reason behind the tuple is that for connect-pending fds we
		need to wait for writing readiness, so we must tell the
		userspace so. Notice that it still goes with the read() path.

		Yes, it is a mess but i couldn't find anything better yet. It
		works, it's efficient; let's pretend it's correct =)

		It includes the main file descriptor, and all the switchboards
		connections; then you call self.read(fd) on what this returns,
		and magic happens."""

		iwtd = []
		owtd = []
		iwtd.append(self)
		for nd in self.sb_fds:
			if nd.state == 'cp':	# connect is pending
				owtd.append(nd)
			elif nd.state == 'xf':	# skip this case because it's
						# not connected yet
				pass
			else:			# readable!
				iwtd.append(nd)
		return (iwtd, owtd)


	def get_tid(self):
		"Returns a valid tid as string"
		self.tid = self.tid + 1
		return str(self.tid - 1)


	def _send(self, cmd, params = '', nd = None, raw = 0):
		"""Sends a command to the server, building it first as a
		string; uses, if specified, the pseudo fd (it can be either
		msnd or sbd)."""
		if not nd:
			nd = self
		tid = nd.get_tid()
		fd = nd.fd
		c = cmd + ' ' + tid
		if params: c = c + ' ' + params
		debug(str(fd.fileno()) + ' >>> ' + c)
		if not raw:
			c = c + '\r\n'
		c = self.encode(c)
		return fd.send(c)


	def _recv(self, fd = None):
		"Reads a command from the server, returns (cmd, tid, params)"
		if not fd:
			fd = self.fd
		# cheap and dirty readline, FIXME
		buf = ''
		c = fd.recv(1)
		while c != '\n' and c != '':
			buf = buf + c
			c = fd.recv(1)

		if c == '':
			raise 'SocketError'

		buf = buf.strip()
		pbuf = buf.split(' ')

		cmd = pbuf[0]

		# it's possible that we don't have any params (errors being
		# the most common) so we cover our backs
		if len(pbuf) >= 3:
			tid = pbuf[1]
			params = self.decode(string.join(pbuf[2:]))
		elif len(pbuf) == 2:
			tid = pbuf[1]
			params = ''
		else:
			tid = '0'
			params = ''

		debug(str(fd.fileno()) + ' <<< ' + buf)
		return (cmd, tid, params)


	def _recvmsg(self, msglen, fd = None):
		"Read a message from the server, returns it"
		if not fd:
			fd = self.fd
		left = msglen
		buf = ''
		while len(buf) != msglen:
			c = fd.recv(left)
			#debug(str(fd.fileno()) + ' <<< ' + buf)
			buf = buf + c
			left = left - len(c)

		return self.decode(buf)


	def submit_sbd(self, sbd):
		"""Submits a switchboard descriptor to add to our list; it is
		also put on our global list.

		Note that if there is no such user, we create it in order to
		be able to do operations on users that are not in our server
		list."""

		self.sb_fds.append(sbd)
		email = sbd.emails[0]
		if email not in self.users.keys():
			self.users[email] = user(email)
		if self.users[email].sbd and self.users[email].sbd != sbd:
			# override the sbd, but keep the message queue
			sbd.msgqueue = self.users[email].sbd.msgqueue[:]
			self.close(self.users[email].sbd)
		self.users[email].sbd = sbd
		return


	def change_status(self, st):
		"""Changes the current status to: online, away, busy, brb,
		phone, lunch, invisible, idle, offline"""
		if not status_table.has_key(st): return 0
		self.status = status_table[st]
		self._send('CHG', self.status)
		return 1


	def privacy(self, public = 1, auth = 0):
		"""Sets our privacy state. First parameter define if you get
		messages from everybody or only from people on your list; the
		second defines if you want users to ask for authorization or
		let everybody add you"""
		if public:	self._send('BLP', 'AL') # be social
		else:		self._send('BLP', 'BL') # live in a cave

		if auth:	self._send('GTC', 'A')	# ask for auth
		else:		self._send('GTC', 'N')	# let them add you

		return 1


	def change_nick(self, nick):
		"Changes our nick"
		nick = nickquote(nick)
		self._send('REA', self.email + ' ' + nick)
		return 1


	def sync(self):
		"Syncronizes the tables"
		self._send('SYN', '0')
		return 1


	def useradd(self, email, nick = None, gid = '0'):
		"Adds a user"
		if not nick: nick = email
		nick = nickquote(nick)
		self._send('ADD', 'AL ' + email + ' ' + nick)
		self._send('ADD', 'FL ' + email + ' ' + nick + ' ' + gid)
		return 1


	def userdel(self, email):
		"Removes a user"
		self._send('REM', 'AL ' + email)
		self._send('REM', 'FL ' + email)
		return 1

	def userren(self, email, newnick):
		"Renames a user"
		newnick = nickquote(newnick)
		self._send('REA', email + ' ' + newnick)
		return 1

	def userblock(self, email):
		self._send('REM', 'AL ' + email)
		self._send('ADD', 'BL ' + email + ' ' + email)
		if 'B' not in self.users[email].lists:
			self.users[email].lists.append('B')

	def userunblock(self, email):
		self._send('REM', 'BL ' + email)
		self._send('ADD', 'AL ' + email + ' ' + email)
		if 'B' in self.users[email].lists:
			self.users[email].lists.remove('B')

	def groupadd(self, name):
		"Adds a group"
		name = nickquote(name)
		self._send('ADG', name + ' 0')
		return 1

	def groupdel(self, gid):
		"Removes a group"
		self._send('RMG', gid)
		return 1

	def groupren(self, gid, newname):
		newname = nickquote(newname)
		self._send('REG', gid + ' ' + newname)
		return 1

	def disconnect(self):
		"Disconnect from the server"
		self.fd.send('OUT\r\n')
		self.fd.close()


	def close(self, sb):
		"Closes a given sbd"
		self.sb_fds.remove(sb)
		self.users[sb.emails[0]].sbd = None
		try:
			self._send('BYE', self.email, nd = sb)
			sb.fd.close()
		except:
			pass
		del(sb)


	def invite(self, email, sbd):
		"Invites a user into an existing sbd"
		self._send('CAL', email, nd = sbd)

	def login(self):
		"Logins to the server, really boring"

		# open socket
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.fd.connect((self.lhost, self.lport))

		# version information
		self._send('VER', 'MSNP8 CVR0')

		r = self._recv()
		if r[0] != 'VER' and r[2][0:4] != 'MSNP8':
			raise 'VersionError', r

		# lie the version, just in case
		self._send('CVR', '0x0409 win 4.10 i386 MSNMSGR 5.0.0544 MSMSGS ' + self.email)
		self._recv()	# we just don't care what we get

		# ask for notification server
		self._send('USR', 'TWN I ' + self.email)

		r = self._recv()
		if r[0] != 'XFR' and r[2][0:2] != 'NS':
			raise 'NSError', r

		# parse the notification server ip and port (as int)
		ns = string.split(r[2])[1]
		self.ns = ns.split(':')
		self.ns[1] = int(self.ns[1])
		self.ns = tuple(self.ns)

		# close the fd and reopen it on the ns
		self.fd.close()
		self.fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.fd.connect(self.ns)

		# version, same as before
		self._send('VER', 'MSNP8 CVR0')
		r = self._recv()
		if r[0] != 'VER' and r[2][0:4] != 'MSNP8':
			raise 'VersionError', r

		# lie the version, just in case
		self._send('CVR', '0x0409 win 4.10 i386 MSNMSGR 5.0.0544 MSMSGS	' + self.email)
		self._recv()	# we just don't care what we get

		# auth: send user, get hash
		self._send('USR', 'TWN I ' + self.email)

		r = self._recv()
		if r[0] != 'USR':
			raise 'AuthError', r
		hash = string.split(r[2])[2]

		# get and use the passport id
		passportid = self.passport_auth(hash)
		self._send('USR', 'TWN S ' + passportid)

		r = self._recv()
		if r[0] != 'USR' and r[2][0:2] != 'OK':
			raise 'AuthError', r
		self.nick = string.split(r[2])[2]
		self.nick = urllib.unquote(self.nick)

		return 1


	def passport_auth(self, hash):
		"""Logins into passport and obtains an ID used for
		authorization; it's a helper function for login"""
		import urllib
		import httplib

		# initial connection
		debug('PASSPORT begin')
		nexus = urllib.urlopen('https://nexus.passport.com/rdr/pprdr.asp')
		h = nexus.headers
		purl = h['PassportURLs']

		# parse the info
		d = {}
		for i in purl.split(','):
		        key, val = i.split('=', 1)
			d[key] = val

		# get the login server
		login_server = 'https://' + d['DALogin']
		login_host = d['DALogin'].split('/')[0]

		# build the authentication headers
		ahead =  'Passport1.4 OrgVerb=GET'
		ahead += ',OrgURL=http%3A%2F%2Fmessenger%2Emsn%2Ecom'
		ahead += ',sign-in=' + urllib.quote(self.email)
		ahead += ',pwd=' + urllib.quote(self.pwd)
		ahead += ',lc=1033,id=507,tw=40,fs=1,'
		ahead += 'ru=http%3A%2F%2Fmessenger%2Emsn%2Ecom,ct=0,'
		ahead += 'kpp=1,kv=5,ver=2.1.0173.1,tpf=' + hash
		headers = { 'Authorization': ahead }

		# connect to the given server
		debug('SSL Connect to %s' % login_server)
		ls = httplib.HTTPSConnection(login_host)

		# make the request
		debug('SSL GET')
		ls.request('GET', login_server, '', headers)
		resp = ls.getresponse()

		# loop if we get redirects until we get a definitive answer
		debug('SSL Response %d' % resp.status)
		while resp.status == 302:
			login_server = resp.getheader('Location')
			login_host = login_server.split('/')[2]
			debug('SSL Redirect to %s' % login_server)
			ls = httplib.HTTPSConnection(login_host)
			headers = { 'Authorization': ahead }
			ls.request('GET', login_server, '', headers)
			resp = ls.getresponse()
			debug('SSL Response %d' % resp.status)

		# now we have a definitive answer, if it's not 200 (success)
		# just raise AuthError
		if resp.status != 200:
			# for now we raise 911, which means authentication
			# failed; but maybe we can get more detailed
			# information
			raise 'AuthError', [911, 'SSL Auth failed']

		# and parse the headers to get the passport id
		try:
			ainfo = resp.getheader('Authentication-Info')
		except:
			ainfo = resp.getheader('WWW-Authenticate')

		d = {}
		for i in ainfo.split(','):
			key, val = i.split('=', 1)
			d[key] = val

		passportid = d['from-PP']
		passportid = passportid[1:-1]		# remove the "'"
		return passportid


	def read(self, nd = None):
		"""Reads from the specified nd and run the callback. The nd
		can be either a msnd or a sbd (that's why it's called 'nd'
		from 'network descriptor').
		"""
		if not nd:
			nd = self

		# handle different stages of switchboard initialization
		if nd in self.sb_fds:
			# connect pending
			if nd.state == 'cp':
				# see if the connect went well
				r = nd.fd.getsockopt(socket.SOL_SOCKET,
					socket.SO_ERROR)
				if r != 0:
					raise 'SocketError', 'ConnectFailed'
				nd.fd.setblocking(1)
				nd.block = 1
				nd.state = 're'

			# need to send the answer to the remote invitation
			if nd.type == 'answer' and nd.state == 're':
				params = self.email + ' ' + nd.hash + ' ' + \
					nd.session_id
				self._send('ANS', params, nd)
				nd.state = 'an'
				return
			if nd.type == 'invite' and nd.state == 're':
				params = self.email + ' ' + nd.hash
				self._send('USR', params, nd)
				nd.state = 'us'
				return



		r = self._recv(nd.fd)
		type = r[0]
		tid = r[1]
		params = string.strip(r[2])

		if   type == 'CHL': self.cb.chl(self, type, tid, params)
		elif type == 'QRY': self.cb.qry(self, type, tid, params)
		elif type == 'ILN': self.cb.iln(self, type, tid, params)
		elif type == 'CHG': self.cb.chg(self, type, tid, params)
		elif type == 'OUT': self.cb.out(self, type, tid, params)
		elif type == 'FLN': self.cb.fln(self, type, tid, params)
		elif type == 'NLN': self.cb.nln(self, type, tid, params)
		elif type == 'BLP': self.cb.blp(self, type, tid, params)
		elif type == 'LST': self.cb.lst(self, type, tid, params)
		elif type == 'GTC': self.cb.gtc(self, type, tid, params)
		elif type == 'SYN': self.cb.syn(self, type, tid, params)
		elif type == 'PRP': self.cb.prp(self, type, tid, params)
		elif type == 'LSG': self.cb.lsg(self, type, tid, params)
		elif type == 'BPR': self.cb.bpr(self, type, tid, params)
		elif type == 'ADD': self.cb.add(self, type, tid, params)
		elif type == 'REA': self.cb.rea(self, type, tid, params)
		elif type == 'REM': self.cb.rem(self, type, tid, params)
		elif type == 'ADG': self.cb.adg(self, type, tid, params)
		elif type == 'RMG': self.cb.rmg(self, type, tid, params)
		elif type == 'REG': self.cb.reg(self, type, tid, params)
		elif type == 'RNG': self.cb.rng(self, type, tid, params)

		elif type == 'IRO': self.cb.iro(self, type, tid, params, nd)
		elif type == 'ANS': self.cb.ans(self, type, tid, params, nd)
		elif type == 'XFR': self.cb.xfr(self, type, tid, params)
		elif type == 'USR': self.cb.usr(self, type, tid, params, nd)
		elif type == 'CAL': self.cb.cal(self, type, tid, params, nd)
		elif type == 'JOI': self.cb.joi(self, type, tid, params, nd)

		elif type == 'ACK': self.cb.ack(self, type, tid, params, nd)
		elif type == 'NAK': self.cb.nak(self, type, tid, params, nd)
		elif type == 'BYE': self.cb.bye(self, type, tid, params, nd)


		elif type == 'MSG':
			params = tid + ' ' + params
			mlen = int(r[2].split()[-1])
			msg = self._recvmsg(mlen, nd.fd)
			self.cb.msg(self, type, params, msg, nd)

		else:
			# catch server errors - always numeric type
			try:
				errno = int(type)
			except:
				errno = None

			if errno:
				self.cb.err(self, errno, \
					str(tid) + ' ' + str(params))
			else:
				# if we got this far, we have no idea
				self.cb.unk(self, type, tid, params)
		return


	def sendmsg(self, email, msg = '', sb = None):
		"""Sends a message to the user identified by 'email', either
		the one specified or flush the queue.
		Returns:
		1	message queued for delivery
		2	queue flushed
		-2	the message is too big

		To verify the message delivery, use the ack callbacks.
		Message sending order is guaranteed within a sbd; but not the
		acknowledge; that's what the ACK/NAK callbacks are for.
		"""

		if email and email not in self.users.keys():
			self.users[email] = user(email)

		if len(msg) > 1500:
			return -2

		if not sb:
			sb = self.users[email].sbd

		# we don't have a connection
		if not sb:
			sb = sbd()
			sb.state = 'xf'
			sb.type = 'invite'
			sb.emails.append(email)
			sb.msgqueue.append(msg)

			self.submit_sbd(sb)	# no need to connect it yet
			# we set the orig_tid of the sbd to the next tid (that
			# is, the tid the XFR is going to have), in order to
			# be able to identify it later, in cb.cb_xfr()
			sb.orig_tid = str(self.tid)
			self._send('XFR', 'SB')
			return 1

		# it's not ready yet
		elif sb.state != 'es':
			sb.msgqueue.append(msg)
			return 1

		# no more excuses, send it
		else:
			# we make a list with all the messages to send
			pend = sb.msgqueue
			if msg:
				pend.append(msg)
			while len(pend):
				m = pend[0]
				header = "MIME-Version: 1.0\r\n" + \
					"Content-Type: text/plain; " + \
					"charset=UTF-8\r\n\r\n"
				m = header + m
				msize = len(self.encode(m))
				params = 'A ' + str(msize) + '\r\n' + m
				self._send('MSG', params, sb, raw = 1)
				del(pend[0])

			return 2



