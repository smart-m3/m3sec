#!/usr/bin/python

#foaf http://xmlns.com/foaf/spec/index.rdf

from smart_m3.m3_kp import *

node = KP("RedlandInsert")
#ss_handle = ("X", (TCPConnector, ("192.168.112.104", 10010)))
ss_handle = ("X", (TCPConnector, ("127.0.0.1", 10010)))

if not node.join(ss_handle):
    sys.exit('Could not join to Smart Space')


# clean all data
#rm = node.CreateRemoveTransaction(ss_handle)
#rm.remove([Triple(URI('NotificationLJ'),None,None)])
#rm.remove([Triple(None,None,None)])
#node.CloseRemoveTransaction(rm)


# insert test data
pro = node.CreateInsertTransaction(ss_handle)

init_triples = [Triple(URI("profile-3a7a8815"),URI("rdf:type"),URI("scribo_Profile")),
                Triple(URI("profile-3a7a8815"),URI("scribo_personInformation"), URI("person-9d166448")),
                Triple(URI("person-9d166448"),URI("rdf:type"),URI("foaf_Person")),
                Triple(URI("person-9d166448"),URI("foaf_name"),Literal("John Connor")),
                Triple(URI("person-9d166448"),URI("foaf_age"),Literal("25")),
                Triple(URI("person-9d166448"),URI("foaf_account"),URI("account-4d45454")),
                Triple(URI("account-4d45454"),URI("rdf:type"),URI("foaf_OnlineAccount")),
                Triple(URI("account-4d45454"),URI("scribo_login"),Literal("scribo-rpc")),
                Triple(URI("account-4d45454"),URI("scribo_password"),Literal("test123")),
                Triple(URI("account-4d45454"),URI("scribo_hasPost"),URI("post-133rf3t")),
                Triple(URI("post-133rf3t"),URI("rdf:type"),URI("scribo_Post")),
                Triple(URI("post-133rf3t"),URI("scribo_title"),Literal("title example")),
                Triple(URI("post-133rf3t"),URI("scribo_text"),Literal("here is a text")),
                Triple(URI("post-133rf3t"),URI("scribo_hasComment"),URI("comment-454g4gd")),
                Triple(URI("comment-454g4gd"),URI("rdf:type"),URI("scribo_Comment")),
                Triple(URI("comment-454g4gd"),URI("scribo_title"),Literal("title comment example")),
                Triple(URI("comment-454g4gd"),URI("scribo_text"),Literal("here is a text of comment"))]

# send profile and account information
pro.send(init_triples)
node.CloseInsertTransaction(pro)

# SENDING NOTIFICATIONS

'''
# send refreshAccount notification
pro = node.CreateInsertTransaction(ss_handle)
notif = [Triple(URI("NotificationLJ"),URI("refreshAccount"),URI("account-4d45454"))]
pro.send(notif)
node.CloseInsertTransaction(pro)
'''


# send refreshPosts notification
#pro = node.CreateInsertTransaction(ss_handle)
#notif = [Triple(URI("NotificationLJ"),URI("refreshPosts"),URI("account-4d45454"))]
#pro.send(notif)
#node.CloseInsertTransaction(pro)


'''
# send sendPost notification
pro = node.CreateInsertTransaction(ss_handle)

post = [Triple(URI("post-fg4tt46"),URI("scribo_text"),Literal("new sended post text")),
        Triple(URI("post-fg4tt46"),URI("scribo_title"),Literal("new sended post title"))]

notif = [Triple(URI("post_notif-676ger34"),URI("postAcc"),URI("account-4d45454")),
         Triple(URI("post_notif-676ger34"),URI("postId"),URI("post-fg4tt46")),
         Triple(URI("NotificationLJ"),URI("sendPost"),URI("post_notif-676ger34"))]

pro.send(post)
pro.send(notif)
node.CloseInsertTransaction(pro)
'''

'''
# send editPost notification
pro = node.CreateInsertTransaction(ss_handle)
new_post = [Triple(URI("post-144rf3t"),URI("scribo_text"),Literal('edited text')),
            Triple(URI("post-144rf3t"),URI("scribo_title"),Literal('edited title'))]

notif = [Triple(URI("notif-4d45ss54"),URI("oldPost"),URI("post-133rf3t")),
         Triple(URI("notif-4d45ss54"),URI("newPost"),URI("post-144rf3t")),
         Triple(URI("NotificationLJ"),URI("editPost"),URI("notif-4d45ss54"))]

pro.send(new_post)
pro.send(notif)
node.CloseInsertTransaction(pro)
'''

'''
# send delPost notification

pro = node.CreateInsertTransaction(ss_handle)

notif = [Triple(URI("notif-fggr454ff"),URI("postId"),URI("post-133rf3t")),
         Triple(URI("notif-fggr454ff"),URI("postAcc"),URI("account-4d45454")),
         Triple(URI("NotificationLJ"),URI("delPost"),URI("notif-fggr454ff"))]

pro.send(notif)
node.CloseInsertTransaction(pro)
'''

node.leave(ss_handle)

