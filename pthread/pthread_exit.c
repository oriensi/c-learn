#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <apue.h>

/*
 * void pthread_cleanup_push(void (*rtn) (void *), void *arg);
 * void pthread_cleanup_pop(int execute);
 *
 * 当线程执行以下动作时，清理函数 rtn 是由 pthread_cleanup_push 函数调度的，调用时只有一个参数 arg
 *	调用 pthread_exit 时;
 *	响应取消请求时;
 *	用非零 execute 参数调用 pthread_cleanup_pop 时.
 * 三个条件为或的关系, 满足任何一个即调用清理函数
 */
void cleanup(void *arg)
{
  printf("cleanup: %s\n", (char*)arg);
}

void *thr_fn1(void *arg)
{
  printf("thread 1 start\n");
  pthread_cleanup_push(cleanup, "thread 1 first handler");
  pthread_cleanup_push(cleanup, "thread 1 second handler");
  printf("thread 1 push complete\n");
  if (arg)
    return ((void*)1);
  pthread_cleanup_pop(0); // 0 not exec cleanup
  pthread_cleanup_pop(1); // !0 exec cleanup
}

void *thr_fn2(void *arg)
{
  printf("thread 2 start\n");
  pthread_cleanup_push(cleanup, "thread 2 first handler");
  pthread_cleanup_push(cleanup, "thread 2 second handler");
  printf("thread 2 push complete\n");
  if (arg)
    pthread_exit((void*)0);
  pthread_cleanup_pop(0);  // pthread_exit exec cleanup
  pthread_cleanup_pop(1);  // pthread_exit exec cleanup
  pthread_exit((void*)3);
}

int main(int argc, char *argv[])
{
  int err;
  pthread_t tid1, tid2;
  void *tret;

  err = pthread_create(&tid1, NULL, thr_fn1, NULL);
  if (err != 0)
    err_exit(err, "can't create thread 1");

  err = pthread_create(&tid2, NULL, thr_fn2, (void*)1);
  if (err != 0)
    err_exit(err, "can't create thread 2");

  err = pthread_join(tid1, &tret);  // 等待 tid1 执行结束
  if (err != 0)
    err_exit(err, "can't join with thread 1");
  printf("thread 1 exit code %ld\n", (long)tret);

  err = pthread_join(tid2, &tret);
  if (err != 0)
    err_exit(err, "can't join with thread 2");
  printf("thread 2 exit code %ld\n", (long)tret);
  exit(0);
}
