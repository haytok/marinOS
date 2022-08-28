#include "defines.h"
#include "kozos.h"
#include "intr.h"
#include "interrupt.h"
#include "syscall.h"
#include "memory.h"
#include "lib.h"

#define THREAD_NUM 6
#define PRIORITY_NUM 16
#define THREAD_NAME_SIZE 15

/* ����åɡ�����ƥ����� */
typedef struct _kz_context {
  uint32 sp; /* �����å����ݥ��� */
} kz_context;

/* ������������ȥ��롦�֥�å�(TCB) */
typedef struct _kz_thread {
  struct _kz_thread *next;
  char name[THREAD_NAME_SIZE + 1]; /* ����å�̾ */
  int priority;   /* ͥ���� */
  char *stack;    /* �����å� */
  uint32 flags;   /* �Ƽ�ե饰 */
#define KZ_THREAD_FLAG_READY (1 << 0)

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

/* ��å��������Хåե� */
typedef struct _kz_msgbuf {
  struct _kz_msgbuf *next;
  kz_thread *sender; /* ��å�������������������å� */
  struct { /* ��å������Υѥ�᡼����¸�ΰ� */
    int size;
    char *p;
  } param;
} kz_msgbuf;

/* ��å��������ܥå��� */
typedef struct _kz_msgbox {
  kz_thread *receiver; /* �����Ԥ����֤Υ���å� */
  kz_msgbuf *head;
  kz_msgbuf *tail;

  /*
   * H8��16�ӥå�CPU�ʤΤǡ�32�ӥå��������Ф��Ƥξ軻̿�᤬̵������ä�
   * ��¤�ΤΥ������������߾�ˤʤäƤ��ʤ��ȡ���¤�Τ�����Υ���ǥå���
   * �׻��Ǿ軻���Ȥ��ơ�___mulsi3��̵���פʤɤΥ�󥯡����顼�ˤʤ��礬
   * ���롥(�����߾�ʤ�Х��եȱ黻�����Ѥ����Τ�����ϽФʤ�)
   * �к��Ȥ��ơ��������������߾�ˤʤ�褦�˥��ߡ������Ф�Ĵ�����롥
   * ¾��¤�Τ�Ʊ�ͤΥ��顼���Ф����ˤϡ�Ʊ�ͤ��н�򤹤뤳�ȡ�
   */
  long dummy[1];
} kz_msgbox;

/* ����åɤΥ�ǥ��������塼 */
static struct {
  kz_thread *head;
  kz_thread *tail;
} readyque[PRIORITY_NUM];

static kz_thread *current; /* �����ȡ�����å� */
static kz_thread threads[THREAD_NUM]; /* ������������ȥ��롦�֥�å� */
static kz_handler_t handlers[SOFTVEC_TYPE_NUM]; /* ����ߥϥ�ɥ� */
static kz_msgbox msgboxes[MSGBOX_ID_NUM]; /* ��å��������ܥå��� */

void dispatch(kz_context *context);

/* �����ȡ�����åɤ��ǥ��������塼����ȴ���Ф� */
static int getcurrent(void)
{
  if (current == NULL) {
    return -1;
  }
  if (!(current->flags & KZ_THREAD_FLAG_READY)) {
    /* ���Ǥ�̵������̵�� */
    return 1;
  }

  /* �����ȡ�����åɤ�ɬ����Ƭ�ˤ���Ϥ��ʤΤǡ���Ƭ����ȴ���Ф� */
  readyque[current->priority].head = current->next;
  if (readyque[current->priority].head == NULL) {
    readyque[current->priority].tail = NULL;
  }
  current->flags &= ~KZ_THREAD_FLAG_READY;
  current->next = NULL;

  return 0;
}

/* �����ȡ�����åɤ��ǥ��������塼�˷Ҥ��� */
static int putcurrent(void)
{
  if (current == NULL) {
    return -1;
  }
  if (current->flags & KZ_THREAD_FLAG_READY) {
    /* ���Ǥ�ͭ�����̵�� */
    return 1;
  }

  /* ��ǥ��������塼����������³���� */
  if (readyque[current->priority].tail) {
    readyque[current->priority].tail->next = current;
  } else {
    readyque[current->priority].head = current;
  }
  readyque[current->priority].tail = current;
  current->flags |= KZ_THREAD_FLAG_READY;

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
static kz_thread_id_t thread_run(kz_func_t func, char *name, int priority,
				 int stacksize, int argc, char *argv[])
{
  int i;
  kz_thread *thp;
  uint32 *sp;
  extern char userstack; /* ��󥫡�������ץȤ��������륹���å��ΰ� */
  static char *thread_stack = &userstack;

  /* �����Ƥ��륿����������ȥ��롦�֥�å��򸡺� */
  for (i = 0; i < THREAD_NUM; i++) {
    thp = &threads[i];
    if (!thp->init.func) /* ���Ĥ��ä� */
      break;
  }
  if (i == THREAD_NUM) /* ���Ĥ���ʤ��ä� */
    return -1;

  memset(thp, 0, sizeof(*thp));

  /* ������������ȥ��롦�֥�å�(TCB)������ */
  strcpy(thp->name, name);
  thp->next     = NULL;
  thp->priority = priority;
  thp->flags    = 0;

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
   * �ץ���ࡦ�����󥿤����ꤹ�롥
   * ����åɤ�ͥ���٤�����ξ��ˤϡ�����߶ػߥ���åɤȤ��롥
   */
  *(--sp) = (uint32)thread_init | ((uint32)(priority ? 0 : 0xc0) << 24);

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

/* �����ƥࡦ������ν���(kz_wait():����åɤμ¹Ը�����) */
static int thread_wait(void)
{
  putcurrent();
  return 0;
}

/* �����ƥࡦ������ν���(kz_sleep():����åɤΥ��꡼��) */
static int thread_sleep(void)
{
  return 0;
}

/* �����ƥࡦ������ν���(kz_wakeup():����åɤΥ������������å�) */
static int thread_wakeup(kz_thread_id_t id)
{
  /* �������������åפ�ƤӽФ�������åɤ��ǥ��������塼���᤹ */
  putcurrent();

  /* ���ꤵ�줿����åɤ��ǥ��������塼����³���ƥ������������åפ��� */
  current = (kz_thread *)id;
  putcurrent();

  return 0;
}

/* �����ƥࡦ������ν���(kz_getid():����å�ID����) */
static kz_thread_id_t thread_getid(void)
{
  putcurrent();
  return (kz_thread_id_t)current;
}

/* �����ƥࡦ������ν���(kz_chpri():����åɤ�ͥ�����ѹ�) */
static int thread_chpri(int priority)
{
  int old = current->priority;
  if (priority >= 0)
    current->priority = priority; /* ͥ�����ѹ� */
  putcurrent(); /* ������ͥ���٤Υ�ǥ��������塼�˷Ҥ�ľ�� */
  return old;
}

/* �����ƥࡦ������ν���(kz_kmalloc():ưŪ�������) */
static void *thread_kmalloc(int size)
{
  putcurrent();
  return kzmem_alloc(size);
}

/* �����ƥࡦ������ν���(kz_kfree():�������) */
static int thread_kmfree(char *p)
{
  kzmem_free(p);
  putcurrent();
  return 0;
}

/* ��å��������������� */
static void sendmsg(kz_msgbox *mboxp, kz_thread *thp, int size, char *p)
{
  kz_msgbuf *mp;

  /* ��å��������Хåե��κ��� */
  mp = (kz_msgbuf *)kzmem_alloc(sizeof(*mp));
  if (mp == NULL)
    kz_sysdown();
  mp->next       = NULL;
  mp->sender     = thp;
  mp->param.size = size;
  mp->param.p    = p;

  /* ��å��������ܥå����������˥�å���������³���� */
  if (mboxp->tail) {
    mboxp->tail->next = mp;
  } else {
    mboxp->head = mp;
  }
  mboxp->tail = mp;
}

/* ��å������μ������� */
static void recvmsg(kz_msgbox *mboxp)
{
  kz_msgbuf *mp;
  kz_syscall_param_t *p;

  /* ��å��������ܥå�������Ƭ�ˤ����å�������ȴ���Ф� */
  mp = mboxp->head;
  mboxp->head = mp->next;
  if (mboxp->head == NULL)
    mboxp->tail = NULL;
  mp->next = NULL;

  /* ��å�������������륹��åɤ��֤��ͤ����ꤹ�� */
  p = mboxp->receiver->syscall.param;
  p->un.recv.ret = (kz_thread_id_t)mp->sender;
  if (p->un.recv.sizep)
    *(p->un.recv.sizep) = mp->param.size;
  if (p->un.recv.pp)
    *(p->un.recv.pp) = mp->param.p;

  /* �����Ԥ�����åɤϤ��ʤ��ʤä��Τǡ�NULL���᤹ */
  mboxp->receiver = NULL;

  /* ��å��������Хåե��β��� */
  kzmem_free(mp);
}

/* �����ƥࡦ������ν���(kz_send():��å���������) */
static int thread_send(kz_msgbox_id_t id, int size, char *p)
{
  kz_msgbox *mboxp = &msgboxes[id];

  putcurrent();
  sendmsg(mboxp, current, size, p); /* ��å��������������� */

  /* �����Ԥ�����åɤ�¸�ߤ��Ƥ�����ˤϼ���������Ԥ� */
  if (mboxp->receiver) {
    current = mboxp->receiver; /* �����Ԥ�����å� */
    recvmsg(mboxp); /* ��å������μ������� */
    putcurrent(); /* �����ˤ��ư���ǽ�ˤʤä��Τǡ��֥�å�������� */
  }

  return size;
}

/* �����ƥࡦ������ν���(kz_recv():��å���������) */
static kz_thread_id_t thread_recv(kz_msgbox_id_t id, int *sizep, char **pp)
{
  kz_msgbox *mboxp = &msgboxes[id];

  if (mboxp->receiver) /* ¾�Υ���åɤ����Ǥ˼����Ԥ����Ƥ��� */
    kz_sysdown();

  mboxp->receiver = current; /* �����Ԥ�����åɤ����� */

  if (mboxp->head == NULL) {
    /*
     * ��å��������ܥå����˥�å�������̵���Τǡ�����åɤ�
     * ���꡼�פ����롥(�����ƥࡦ�����뤬�֥�å�����)
     */
    return -1;
  }

  recvmsg(mboxp); /* ��å������μ������� */
  putcurrent(); /* ��å�����������Ǥ����Τǡ���ǥ������֤ˤ��� */

  return current->syscall.param->un.recv.ret;
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
  /* �����ƥࡦ������μ¹����current���񤭴����Τ���� */
  switch (type) {
  case KZ_SYSCALL_TYPE_RUN: /* kz_run() */
    p->un.run.ret = thread_run(p->un.run.func, p->un.run.name,
			       p->un.run.priority, p->un.run.stacksize,
			       p->un.run.argc, p->un.run.argv);
    break;
  case KZ_SYSCALL_TYPE_EXIT: /* kz_exit() */
    /* TCB���õ���Τǡ�����ͤ�񤭹���ǤϤ����ʤ� */
    thread_exit();
    break;
  case KZ_SYSCALL_TYPE_WAIT: /* kz_wait() */
    p->un.wait.ret = thread_wait();
    break;
  case KZ_SYSCALL_TYPE_SLEEP: /* kz_sleep() */
    p->un.sleep.ret = thread_sleep();
    break;
  case KZ_SYSCALL_TYPE_WAKEUP: /* kz_wakeup() */
    p->un.wakeup.ret = thread_wakeup(p->un.wakeup.id);
    break;
  case KZ_SYSCALL_TYPE_GETID: /* kz_getid() */
    p->un.getid.ret = thread_getid();
    break;
  case KZ_SYSCALL_TYPE_CHPRI: /* kz_chpri() */
    p->un.chpri.ret = thread_chpri(p->un.chpri.priority);
    break;
  case KZ_SYSCALL_TYPE_KMALLOC: /* kz_kmalloc() */
    p->un.kmalloc.ret = thread_kmalloc(p->un.kmalloc.size);
    break;
  case KZ_SYSCALL_TYPE_KMFREE: /* kz_kmfree() */
    p->un.kmfree.ret = thread_kmfree(p->un.kmfree.p);
    break;
  case KZ_SYSCALL_TYPE_SEND: /* kz_send() */
    p->un.send.ret = thread_send(p->un.send.id,
				 p->un.send.size, p->un.send.p);
    break;
  case KZ_SYSCALL_TYPE_RECV: /* kz_recv() */
    p->un.recv.ret = thread_recv(p->un.recv.id,
				 p->un.recv.sizep, p->un.recv.pp);
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
  int i;

  /*
   * ͥ���̤ι⤤��(ͥ���٤ο��ͤξ�������)�˥�ǥ��������塼�򸫤ơ�
   * ư���ǽ�ʥ���åɤ򸡺����롥
   */
  for (i = 0; i < PRIORITY_NUM; i++) {
    if (readyque[i].head) /* ���Ĥ��ä� */
      break;
  }
  if (i == PRIORITY_NUM) /* ���Ĥ���ʤ��ä� */
    kz_sysdown();

  current = readyque[i].head; /* �����ȡ�����åɤ����ꤹ�� */
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

void kz_start(kz_func_t func, char *name, int priority, int stacksize,
	      int argc, char *argv[])
{
  kzmem_init(); /* ưŪ����ν���� */

  /*
   * �ʹߤǸƤӽФ�����åɴ�Ϣ�Υ饤�֥��ؿ��������� current ��
   * ���Ƥ����礬����Τǡ�current �� NULL �˽�������Ƥ�����
   */
  current = NULL;

  memset(readyque, 0, sizeof(readyque));
  memset(threads,  0, sizeof(threads));
  memset(handlers, 0, sizeof(handlers));
  memset(msgboxes, 0, sizeof(msgboxes));

  /* ����ߥϥ�ɥ����Ͽ */
  setintr(SOFTVEC_TYPE_SYSCALL, syscall_intr); /* �����ƥࡦ������ */
  setintr(SOFTVEC_TYPE_SOFTERR, softerr_intr); /* �������װ�ȯ�� */

  /* �����ƥࡦ������ȯ���ԲĤʤΤ�ľ�ܴؿ���ƤӽФ��ƥ���åɺ������� */
  current = (kz_thread *)thread_run(func, name, priority, stacksize,
				    argc, argv);

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
