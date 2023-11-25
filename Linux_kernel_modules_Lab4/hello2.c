

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include "hello1.h"

MODULE_AUTHOR("Taras Vytvytskyi");
MODULE_DESCRIPTION("Hello, world utilising linked list API");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int howmany = 1;
module_param(howmany, uint, 0444);
MODULE_PARM_DESC(howmany, "Amount of loop runs");


static int __init hello_init(void)
{
	if (howmany > 10)
		return -EINVAL;
	if ((howmany == 0) || ((howmany >= 5) && (howmany <= 10)))
		pr_warn("Warning: input parameter is within specified range\n");

	print_hello(howmany);

	return 0;
}

static void __exit hello_exit(void)
{
	exit_hello();
}

module_init(hello_init);
module_exit(hello_exit);
