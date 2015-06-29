/*
 * lib/genl/genl.c		Generic Netlink
 *
 *	This library is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU Lesser General Public
 *	License as published by the Free Software Foundation version 2.1
 *	of the License.
 *
 * Copyright (c) 2003-2008 Thomas Graf <tgraf@suug.ch>
 */

/**
 * @defgroup genl Generic Netlink
 *
 * @par Message Format
 * @code
 *  <------- NLMSG_ALIGN(hlen) ------> <---- NLMSG_ALIGN(len) --->
 * +----------------------------+- - -+- - - - - - - - - - -+- - -+
 * |           Header           | Pad |       Payload       | Pad |
 * |      struct nlmsghdr       |     |                     |     |
 * +----------------------------+- - -+- - - - - - - - - - -+- - -+
 * @endcode
 * @code
 *  <-------- GENL_HDRLEN -------> <--- hdrlen -->
 *                                 <------- genlmsg_len(ghdr) ------>
 * +------------------------+- - -+---------------+- - -+------------+
 * | Generic Netlink Header | Pad | Family Header | Pad | Attributes |
 * |    struct genlmsghdr   |     |               |     |            |
 * +------------------------+- - -+---------------+- - -+------------+
 * genlmsg_data(ghdr)--------------^                     ^
 * genlmsg_attrdata(ghdr, hdrlen)-------------------------
 * @endcode
 *
 * @par Example
 * @code
 *
 */

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/attr.h>
#include <netlink/socket.h>

#define VERSION 1

/* attributes (variables):
 * the index in this enum is used as a reference for the type,
 * userspace application has to indicate the corresponding type
 * the policy is used for security considerations 
 */
enum {
    DOC_EXMPL_A_UNSPEC,
    DOC_EXMPL_A_MSG,
    DOC_EXMPL_A_WAVE,
    __DOC_EXMPL_A_MAX,
};
#define DOC_EXMPL_A_MAX (__DOC_EXMPL_A_MAX - 1)
/* commands: enumeration of all commands (functions), 
 * used by userspace application to identify command to be executed
 */

enum {
    DOC_EXMPL_C_UNSPEC,
    DOC_EXMPL_C_ECHO,
    DOC_EXMPL_C_TEST,
    __DOC_EXMPL_C_MAX,
};

#define DOC_EXMPL_C_MAX (__DOC_EXMPL_C_MAX - 1)

static int parse_cb(struct nl_msg *msg, void *arg)
{
    struct nlmsghdr *nlh = nlmsg_hdr(msg);
    struct nlattr *attrs[DOC_EXMPL_A_MAX+1];
    struct nla_policy attr_policy[DOC_EXMPL_A_MAX + 1 ] = {
        [DOC_EXMPL_A_MSG] = {.type = NLA_STRING,.maxlen=200 
                            }
    };
    char * reply ;
    int err;


    // Validate message and parse attributes
    if ((err = genlmsg_parse(nlh, 0, attrs, DOC_EXMPL_A_MAX, attr_policy)) < 0){
	    fprintf(stderr, "Error in validating and parsing ");
     }

    if (attrs[DOC_EXMPL_A_MSG]) {
        reply = nla_get_string(attrs[DOC_EXMPL_A_MSG]);
        fprintf(stderr, "Reply from kernel = %s \n", reply);
        
    }
    return 0;
}


int main(){
    struct nl_sock *sock;
    struct nl_msg *msg;
    int family_id;

    // Allocate a new netlink socket
    sock = nl_socket_alloc();

    // Connect to generic netlink socket on kernel side
    genl_connect(sock);

    // Disable checking of sequence number as we are not following request
    // response model  
    nl_socket_disable_seq_check(sock);
    // Ask kernel to resolve family name to family id
    family_id = genl_ctrl_resolve(sock, "CONTROL_EXMPL");
    printf("Family ID = %d\n", family_id);

    // Construct a generic netlink by allocating a new message, fill in
    // the header and append a simple integer attribute.
    msg = nlmsg_alloc();
    genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family_id , 0, NLM_F_REQUEST, DOC_EXMPL_C_ECHO,VERSION);

    nla_put_string(msg, DOC_EXMPL_A_MSG ,"Hello world" );

    // Send message over netlink socket
    nl_send_auto_complete(sock, msg);

    // Free message
    nlmsg_free(msg);

    // Prepare socket to receive the answer by specifying the callback
    // function to be called for valid messages.
    nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, parse_cb, NULL);

    // Wait for the answer and receive it
    for (;;) {
	nl_recvmsgs_default(sock);
    }

}

