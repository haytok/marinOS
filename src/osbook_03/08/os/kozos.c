#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "lib.h"

#define THREAD_NUM 6
#define THREAD_NAME_SIZE 15

/* ����åɡ�����ƥ����� */
typedef struct _kz_context {
  uint32 sp; /* �����å����ݥ��� */
} kz_context;

/* ������������ȥ����롦�֥��å�(TCB) */
typedef struct _kz_thread {
  struct _kz_thread *next;
  char name[THREAD_NAME_SIZE + 1]; /* ����å�̾ */
  char *stack;    /* �����å� */

  struct { /* ����åɤΥ������ȡ����å�(thread_init())���Ϥ��ѥ�᡼�� */
    kz_func_t func; /* ����åɤΥᥤ��ؿ� */
    int argc;       /* ����åɤΥᥤ��ؿ����Ϥ� argc */
    char **argv;    /* ����åɤΥᥤ��ؿ����Ϥ� argv */
  } init;

  struct { /* �����ƥࡦ�������ѥХåե� */
    kz_syscall_type_t type;
    kz_syscall_param_t *param;
  } syscall;

  kz_context context; /* ����ƥ����Ⱦ��� */
} kz_thread;

/* ����åɤΥ�ǥ��������塼 */
static struct {
  kz_thread *head;
  kz_thread *tail;
} readyque;

static kz_thread *current; /* �����ȡ�����å� */
static kz_thread threads[THREAD_NUM]; /* ������������ȥ����롦�֥��å� */
static kz_handler_t handlers[SOFTVEC_TYPE_NUM]; /* ����ߥϥ�ɥ� */

void dispatch(kz_context *context);

/* �����ȡ�����åɤ��ǥ��������塼����ȴ���Ф� */
static int getcurrent(void)
{
  if (current == NULL) {
    return -1;
  }

  /* �����ȡ�����åɤ�ɬ����Ƭ�ˤ���Ϥ��ʤΤǡ���Ƭ����ȴ���Ф� */
  readyque.head = current->next;
  if (readyque.head == NULL) {
    readyque.tail = NULL;
  }
  current->next = NULL;

  return 0;
}

/* �����ȡ�����åɤ��ǥ��������塼�˷Ҥ��� */
static int putcurrent(void)
{
  if (current == NULL) {
    return -1;
  }

  /* ��ǥ��������塼����������³���� */
  if (readyque.tail) {
    readyque.tail->next = current;
  } else {
    readyque.head = current;
  }
  readyque.tail = current;

  return 0;
}

static void thread_end(void)
{
  kz_exit();
}

/* ����åɤΥ������ȡ����å� */
static void thread_init(kz_thread *thp)
{
  /* ����åɤΥᥤ��ؿ���ƤӽФ� */
  thp->init.func(thp->init.argc, thp->init.argv);
  thread_end();
}

/* �����ƥࡦ������ν���(kz_run():����åɤε�ư) */
static kz_thread_id_t thread_run(kz_func_t func, char *name,
				 int stacksize, int argc, char *argv[])
{
  int i;
  kz_thread *thp;
  uint32 *sp;
  extern char userstack; /* ��󥫡�������ץȤ��������륹���å��ΰ� */
  static char *thread_stack = &userstack;

  /* �����Ƥ��륿����������ȥ����롦�֥��å��򸡺� */
  for (i = 0; i < THREAD_NUM; i++) {
    thp = &threads[i];
    if (!thp->init.func) /* ���Ĥ��ä� */
      break;
  }
  if (i == THREAD_NUM) /* ���Ĥ���ʤ��ä� */
    return -1;

  memset(thp, 0, sizeof(*thp));

  /* ������������ȥ����롦�֥��å�(TCB)������ */
  strcpy(thp->name, name);
  thp->next     = NULL;

  thp->init.func = func;
  thp->init.argc = argc;
  thp->init.argv = argv;

  /* �����å��ΰ����� */
  memset(thread_stack, 0, stacksize);
  thread_stack += stacksize;

  thp->stack = thread_stack; /* �����å������� */

  /* �����å��ν���� */
  sp = (uint32 *)thp->stack;
  *(--sp) = (uint32)thread_end;

  /*
   * �ץ�����ࡦ�����󥿤����ꤹ�롥
   */
  *(--sp) = (uint32)thread_init;

  *(--sp) = 0; /* ER6 */
  *(--sp) = 0; /* ER5 */
  *(--sp) = 0; /* ER4 */
  *(--sp) = 0; /* ER3 */
  *(--sp) = 0; /* ER2 */
  *(--sp) = 0; /* ER1 */

  /* ����åɤΥ������ȡ����å�(thread_init())���Ϥ����� */
  *(--sp) = (uint32)thp;  /* ER0 */

  /* ����åɤΥ���ƥ����Ȥ����� */
  thp->context.sp = (uint32)sp;

  /* �����ƥࡦ�������ƤӽФ�������åɤ��ǥ��������塼���᤹ */
  putcurrent();

  /* ����������������åɤ򡤥�ǥ��������塼����³���� */
  current = thp;
  putcurrent();

  return (kz_thread_id_t)current;
}

/* �����ƥࡦ������ν���(kz_exit():����åɤν�λ) */
static int thread_exit(void)
{
  /*
   * ����ʤ饹���å���������ƺ����ѤǤ���褦�ˤ��٤�������ά��
   * ���Τ��ᡤ����åɤ����ˤ��������õ��褦�ʤ��Ȥϸ����ǤǤ��ʤ���
   */
  puts(current->name);
  puts(" EXIT.\n");
  memset(current, 0, sizeof(*current));
  return 0;
}

/* ����ߥϥ�ɥ����Ͽ */
static int setintr(softvec_type_t type, kz_handler_t handler)
{
  static void thread_intr(softvec_type_t type, unsigned long sp);

  /*
   * ����ߤ�����դ��뤿��ˡ����եȥ�����������ߥ٥�����
   * OS�γ���߽����������Ȥʤ�ؿ�����Ͽ���롥
   */
  softvec_setintr(type, thread_intr);

  handlers[type] = handler; /* OS¦����ƤӽФ�����ߥϥ�ɥ����Ͽ */

  return 0;
}

static void call_functions(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /* �����ƥࡦ������μ¹����current���񤭴����Τ����� */
  switch (type) {
  case KZ_SYSCALL_TYPE_RUN: /* kz_run() */
    p->un.run.ret = thread_run(p->un.run.func, p->un.run.name,
			       p->un.run.stacksize,
			       p->un.run.argc, p->un.run.argv);
    break;
  case KZ_SYSCALL_TYPE_EXIT: /* kz_exit() */
    /* TCB���õ���Τǡ�����ͤ�񤭹���ǤϤ����ʤ� */
    thread_exit();
    break;
  default:
    break;
  }
}

/* �����ƥࡦ������ν��� */
static void syscall_proc(kz_syscall_type_t type, kz_syscall_param_t *p)
{
  /*
   * �����ƥࡦ�������ƤӽФ�������åɤ��ǥ��������塼����
   * ���������֤ǽ����ؿ���ƤӽФ������Τ��᥷���ƥࡦ�������
   * �ƤӽФ�������åɤ򤽤Τޤ�ư���³�����������ˤϡ�
   * �����ؿ��������� putcurrent() ��Ԥ�ɬ�פ����롥
   */
  getcurrent();
  call_functions(type, p);
}

/* ����åɤΥ������塼��� */
static void schedule(void)
{
  if (!readyque.head) /* ���Ĥ���ʤ��ä� */
    kz_sysdown();

  current = readyque.head; /* �����ȡ�����åɤ����ꤹ�� */
}

static void syscall_intr(void)
{
  syscall_proc(current->syscall.type, current->syscall.param);
}

static void softerr_intr(void)
{
  puts(current->name);
  puts(" DOWN.\n");
  getcurrent(); /* ��ǥ������塼���鳰�� */
  thread_exit(); /* ����åɽ�λ���� */
}

/* ����߽����������ؿ� */
static void thread_intr(softvec_type_t type, unsigned long sp)
{
  /* �����ȡ�����åɤΥ���ƥ����Ȥ���¸���� */
  current->context.sp = sp;

  /*
   * ����ߤ��Ȥν�����¹Ԥ��롥
   * SOFTVEC_TYPE_SYSCALL, SOFTVEC_TYPE_SOFTERR �ξ���
   * syscall_intr(), softerr_intr() ���ϥ�ɥ����Ͽ����Ƥ���Τǡ�
   * ����餬�¹Ԥ���롥
   */
  if (handlers[type])
    handlers[type]();

  schedule(); /* ����åɤΥ������塼��� */

  /*
   * ����åɤΥǥ����ѥå�
   * (dispatch()�ؿ������Τ�startup.s�ˤ��ꡤ������֥�ǵ��Ҥ���Ƥ���)
   */
  dispatch(&current->context);
  /* �����ˤ��֤äƤ��ʤ� */
}

void kz_start(kz_func_t func, char *name, int stacksize,
	      int argc, char *argv[])
{
  /*
   * �ʹߤǸƤӽФ�����åɴ�Ϣ�Υ饤�֥��ؿ��������� current ��
   * ���Ƥ����礬����Τǡ�current �� NULL �˽�������Ƥ�����
   */
  current = NULL;

  readyque.head = readyque.tail = NULL;
  memset(threads,  0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));

  /* ����ߥϥ�ɥ����Ͽ */
  setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr); /* �����ƥࡦ������ */
  setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr); /* �������װ�ȯ�� */

  /* �����ƥࡦ������ȯ���ԲĤʤΤ�ľ�ܴؿ���ƤӽФ��ƥ���åɺ������� */
  current = (kz_thread *)thread_run(func, name, stacksize, argc, argv);

  /* �ǽ�Υ���åɤ�ư */
  dispatch(&current->context);

  /* �����ˤ��֤äƤ��ʤ� */
}

void kz_sysdown(void)
{
  puts("system error!\n");
  while (1)
    ;
}

/* �����ƥࡦ������ƤӽФ��ѥ饤�֥��ؿ� */
void kz_syscall(kz_syscall_type_t type, kz_syscall_param_t *param)
{
  current->syscall.type  = type;
  current->syscall.param = param;
  asm volatile ("trapa #0"); /* �ȥ�å׳����ȯ�� */
}
