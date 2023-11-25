

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/list.h>
#include <linux/types.h>
#include "hello1.h"

struct event_entry_t {
	ktime_t time;
	ktime_t time_after_print;
	struct list_head next;
};

MODULE_AUTHOR("Taras Vytvytskyi");
MODULE_DESCRIPTION("Linked list hello-printer API");
MODULE_LICENSE("Dual BSD/GPL");

static LIST_HEAD(events_list_head);

int print_hello(unsigned int howmany)
{
	struct event_entry_t *event = NULL;
	int i;

	for (i = 0; i < howmany; i++) {
		event = kmalloc(sizeof(struct event_entry_t), GFP_KERNEL);
		if (event == NULL)
			return -ENOMEM;
		event->time = ktime_get();
		INIT_LIST_HEAD(&event->next);
		list_add_tail(&event->next, &events_list_head);
		pr_info("Hello, world!\n");
		event->time_after_print = ktime_get();
	}

	return 0;
}
EXPORT_SYMBOL(print_hello);

void exit_hello(void)
{
	struct event_entry_t *md, *tmp;
	int counter = 0;

	list_for_each_entry_safe(md, tmp, &events_list_head, next) {
		pr_info("Print #%d took %lld nanoseconds\n",
			   counter++, md->time_after_print - md->time);
		list_del(&md->next);
		kfree(md);
	}
}
EXPORT_SYMBOL(exit_hello);
