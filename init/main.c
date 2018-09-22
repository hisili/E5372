/*
 *  linux/init/main.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  GK 2/5/95  -  Changed to support mounting root fs via NFS
 *  Added initrd & change_root: Werner Almesberger & Hans Lermen, Feb '96
 *  Moan early if gcc is old, avoiding bogus kernels - Paul Gortmaker, May '96
 *  Simplified starting of init:  Michael A. Griffith <grif@acm.org> 
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/stackprotector.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <linux/initrd.h>
#include <linux/bootmem.h>
#include <linux/acpi.h>
#include <linux/tty.h>
#include <linux/percpu.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/kernel_stat.h>
#include <linux/start_kernel.h>
#include <linux/security.h>
#include <linux/smp.h>
#include <linux/workqueue.h>
#include <linux/profile.h>
#include <linux/rcupdate.h>
#include <linux/moduleparam.h>
#include <linux/kallsyms.h>
#include <linux/writeback.h>
#include <linux/cpu.h>
#include <linux/cpuset.h>
#include <linux/cgroup.h>
#include <linux/efi.h>
#include <linux/tick.h>
#include <linux/interrupt.h>
#include <linux/taskstats_kern.h>
#include <linux/delayacct.h>
#include <linux/unistd.h>
#include <linux/rmap.h>
#include <linux/mempolicy.h>
#include <linux/key.h>
#include <linux/buffer_head.h>
#include <linux/page_cgroup.h>
#include <linux/debug_locks.h>
#include <linux/debugobjects.h>
#include <linux/lockdep.h>
#include <linux/kmemleak.h>
#include <linux/pid_namespace.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/idr.h>
#include <linux/kgdb.h>
#include <linux/ftrace.h>
#include <linux/async.h>
#include <linux/kmemcheck.h>
#include <linux/kmemtrace.h>
#include <linux/sfi.h>
#include <linux/shmem_fs.h>
#include <linux/slab.h>
#include <trace/boot.h>

#include <asm/io.h>
#include <asm/bugs.h>
#include <asm/setup.h>
#include <asm/sections.h>
#include <asm/cacheflush.h>
#include "BSP.h"
#include <vxprintk.h>

#ifdef CONFIG_X86_LOCAL_APIC
#include <asm/smp.h>
#endif

#include <generated/FeatureConfig.h>

#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
#include <mach/powerExchange.h>
#include <linux/reboot.h>

void power_on_proc_init( void );

static __init void power_on_check( void );

int g_power_timeout_test = 0; /* just for test */
#endif /* FEATURE_POWER_ON_OFF */

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON)
/*Include DEVICE_EVENT type define header file.*/
#include <linux/netlink.h>

extern uint8_t is_current_firmware_factory;
#endif /*(defined(CHIP_BB_6920...*/

static int kernel_init(void *);

#ifdef BOOT_OPTI_BUILDIN
extern int VOS_ModuleInit(void);
#endif
extern void init_IRQ(void);
extern void fork_init(unsigned long);
extern void mca_init(void);
extern void sbus_init(void);
extern void prio_tree_init(void);
extern void radix_tree_init(void);
extern void free_initmem(void);
#ifndef CONFIG_DEBUG_RODATA
static inline void mark_rodata_ro(void) { }
#endif

#ifdef CONFIG_TC
extern void tc_init(void);
#endif

enum system_states system_state __read_mostly;
EXPORT_SYMBOL(system_state);

/*
 * Boot command-line arguments
 */
#define MAX_INIT_ARGS CONFIG_INIT_ENV_ARG_LIMIT
#define MAX_INIT_ENVS CONFIG_INIT_ENV_ARG_LIMIT

extern void time_init(void);
/* Default late time init is NULL. archs can override this later. */
void (*__initdata late_time_init)(void);
extern void softirq_init(void);

/* Untouched command line saved by arch-specific code. */
char __initdata boot_command_line[COMMAND_LINE_SIZE];
/* Untouched saved command line (eg. for /proc) */
char *saved_command_line;
/* Command line for parameter parsing */
static char *static_command_line;

static char *execute_command;
static char *ramdisk_execute_command;

#ifdef CONFIG_SMP
/* Setup configured maximum number of CPUs to activate */
unsigned int setup_max_cpus = NR_CPUS;
EXPORT_SYMBOL(setup_max_cpus);


/*
 * Setup routine for controlling SMP activation
 *
 * Command-line option of "nosmp" or "maxcpus=0" will disable SMP
 * activation entirely (the MPS table probe still happens, though).
 *
 * Command-line option of "maxcpus=<NUM>", where <NUM> is an integer
 * greater than 0, limits the maximum number of CPUs activated in
 * SMP mode to <NUM>.
 */

void __weak arch_disable_smp_support(void) { }

static int __init nosmp(char *str)
{
	setup_max_cpus = 0;
	arch_disable_smp_support();

	return 0;
}

early_param("nosmp", nosmp);

/* this is hard limit */
static int __init nrcpus(char *str)
{
	int nr_cpus;

	get_option(&str, &nr_cpus);
	if (nr_cpus > 0 && nr_cpus < nr_cpu_ids)
		nr_cpu_ids = nr_cpus;

	return 0;
}

early_param("nr_cpus", nrcpus);

static int __init maxcpus(char *str)
{
	get_option(&str, &setup_max_cpus);
	if (setup_max_cpus == 0)
		arch_disable_smp_support();

	return 0;
}

early_param("maxcpus", maxcpus);
#else
static const unsigned int setup_max_cpus = NR_CPUS;
#endif

/*
 * If set, this is an indication to the drivers that reset the underlying
 * device before going ahead with the initialization otherwise driver might
 * rely on the BIOS and skip the reset operation.
 *
 * This is useful if kernel is booting in an unreliable environment.
 * For ex. kdump situaiton where previous kernel has crashed, BIOS has been
 * skipped and devices will be in unknown state.
 */
unsigned int reset_devices;
EXPORT_SYMBOL(reset_devices);

static int __init set_reset_devices(char *str)
{
	reset_devices = 1;
	return 1;
}

__setup("reset_devices", set_reset_devices);

static char * argv_init[MAX_INIT_ARGS+2] = { "init", NULL, };
char * envp_init[MAX_INIT_ENVS+2] = { "HOME=/", "TERM=linux", NULL, };
static const char *panic_later, *panic_param;

extern struct obs_kernel_param __setup_start[], __setup_end[];

static int __init obsolete_checksetup(char *line)
{
	struct obs_kernel_param *p;
	int had_early_param = 0;

	p = __setup_start;
	do {
		int n = strlen(p->str);
		if (!strncmp(line, p->str, n)) {
			if (p->early) {
				/* Already done in parse_early_param?
				 * (Needs exact match on param part).
				 * Keep iterating, as we can have early
				 * params and __setups of same names 8( */
				if (line[n] == '\0' || line[n] == '=')
					had_early_param = 1;
			} else if (!p->setup_func) {
				printk(KERN_WARNING "Parameter %s is obsolete,"
				       " ignored\n", p->str);
				return 1;
			} else if (p->setup_func(line + n))
				return 1;
		}
		p++;
	} while (p < __setup_end);

	return had_early_param;
}

/*
 * This should be approx 2 Bo*oMips to start (note initial shift), and will
 * still work even if initially too large, it will just take slightly longer
 */
unsigned long loops_per_jiffy = (1<<12);

EXPORT_SYMBOL(loops_per_jiffy);

static int __init debug_kernel(char *str)
{
	console_loglevel = 10;
	return 0;
}

static int __init quiet_kernel(char *str)
{
	console_loglevel = 4;
	return 0;
}

early_param("debug", debug_kernel);
early_param("quiet", quiet_kernel);

static int __init loglevel(char *str)
{
	get_option(&str, &console_loglevel);
	return 0;
}

early_param("loglevel", loglevel);

/*
 * Unknown boot options get handed to init, unless they look like
 * unused parameters (modprobe will find them in /proc/cmdline).
 */
static int __init unknown_bootoption(char *param, char *val)
{
	/* Change NUL term back to "=", to make "param" the whole string. */
	if (val) {
		/* param=val or param="val"? */
		if (val == param+strlen(param)+1)
			val[-1] = '=';
		else if (val == param+strlen(param)+2) {
			val[-2] = '=';
			memmove(val-1, val, strlen(val)+1);
			val--;
		} else
			BUG();
	}

	/* Handle obsolete-style parameters */
	if (obsolete_checksetup(param))
		return 0;

	/* Unused module parameter. */
	if (strchr(param, '.') && (!val || strchr(param, '.') < val))
		return 0;

	if (panic_later)
		return 0;

	if (val) {
		/* Environment option */
		unsigned int i;
		for (i = 0; envp_init[i]; i++) {
			if (i == MAX_INIT_ENVS) {
				panic_later = "Too many boot env vars at `%s'";
				panic_param = param;
			}
			if (!strncmp(param, envp_init[i], val - param))
				break;
		}
		envp_init[i] = param;
	} else {
		/* Command line option */
		unsigned int i;
		for (i = 0; argv_init[i]; i++) {
			if (i == MAX_INIT_ARGS) {
				panic_later = "Too many boot init vars at `%s'";
				panic_param = param;
			}
		}
		argv_init[i] = param;
	}
	return 0;
}

#ifdef CONFIG_DEBUG_PAGEALLOC
int __read_mostly debug_pagealloc_enabled = 0;
#endif

static int __init init_setup(char *str)
{
	unsigned int i;

	execute_command = str;
	/*
	 * In case LILO is going to boot us with default command line,
	 * it prepends "auto" before the whole cmdline which makes
	 * the shell think it should execute a script with such name.
	 * So we ignore all arguments entered _before_ init=... [MJ]
	 */
	for (i = 1; i < MAX_INIT_ARGS; i++)
		argv_init[i] = NULL;
	return 1;
}
__setup("init=", init_setup);

static int __init rdinit_setup(char *str)
{
	unsigned int i;

	ramdisk_execute_command = str;
	/* See "auto" comment in init_setup */
	for (i = 1; i < MAX_INIT_ARGS; i++)
		argv_init[i] = NULL;
	return 1;
}
__setup("rdinit=", rdinit_setup);

#ifndef CONFIG_SMP

#ifdef CONFIG_X86_LOCAL_APIC
static void __init smp_init(void)
{
	APIC_init_uniprocessor();
}
#else
#define smp_init()	do { } while (0)
#endif

static inline void setup_nr_cpu_ids(void) { }
static inline void smp_prepare_cpus(unsigned int maxcpus) { }

#else

/* Setup number of possible processor ids */
int nr_cpu_ids __read_mostly = NR_CPUS;
EXPORT_SYMBOL(nr_cpu_ids);

/* An arch may set nr_cpu_ids earlier if needed, so this would be redundant */
static void __init setup_nr_cpu_ids(void)
{
	nr_cpu_ids = find_last_bit(cpumask_bits(cpu_possible_mask),NR_CPUS) + 1;
}

/* Called by boot processor to activate the rest. */
static void __init smp_init(void)
{
	unsigned int cpu;

	/* FIXME: This should be done in userspace --RR */
	for_each_present_cpu(cpu) {
		if (num_online_cpus() >= setup_max_cpus)
			break;
		if (!cpu_online(cpu))
			cpu_up(cpu);
	}

	/* Any cleanup work */
	printk(KERN_INFO "Brought up %ld CPUs\n", (long)num_online_cpus());
	smp_cpus_done(setup_max_cpus);
}

#endif

/*
 * We need to store the untouched command line for future reference.
 * We also need to store the touched command line since the parameter
 * parsing is performed in place, and we should allow a component to
 * store reference of name/value for future reference.
 */
static void __init setup_command_line(char *command_line)
{
	saved_command_line = alloc_bootmem(strlen (boot_command_line)+1);
	static_command_line = alloc_bootmem(strlen (command_line)+1);
	strcpy (saved_command_line, boot_command_line);
	strcpy (static_command_line, command_line);
}

/*
 * We need to finalize in a non-__init function or else race conditions
 * between the root thread and the init thread may cause start_kernel to
 * be reaped by free_initmem before the root thread has proceeded to
 * cpu_idle.
 *
 * gcc-3.4 accidentally inlines this function, so use noinline.
 */

static __initdata DECLARE_COMPLETION(kthreadd_done);

static noinline void __init_refok rest_init(void)
	__releases(kernel_lock)
{
	int pid;

	rcu_scheduler_starting();
	/*
	 * We need to spawn init first so that it obtains pid 1, however
	 * the init task will end up wanting to create kthreads, which, if
	 * we schedule it before we create kthreadd, will OOPS.
	 */
	kernel_thread(kernel_init, NULL, CLONE_FS | CLONE_SIGHAND);
	numa_default_policy();
	pid = kernel_thread(kthreadd, NULL, CLONE_FS | CLONE_FILES);
	rcu_read_lock();
	kthreadd_task = find_task_by_pid_ns(pid, &init_pid_ns);
	rcu_read_unlock();
	complete(&kthreadd_done);
	unlock_kernel();

	/*
	 * The boot idle thread must execute schedule()
	 * at least once to get things moving:
	 */
	init_idle_bootup_task(current);
	preempt_enable_no_resched();
	schedule();
	preempt_disable();

	/* Call into cpu_idle with preempt disabled */
	cpu_idle();
}

/* Check for early params. */
static int __init do_early_param(char *param, char *val)
{
	struct obs_kernel_param *p;

	for (p = __setup_start; p < __setup_end; p++) {
		if ((p->early && strcmp(param, p->str) == 0) ||
		    (strcmp(param, "console") == 0 &&
		     strcmp(p->str, "earlycon") == 0)
		) {
			if (p->setup_func(val) != 0)
				printk(KERN_WARNING
				       "Malformed early option '%s'\n", param);
		}
	}
	/* We accept everything at this stage. */
	return 0;
}

void __init parse_early_options(char *cmdline)
{
	parse_args("early options", cmdline, NULL, 0, do_early_param);
}

/* Arch code calls this early on, or if not, just before other parsing. */
void __init parse_early_param(void)
{
	static __initdata int done = 0;
	static __initdata char tmp_cmdline[COMMAND_LINE_SIZE];

	if (done)
		return;

	/* All fall through to do_early_param. */
	strlcpy(tmp_cmdline, boot_command_line, COMMAND_LINE_SIZE);
	parse_early_options(tmp_cmdline);
	done = 1;
}

/*
 *	Activate the first processor.
 */

static void __init boot_cpu_init(void)
{
	int cpu = smp_processor_id();
	/* Mark the boot cpu "present", "online" etc for SMP and UP case */
	set_cpu_online(cpu, true);
	set_cpu_active(cpu, true);
	set_cpu_present(cpu, true);
	set_cpu_possible(cpu, true);
}

void __init __weak smp_setup_processor_id(void)
{
}

void __init __weak thread_info_cache_init(void)
{
}

/*
 * Set up kernel memory allocators
 */
static void __init mm_init(void)
{
	/*
	 * page_cgroup requires countinous pages as memmap
	 * and it's bigger than MAX_ORDER unless SPARSEMEM.
	 */
	page_cgroup_init_flatmem();
	mem_init();
	kmem_cache_init();
	pgtable_cache_init();
	vmalloc_init();
}

asmlinkage void __init start_kernel(void)
{
	char * command_line;
	extern struct kernel_param __start___param[], __stop___param[];

	smp_setup_processor_id();

	/*
	 * Need to run as early as possible, to initialize the
	 * lockdep hash:
	 */
	lockdep_init();
	debug_objects_early_init();

	/*
	 * Set up the the initial canary ASAP:
	 */
	boot_init_stack_canary();

	cgroup_init_early();

	local_irq_disable();
	early_boot_irqs_off();
	early_init_irq_lock_class();

/*
 * Interrupts are still disabled. Do necessary setups, then
 * enable them
 */
	lock_kernel();
	tick_init();
	boot_cpu_init();
	page_address_init();
	printk(KERN_NOTICE "%s", linux_banner);
	setup_arch(&command_line);
	mm_init_owner(&init_mm, &init_task);
	setup_command_line(command_line);
	setup_nr_cpu_ids();
	setup_per_cpu_areas();
	smp_prepare_boot_cpu();	/* arch-specific boot-cpu hooks */

	build_all_zonelists(NULL);
	page_alloc_init();

	printk(KERN_NOTICE "Kernel command line: %s\n", boot_command_line);
	parse_early_param();
	parse_args("Booting kernel", static_command_line, __start___param,
		   __stop___param - __start___param,
		   &unknown_bootoption);
	/*
	 * These use large bootmem allocations and must precede
	 * kmem_cache_init()
	 */
	pidhash_init();
	vfs_caches_init_early();
	sort_main_extable();
	trap_init();
	mm_init();
	/*
	 * Set up the scheduler prior starting any interrupts (such as the
	 * timer interrupt). Full topology setup happens at smp_init()
	 * time - but meanwhile we still have a functioning scheduler.
	 */
	sched_init();
	/*
	 * Disable preemption - early bootup scheduling is extremely
	 * fragile until we cpu_idle() for the first time.
	 */
	preempt_disable();
	if (!irqs_disabled()) {
		printk(KERN_WARNING "start_kernel(): bug: interrupts were "
				"enabled *very* early, fixing it\n");
		local_irq_disable();
	}
	rcu_init();
	radix_tree_init();
	/* init some links before init_ISA_irqs() */
	early_irq_init();
	init_IRQ();
	prio_tree_init();
	init_timers();
	hrtimers_init();
	softirq_init();
	timekeeping_init();
	time_init();
	profile_init();
#ifdef CONFIG_MICROSTATE_ACCT
    msa_init(&init_task);
#endif
	if (!irqs_disabled())
		printk(KERN_CRIT "start_kernel(): bug: interrupts were "
				 "enabled early\n");
	early_boot_irqs_on();
	local_irq_enable();

	/* Interrupts are enabled now so all GFP allocations are safe. */
	gfp_allowed_mask = __GFP_BITS_MASK;

	kmem_cache_init_late();

	/*
	 * HACK ALERT! This is early. We're enabling the console before
	 * we've done PCI setups etc, and console_init() must be aware of
	 * this. But we do want output early, in case something goes wrong.
	 */
	console_init();
	if (panic_later)
		panic(panic_later, panic_param);

	lockdep_info();

	/*
	 * Need to run this when irqs are enabled, because it wants
	 * to self-test [hard/soft]-irqs on/off lock inversion bugs
	 * too:
	 */
	locking_selftest();

#ifdef CONFIG_BLK_DEV_INITRD
	if (initrd_start && !initrd_below_start_ok &&
	    page_to_pfn(virt_to_page((void *)initrd_start)) < min_low_pfn) {
		printk(KERN_CRIT "initrd overwritten (0x%08lx < 0x%08lx) - "
		    "disabling it.\n",
		    page_to_pfn(virt_to_page((void *)initrd_start)),
		    min_low_pfn);
		initrd_start = 0;
	}
#endif
	page_cgroup_init();
	enable_debug_pagealloc();
	kmemtrace_init();
	kmemleak_init();
	debug_objects_mem_init();
	idr_init_cache();
	setup_per_cpu_pageset();
	numa_policy_init();
	if (late_time_init)
		late_time_init();
	sched_clock_init();
	calibrate_delay();
	pidmap_init();
	anon_vma_init();
#ifdef CONFIG_X86
	if (efi_enabled)
		efi_enter_virtual_mode();
#endif
	thread_info_cache_init();
	cred_init();
	fork_init(totalram_pages);
	proc_caches_init();
	buffer_init();
	key_init();
	security_init();
	dbg_late_init();
	vfs_caches_init(totalram_pages);
	signals_init();
	/* rootfs populating might need page-writeback */
	page_writeback_init();
#ifdef CONFIG_PROC_FS
	proc_root_init();
#endif
	cgroup_init();
	cpuset_init();
	taskstats_init_early();
	delayacct_init();

	check_bugs();

	acpi_early_init(); /* before LAPIC and SMP init */
	sfi_init_late();

	ftrace_init();

	/* Do the rest non-__init'ed, we're now alive */
	rest_init();
}

/* Call all constructor functions linked into the kernel. */
static void __init do_ctors(void)
{
#ifdef CONFIG_CONSTRUCTORS
	ctor_fn_t *fn = (ctor_fn_t *) __ctors_start;

	for (; fn < (ctor_fn_t *) __ctors_end; fn++)
		(*fn)();
#endif
}

int initcall_debug;
core_param(initcall_debug, initcall_debug, bool, 0644);

static char msgbuf[64];
static struct boot_trace_call call;
static struct boot_trace_ret ret;

int do_one_initcall(initcall_t fn)
{
	int count = preempt_count();
	ktime_t calltime, delta, rettime;

	if (initcall_debug) {
		call.caller = task_pid_nr(current);
		printk("calling  %pF @ %i\n", fn, call.caller);
		calltime = ktime_get();
		trace_boot_call(&call, fn);
		enable_boot_trace();
	}

	ret.result = fn();

	if (initcall_debug) {
		disable_boot_trace();
		rettime = ktime_get();
		delta = ktime_sub(rettime, calltime);
		ret.duration = (unsigned long long) ktime_to_ns(delta) >> 10;
		trace_boot_ret(&ret, fn);
		printk("initcall %pF returned %d after %Ld usecs\n", fn,
			ret.result, ret.duration);
	}

	msgbuf[0] = 0;

	if (ret.result && ret.result != -ENODEV && initcall_debug)
		sprintf(msgbuf, "error code %d ", ret.result);

	if (preempt_count() != count) {
		strlcat(msgbuf, "preemption imbalance ", sizeof(msgbuf));
		preempt_count() = count;
	}
	if (irqs_disabled()) {
		strlcat(msgbuf, "disabled interrupts ", sizeof(msgbuf));
		local_irq_enable();
	}
	if (msgbuf[0]) {
		printk("initcall %pF returned with %s\n", fn, msgbuf);
	}

	return ret.result;
}


extern initcall_t __initcall_start[], __initcall_end[], __early_initcall_end[];

static void __init do_initcalls(void)
{
	initcall_t *fn;

	for (fn = __early_initcall_end; fn < __initcall_end; fn++)
		do_one_initcall(*fn);

	/* Make sure there is no pending stuff from the initcall sequence */
	flush_scheduled_work();
}

/*
 * Ok, the machine is now initialized. None of the devices
 * have been touched yet, but the CPU subsystem is up and
 * running, and memory and process management works.
 *
 * Now we can finally start doing some real work..
 */
static void __init do_basic_setup(void)
{
	init_workqueues();
	cpuset_init_smp();
	usermodehelper_init();
	init_tmpfs();
	driver_init();
	init_irq_proc();
	do_ctors();
	do_initcalls();
}

static void __init do_pre_smp_initcalls(void)
{
	initcall_t *fn;

	for (fn = __initcall_start; fn < __early_initcall_end; fn++)
		do_one_initcall(*fn);
}

static void run_init_process(char *init_filename)
{
	argv_init[0] = init_filename;
	kernel_execve(init_filename, argv_init, envp_init);
}

/* This is a non __init function. Force it to be noinline otherwise gcc
 * makes it inline to init() and it becomes part of init.text section
 */
static noinline int init_post(void)
	__releases(kernel_lock)
{
#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
    POWER_ON_STATUS_ENUM  enStatus;    
#endif
	/* need to finish all async __init code before freeing the memory */
	async_synchronize_full();
	free_initmem();
	unlock_kernel();
	mark_rodata_ro();
	system_state = SYSTEM_RUNNING;
	numa_default_policy();


	current->signal->flags |= SIGNAL_UNKILLABLE;

    /* l00131505, fixme later after added application */
#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
      printk( "\r\n############ Do Power Check \r\n" );
      power_on_check( );
      
      enStatus = power_on_status_get();

      if( POWER_ON_STATUS_FINISH_NORMAL == enStatus )
      {
          /* start application for normal mode */
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
          tftPwrOnAniInstall();
#endif
          printk( "\r\n ######### Linux start normal mode \r\n" );
      }
      else if( POWER_ON_STATUS_FINISH_CHARGE == enStatus )
      {
          /* start application for charge mode */
          printk( "\r\n ######### Linux start charge mode \r\n" );          
#if  (defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920ES_E5776S) || defined(FEATURE_BOARD_STUB_BJ_UDP)\
	||defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920CS_E5776S))
          tftPwrOffChgAniInstall();
#else
          balong_oled_ioctl( 1, OLED_DEL_TIMER, 0 );
          oledPwrOffChgAniInstall();
#endif		  
      }
      else
      {
           /* unknown status, it wouldn't happen because the status has been check
               in power_on_check() */
#if  (defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920ES_E5776S) || defined(FEATURE_BOARD_STUB_BJ_UDP)\
	||defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920CS_E5776S))
           tftClearWholeScreen();
		   /*BEGIN: AP, Motified by xuchao x00202188, 2012/2/20*/
           tftStringDisplay( 16, 58, (UINT8 *)"ERROR" );
		   /*END  : AP, Motified by xuchao x00202188, 2012/2/20*/
#else
           balong_oled_ioctl( 1, OLED_DEL_TIMER, 0 );
           oledClearWholeScreen();
           oledStringDisplay( 16, 16, (UINT8 *)"Error!" );
#endif		   
           panic( "\r\nUnknown power on mode\r\n" );
      }
        
#endif   

#ifdef BOOT_OPTI_BUILDIN
#if defined (FEATURE_FLASH_LESS)
#else
	
#endif
#endif

	printk(KERN_ERR "end log trace.");
	vxprintk_stop();
	
	if (ramdisk_execute_command) {
		run_init_process(ramdisk_execute_command);
		printk(KERN_WARNING "Failed to execute %s\n",
				ramdisk_execute_command);
	}

	/*
	 * We try each of these until one succeeds.
	 *
	 * The Bourne shell can be used instead of init if we are
	 * trying to recover a really broken machine.
	 */
	if (execute_command) {
		run_init_process(execute_command);
		printk(KERN_WARNING "Failed to execute %s.  Attempting "
					"defaults...\n", execute_command);
	}
	run_init_process("/sbin/init");
	run_init_process("/etc/init");
	run_init_process("/bin/init");
	run_init_process("/bin/sh");

	panic("No init found.  Try passing init= option to kernel. "
	      "See Linux Documentation/init.txt for guidance.");
}

#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
static __init void power_on_delay_and_shutdown( void )
{
    msleep( 2000 );

#if 1
    kernel_power_off(); /* can we call this function here ? */
#else
    /* Just notify C-CPU that linux has been shutdown.
        because nothing need to be close in linux while booting.
    */
    power_off_linux_status_set( POWER_OFF_LINUX_STATUS_SHUTDOWN );
#endif

    for( ; ; )
    {
        mdelay(2000 );
    }
}

static __init void power_on_battery_low_handler( void )
{
    printk( "\r\n power_on_battery_low_handler \r\n" );
#if  (defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920ES_E5776S) || defined(FEATURE_BOARD_STUB_BJ_UDP)\
	||defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920CS_E5776S))
    tftClearWholeScreen();
    /*Change the low batt msg from INSERT CHARGER to Connect charger per common E5 UI spec.*/
    //tftStringDisplay( 8, 58, (UINT8 *)"INSERT CHARGER" );
    tftStringDisplay(20, 56, (UINT8 *)"Low Battery");
#else
    oledClearWholeScreen();
    oledStringDisplay( 16, 16, (UINT8 *)"Low Battery" );
#endif
    power_on_delay_and_shutdown(  );
}

static __init void power_on_battery_error_handler( void )
{
    printk( "\r\n power_on_battery_error_handler \r\n" );
#if  (defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920ES_E5776S) || defined(FEATURE_BOARD_STUB_BJ_UDP)\
	||defined(PRODUCT_CFG_PRODUCT_TYPE_HI6920CS_E5776S))
    tftClearWholeScreen();
    if(HW_VER_PRODUCT_E5776_EM ==  BSP_HwGetVerMain())
	{
    	tftStringDisplay( 12, 58, (UINT8 *)"BATTERY ERROR");
    }
	else
	{
		tftStringDisplay( 12, 58, (UINT8 *)"Battery Error");
	}
#else
    oledClearWholeScreen();
    oledStringDisplay( 16, 16, (UINT8 *)"BATTERY ERROR" );
#endif
    power_on_delay_and_shutdown(  );
}

void drvShutdown( DRV_SHUTDOWN_REASON_ENUM enReason )
{
    BSP_U32  msg[4];

    printk( "\r\n drvShutdown reason = 0x%08X,  ID = %d \r\n", enReason, IFCP_FUNC_ONOFF_SHUTDOWN );
    msg[0] = IFC_MODULE_BSP;
    msg[1] = IFCP_FUNC_ONOFF_SHUTDOWN;
    msg[2] = (BSP_U32)(enReason);
    msg[3] = (BSP_U32)(0);

    BSP_IFC_Send( (BSP_VOID *)(msg), sizeof(msg) );
}
EXPORT_SYMBOL(drvShutdown);

#if 1 /* fixme later. stub function */
typedef struct device_event_st
{
    int device_id; /* device id  */
    int value;        /* device value */
    char * desc;    /* device payload */
}nl_dev_type;


int device_event_notify(nl_dev_type * nl_dev_skb) 
{
    BSP_U32  msg[4];
    
    if( 0 == g_power_timeout_test )
    {
        msleep( 3000 ); /* wait 3 second */
        msg[0] = IFC_MODULE_BSP;
        msg[1] = IFCP_FUNC_ONOFF_DRV_POWER_OFF;
        msg[2] = (BSP_U32)(0);
        msg[3] = (BSP_U32)(0);
        
        printk( "\r\n device_event_notify. now power off directly . ID = %d \r\n", IFCP_FUNC_ONOFF_DRV_POWER_OFF );  
        BSP_IFC_Send( (BSP_VOID *)(msg), sizeof(msg) );
    }
    else
    {
        printk( "\r\n device_event_notify. test timeout, dont call drvPowerOff now! \r\n" );
    }
    
    return 0;
}
#endif

void power_off_notify_main_controller( BSP_VOID * pMsgBody, BSP_U32 u32Len )
{
    BSP_U32              *ptr       = (BSP_U32 *)(pMsgBody);
    nl_dev_type           dev_event;    
    POWER_ON_STATUS_ENUM  enStatus = power_on_status_get();

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON)
    int ret = 0;
    DEVICE_EVENT dev_evt;
#endif /*(defined(CHIP_BB_6920...*/
    
    if( POWER_ON_STATUS_FINISH_NORMAL == enStatus )
    {
        /* Normal mode, send the power off message to Application.
           and the application to shutdown the system
         */
        dev_event.device_id = *ptr;
        dev_event.value     = *(ptr + 1);
        dev_event.desc      = NULL;

#if defined (FEATURE_LCD_ON)
	/*Í¶LCD·´¶ÔÆ±*/
		BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_LCD);
#endif

        printk( "\r\n power_off_notify_main_controller. id = %d, value = %d. \r\n", dev_event.device_id, dev_event.value );

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && (FEATURE_E5 == FEATURE_ON)
        dev_evt.device_id  = *ptr;
        dev_evt.event_code = *(ptr + 1);
        dev_evt.len        = 0;
        
        ret = device_event_report(&dev_evt, sizeof(DEVICE_EVENT));
        if (ret)
        {
            pr_err("%s: Send dev_evt[ID: %d, CODE: %d]to APP failed\n", 
                    __FUNCTION__, dev_evt.device_id, dev_evt.event_code);
        }

        /*If factory version, notify C-CPU to shutdown system.*/
        if (is_current_firmware_factory)
        {
            device_event_notify( &dev_event );
        }
#else
        device_event_notify( &dev_event );
#endif /*(defined(CHIP_BB_6920...*/
    }
    else
    {
        /* Charge / Exception mode, shutdown the linux directly */
        printk( "\r\n Charge mode(0x%08X), Power off directly! \r\n", enStatus );
        #if 0 /* No power off script, and orderly_poweroff not work now, so, call kernel_power_off directly */
        orderly_poweroff( 1 ); /* if shutdown failed, it will call kernel_power_off directly */
        #else
        kernel_power_off();
        #endif
    }
}

static __init void power_on_check( void )
{
        volatile POWER_ON_STATUS_ENUM   enStatus;
        BSP_U32                         u32Ret;

        power_on_proc_init();
        print_exchange_addr( );    
        printk( "\r\n start power on check.  IFCP_FUNC_ONOFF_NOTIFY_APP = %d, IFCP_FUNC_ONOFF_SHUTDOWN = %d\r\n", IFCP_FUNC_ONOFF_NOTIFY_APP, IFCP_FUNC_ONOFF_SHUTDOWN );
        /*
            IFC has been inited, so, create the remote function handler.
        */
        u32Ret = BSP_IFCP_RegFunc( IFCP_FUNC_ONOFF_NOTIFY_APP, (BSP_IFCP_REG_FUNC)power_off_notify_main_controller );
        if( OK != u32Ret )
        {
            panic( "\r\nPower on register func failed. \r\n");
        }

        /* It is a dead loop, if the status is wrong always, C-CPU will handle it */
        while( 1 )
        {
            enStatus =  power_on_status_get();

            switch( enStatus )
            {
                
            case POWER_ON_STATUS_BOOTING:
                /* booting, the status not be set, so, just wait */
                msleep( 500 );
                break;
            case POWER_ON_STATUS_CHARGING:
                /* the capacity is low, need charge a little time(max 30minutes), just wait */
                msleep( 1000 );
                break;
            case POWER_ON_STATUS_CAPACITY_LOW:
                /* the battery is low capacity, give the warning and shutdown the linux */
								
				BSP_SYNC_Give(SYNC_MODULE_CHG);
                power_on_battery_low_handler();
				
                /* wouldn't reach here */
                break;
            case POWER_ON_STATUS_BATTERY_ERROR:
                /* the battery is wrong, give the warning and shutdown the linux */
                power_on_battery_error_handler( );

                /* wouldn't reach here */
                break;
            case POWER_ON_STATUS_FINISH_NORMAL:
            case POWER_ON_STATUS_FINISH_CHARGE:
                /* power on process finish, GO ON */
                
                printk( "\r\n power on check finish \r\n" );
                return ;
                /* break; */
            default:
                /* Unknown status, just wait and let C-CPU to handle it */
                printk( "\r\n power on : unknown = 0x%08X\r\n", enStatus );
                msleep( 2000 );
                break;
              
            }
        }
}

#endif /* FEATURE_POWER_ON_OFF */

static int __init kernel_init(void * unused)
{
	/*
	 * Wait until kthreadd is all set-up.
	 */
	wait_for_completion(&kthreadd_done);
	lock_kernel();

	/*
	 * init can allocate pages on any node
	 */
	set_mems_allowed(node_states[N_HIGH_MEMORY]);
	/*
	 * init can run on any cpu.
	 */
	set_cpus_allowed_ptr(current, cpu_all_mask);
	/*
	 * Tell the world that we're going to be the grim
	 * reaper of innocent orphaned children.
	 *
	 * We don't want people to have to make incorrect
	 * assumptions about where in the task array this
	 * can be found.
	 */
	init_pid_ns.child_reaper = current;

	cad_pid = task_pid(current);

	smp_prepare_cpus(setup_max_cpus);

	do_pre_smp_initcalls();
	start_boot_trace();

	vxprintk_init();
	printk(KERN_ERR "start log trace.");
	
	smp_init();
	sched_init_smp();

	do_basic_setup();

	/* Open the /dev/console on the rootfs, this should never fail */
	if (sys_open((const char __user *) "/dev/console", O_RDWR, 0) < 0)
		printk(KERN_WARNING "Warning: unable to open an initial console.\n");

	(void) sys_dup(0);
	(void) sys_dup(0);
	/*
	 * check if there is an early userspace init.  If yes, let it do all
	 * the work
	 */

	if (!ramdisk_execute_command)
		ramdisk_execute_command = "/init";

	if (sys_access((const char __user *) ramdisk_execute_command, 0) != 0) {
		ramdisk_execute_command = NULL;
		prepare_namespace();
	}

	/*
	 * Ok, we have completed the initial bootup, and
	 * we're essentially up and running. Get rid of the
	 * initmem segments and start the user-mode stuff..
	 */

	init_post();
	return 0;
}
ENUM_POWER_ON_MODE BSP_ONOFF_StartupModeGet( void )
{
#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
    POWER_ON_STATUS_ENUM  enStatus = power_on_status_get();
    if( POWER_ON_STATUS_FINISH_NORMAL == enStatus )
    {
        /* start application for normal mode */
        return POWER_ON_NORMAL;
    }
    else if( POWER_ON_STATUS_FINISH_CHARGE == enStatus )
    {
        /* start application for charge mode */
        return POWER_ON_CHARGING;
    }
    else
    {
         /* unknown status, it wouldn't happen because the status has been check
             in power_on_check() */
        return POWER_ON_INVALID;
    }
#else
	return POWER_ON_NORMAL;
#endif	
}
EXPORT_SYMBOL(BSP_ONOFF_StartupModeGet);

#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
/* add power_on_off proc for test */
ssize_t power_on_proc_read( struct file *file, char __user *buf, size_t size, loff_t *ppos )
{
    ssize_t  len  = 0;
    int      mode = (int)BSP_ONOFF_StartupModeGet();
    char     mode_buf[ 128 + 4 ];
        
    memset( mode_buf, 0, 32 );
    sprintf( mode_buf, "#StartupMode:%d!", mode );            
    len = strlen(mode_buf) + 1;
                
    if( (size >= 32) && (0 == *ppos) )
    {
        copy_to_user( buf, mode_buf, len );
        
        printk( "\r\n power_on_proc_read. len = %d, MaxSize = %d, mode = %s!\r\n", len, (int)size, mode_buf );
        *ppos = len;
        return len;
    }
    else
    {
        printk( "\r\n power_on_proc_read. Invalid param. MaxSize = %d(>=32), ppos = %d(=0)\r\n", size, *ppos );
        return 0;
    }
}

ssize_t power_on_proc_write( struct file *file, const char __user *buf, size_t len, loff_t *ppos )
{
    char            cmd[ 128 ];
    unsigned long   param;
    
    if( len >= 64 )
    {
        return -EFAULT;
    }
    
    copy_from_user( cmd, buf, len );
    cmd[ len + 1 ] = 0;
    
    switch( cmd[0] )
    {
    case 'd':
        param = (unsigned long)(cmd[1] - '0');
        
        printk( "\r\n power_on_proc_write. len = %d, cmd(Shutdown) = %s \r\n", len, cmd );    
        
        drvShutdown( (DRV_SHUTDOWN_REASON_ENUM)param );
    break;
    case 't':
        param = (unsigned long)(cmd[1] - '0');
        g_power_timeout_test = (int)param;
        printk( "\r\n g_power_timeout_test = %d\r\n", g_power_timeout_test );
        break;
    case 'g':
        param = (unsigned long)BSP_ONOFF_StartupModeGet();
        printk( "\r\n BSP_ONOFF_StartupModeGet = %d", param );
    break;
    default:
        printk( "\r\n power_on_proc_write. len = %d, cmd(unknow) = %s \r\n", len, cmd );    
    break;
    }
        
    return len;
}

static const struct file_operations power_on_proc_fops = 
{
    .owner      = THIS_MODULE,
    .read       = power_on_proc_read,
    .write      = power_on_proc_write,
};

void power_on_proc_init( void )
{
    struct proc_dir_entry * entry;
    
    entry = proc_create( "power_on", S_IWUSR, NULL, &power_on_proc_fops );
    
}


#endif /* #if(FEATURE_POWER_ON_OFF == FEATURE_ON ) */



