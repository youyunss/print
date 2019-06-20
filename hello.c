
#include <linux/init.h>

#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/unistd.h>

#include <asm/uaccess.h>

#include <linux/sched.h>



#define my_syscall_num 223

#define sys_call_table_address 0xc15b3000



static int counter = 0;

struct process

{

	int pid;

	int depth;

};



struct process a[512];



unsigned int clear_and_return_cr0(void);

void setback_cr0(unsigned int val);

asmlinkage long sys_mycall(char __user *buf);

int orig_cr0;

unsigned long *sys_call_table = 0;

static int(*anything_saved)(void);





void processtree(struct task_struct * p, int b)

{

	struct list_head * l;

	a[counter].pid = p->pid;

	a[counter].depth = b;

	counter++;

	for (l = p->children.next; l != &(p->children); l = l->next)

	{

		struct task_struct *t = list_entry(l, struct task_struct, sibling);

		processtree(t, b + 1);

	}

}



unsigned int clear_and_return_cr0(void)

{

	unsigned int cr0 = 0;

	unsigned int ret;

	asm("movl %%cr0, %%eax":"=a"(cr0));

	ret = cr0;

	cr0 &= 0xfffeffff;

	asm("movl %%eax, %%cr0"::"a"(cr0));

	return ret;

}



void setback_cr0(unsigned int val)//��ȡval��ֵ��eax�Ĵ������ٽ�eax�Ĵ�����ֵ����cr0��

{

	asm volatile("movl %%eax, %%cr0"::"a"(val));

}



static int __init init_addsyscall(void)

{

	printk("hello,lihuan kernel\n");

	sys_call_table = (unsigned long *)sys_call_table_address;//��ȡϵͳ���÷����׵�ַ

	printk("%x\n", sys_call_table);

	anything_saved = (int(*)(void)) (sys_call_table[my_syscall_num]);//����ԭʼϵͳ���õĵ�ַ

	orig_cr0 = clear_and_return_cr0();//����cr0�ɸ���

	sys_call_table[my_syscall_num] = (unsigned long)&sys_mycall;//����ԭʼ��ϵͳ���÷����ַ

	setback_cr0(orig_cr0);//����Ϊԭʼ��ֻ��cr0

	return 0;

}



asmlinkage long sys_mycall(char __user * buf)

{

	int b = 0;

	struct task_struct * p;

	printk("This is lihuan_syscall!\n");

	/*	if(num%2==0)

			{num=num%10000;}

		else

			{num=num%100000;}

		return num;

	*/

	/*	for(i=0;i<20;i++)

			a[i]=15;



		if(copy_to_user(buf,a,20*sizeof(int)))

			return -EFAULT;

		else

			return sizeof(a);

	*/

	for (p = current; p != &init_task; p = p->parent);

	processtree(p, b);



	if (copy_to_user((struct process *)buf, a, 512 * sizeof(struct process)))

		return -EFAULT;

	else

		return sizeof(a);

}



static void __exit exit_addsyscall(void)

{

	//����cr0�ж�sys_call_table�ĸ���Ȩ�ޡ�

	orig_cr0 = clear_and_return_cr0();//����cr0�ɸ���

	//�ָ�ԭ�е��ж��������еĺ���ָ���ֵ��

	sys_call_table[my_syscall_num] = (unsigned long)anything_saved;

	//�ָ�ԭ�е�cr0��ֵ

	setback_cr0(orig_cr0);

	printk("call lihuan exit \n");

}



module_init(init_addsyscall);

module_exit(exit_addsyscall);

MODULE_LICENSE("GPL");
