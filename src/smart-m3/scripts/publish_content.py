#!/usr/bin/python

#foaf http://xmlns.com/foaf/spec/index.rdf

from smart_m3.m3_kp import *

# connect to SS
node = KP("RedlandInsert")
ss_handle = ("X", (TCPConnector, ("127.0.0.1", 10010)))

if not node.join(ss_handle):
    sys.exit('Could not join to Smart Space')


# insert test data
pro = node.CreateInsertTransaction(ss_handle)

init_triples = [Triple(URI("Kirill"),URI("is a"),URI("homo")),
                Triple(URI("Kirill"),URI("has a"), URI("sister")),
                Triple(URI("Katya"),URI("is a sister of"),URI("Kirill")),
                Triple(URI("Kirill"),URI("foaf_name"),Literal("Kirill Yudenok")),
                Triple(URI("Kirill"),URI("foaf_age"),Literal("25")),
                Triple(URI("Kirill"),URI("is a brother of"),URI("Katya"))]

# send profile and account information
pro.send(init_triples)
node.CloseInsertTransaction(pro)

node.leave(ss_handle)

