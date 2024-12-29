#ifndef NETSTAT_CUSTOM_H
#define NETSTAT_CUSTOM_H

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <net/tcp.h>
#include <net/inet_sock.h>

#define PROCFS_NAME "netstat_custom"

#define FILTER_SIZE 128

extern char filter[FILTER_SIZE];

const char *tcp_state_to_string(int state);

bool filter_state(int state);

int netstat_show(struct seq_file *m, void *v);

int netstat_open(struct inode *inode, struct file *file);

ssize_t netstat_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos);

extern const struct proc_ops netstat_ops;

#endif 
