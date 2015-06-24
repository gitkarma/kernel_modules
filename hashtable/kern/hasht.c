/*
===============================================================================
Driver Name		:		hasht
Author			:	    viveknainwal@gmail.com	
License			:		GPL
Description		:		LINUX DEVICE DRIVER PROJECT
===============================================================================
*/

#include"hasht.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("vivek");

//Data to be placed in hashtable
struct appinfo {
	u32 psid;
	struct hlist_node hlist;
};

// Define and initilize the hashtable 2^3 buckets
DEFINE_HASHTABLE(app_ht, 3);

static int __init hasht_init(void)
{
	u8 i;
	u32 bkt;
	struct appinfo *ptr;
	struct appinfo *curr;

	PINFO("INIT\n");
	PINFO("Adding ten appinfo  to hashtable");
	for(i = 0 ; i < 10 ;i++){
		ptr =  (struct appinfo *)kmalloc(sizeof(struct appinfo),GFP_KERNEL);
		if (IS_ERR(ptr)){
			PERR("Not able to allocate data buffer");
		}
		ptr->psid = i;

		hash_add_rcu(app_ht,&ptr->hlist,ptr->psid);
	}

	PINFO("Traversing all the application in hashtable\n");
	hash_for_each_rcu(app_ht,bkt,curr,hlist){
		PINFO("psid=%d is in bucket %d\n", curr->psid, bkt);
	}

    PINFO("Removing appinfo  with psid == 5\n");
    hash_for_each_possible_rcu(app_ht, curr, hlist, 5){
        if (curr->psid == 5){
            hash_del_rcu(&curr->hlist);
            kfree(curr);
            break;
        }
    }

    PINFO("Again Traversing all the application in hashtable\n");
    hash_for_each_rcu(app_ht,bkt,curr,hlist){
        printk(KERN_INFO "psid=%d is in bucket %d\n", curr->psid, bkt);
    }
	return 0;
}

static void __exit hasht_exit(void)
{	
    u32 bkt;
    struct appinfo *curr;

	PINFO("EXIT\n");
	PINFO("Removing all the entries from hashtable \n");
	hash_for_each_rcu(app_ht,bkt,curr,hlist){
		PINFO("Removing bkt=%d,psid=%d\n",bkt,curr->psid);
		hash_del_rcu(&curr->hlist);
		kfree(curr);
	}

}

module_init(hasht_init);
module_exit(hasht_exit);

