#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static void f1(int, int, int, int);
static void f2(void);

static jmp_buf jmpbuffer;
static int globval;

/* 全局变量： 作用域为整个进程
 * 自动变量： 默认为自动变量, auto 可以省略
 * 寄存器变量： 告诉编译器此变量经常使用, 可以放入寄存器中优化(是否如此不确定), 不能取地址
 * 易失变量： 随时可能改变的变量, 需要直接存取原始地址，不进行优化
 */
int main(int argc, char *argv[])
{
  int          autoval;
  register int regival;
  volatile int volaval;
  static int   statval;

  globval = 1; autoval = 2; regival = 3; volaval = 4; statval = 5;

  /*
   * gcc 使用 -O 选项进行优化编译后
   * auto 和 register 变量会变为 '2' '3', 而不是最后一次修改的 '96' '97'
   * 所以 auto 和 register 变量在使用 longjmp 后值不确定
   */
  if (setjmp(jmpbuffer) != 0) {
    printf("after longjmp:\n");
    printf("globval = %d, autoval = %d, regival = %d,"
           " volaval = %d, statval = %d\n",
           globval, autoval, regival, volaval, statval);
    exit(0);
  }

  /* change variables after setjmp, but before longjmp. */
  globval = 95; autoval = 96; regival = 97; volaval = 98; statval = 99;

  f1(autoval, regival, volaval, statval);
  return 0;
}

static void f1(int i, int j, int k, int l)
{
  printf("in f1():\n");
  printf("globval = %d, autoval = %d, regival = %d, "
         "volaval = %d, statval = %d\n",
         globval, i, j, k, l);
  f2();
}

static void f2(void)
{
  longjmp(jmpbuffer, 1);
}
