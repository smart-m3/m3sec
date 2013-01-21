#!/usr/bin/python

#foaf http://xmlns.com/foaf/spec/index.rdf
#scribo http://cs.karelia.ru#'''

from smart_m3.m3_kp import *

# connect to SS
node = KP("RedlandInsert")
ss_handle = ("X", (TCPConnector, ("127.0.0.1", 10010)))

if not node.join(ss_handle):
    sys.exit('Could not join to Smart Space')

# clean all data
rm = node.CreateRemoveTransaction(ss_handle)
rm.remove([Triple(URI('NotificationLJ'),None,None)])
rm.remove([Triple(None,None,None)])
node.CloseRemoveTransaction(rm)

# leave SS
node.leave(ss_handle)

