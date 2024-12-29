#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <net/tcp.h>
#include <net/inet_sock.h>
#include "netstat_procfs.h"

char filter[FILTER_SIZE] = {0};

const char *tcp_state_to_string(int state) {
    switch (state) {
        case TCP_ESTABLISHED: return "ESTABLISHED";
        case TCP_SYN_SENT: return "SYN_SENT";
        case TCP_SYN_RECV: return "SYN_RECV";
        case TCP_FIN_WAIT1: return "FIN_WAIT1";
        case TCP_FIN_WAIT2: return "FIN_WAIT2";
        case TCP_TIME_WAIT: return "TIME_WAIT";
        case TCP_CLOSE: return "CLOSE";
        case TCP_CLOSE_WAIT: return "CLOSE_WAIT";
        case TCP_LAST_ACK: return "LAST_ACK";
        case TCP_LISTEN: return "LISTEN";
        case TCP_CLOSING: return "CLOSING";
        case TCP_NEW_SYN_RECV: return "NEW_SYN_RECV";
        default: return "UNKNOWN";
    }
}

bool filter_state(int state) {
    if (strlen(filter) == 0)
        return true;

    return strcmp(filter, tcp_state_to_string(state)) == 0;
}

int netstat_show(struct seq_file *m, void *v) {
    struct sock *sk;
    struct hlist_nulls_node *node;
    unsigned int bucket;

    seq_printf(m, "%-21s %-21s %-15s\n", 
               "Local Address:Port", 
               "Remote Address:Port", 
               "State");

    for (bucket = 0; bucket <= tcp_hashinfo.ehash_mask; bucket++) {
        sk_nulls_for_each(sk, node, &tcp_hashinfo.ehash[bucket].chain) {
            struct inet_sock *inet = inet_sk(sk);

            if (!filter_state(sk->sk_state))
                continue;

            seq_printf(m, "%-21pI4:%-5u %-21pI4:%-5u %-15s\n",
                       &inet->inet_saddr, ntohs(inet->inet_sport),
                       &inet->inet_daddr, ntohs(inet->inet_dport),
                       tcp_state_to_string(sk->sk_state));
        }
    }

    return 0;
}

int netstat_open(struct inode *inode, struct file *file) {
    return single_open(file, netstat_show, NULL);
}

ssize_t netstat_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos) {
    if (count >= sizeof(filter))
        return -EINVAL;

    if (copy_from_user(filter, buffer, count))
        return -EFAULT;

    filter[count] = '\0';
    strim(filter);
    pr_info("Received filter: %s\n", filter);

    return count;
}

const struct proc_ops netstat_ops = {
    .proc_open = netstat_open,
    .proc_read = seq_read,
    .proc_write = netstat_write,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init netstat_init(void) {
    struct proc_dir_entry *entry;
    entry = proc_create(PROCFS_NAME, 0666, NULL, &netstat_ops);
    if (!entry) {
        return -ENOMEM;
    }

    pr_info("netstat_custom module loaded\n");
    return 0;
}

static void __exit netstat_exit(void) {
    remove_proc_entry(PROCFS_NAME, NULL);
    pr_info("netstat_custom module unloaded\n");
}

module_init(netstat_init);
module_exit(netstat_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mari");
MODULE_DESCRIPTION("Custom netstat implementation using procfs");
