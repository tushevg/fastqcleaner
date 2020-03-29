#include "tests.h"

int run_tests(void)
{
    int result = 0;
    result += test_bs_packseq();
    result += test_bs_bitset();
    return result;
}


int test_bs_packseq(void)
{
    int result = 0;
    const char *dna = "ACGT";
    bitseq_t *bs = bs_pack(dna);
    result = (bs->bitseq[0] == 30) ? 1 : 0;
    bs_print(bs);
    printf("bit%d = %d\n", 3, bs_exist(bs, 3));

    bs_clear(bs, 3);
    bs_print(bs);
    printf("bit%d = %d\n", 3, bs_exist(bs, 3));

    bs_set(bs, 3);
    bs_print(bs);
    printf("bit%d = %d\n", 3, bs_exist(bs, 3));

    bs_destroy(bs);
    return result;
}


int test_bs_bitset(void)
{
    int result = 0;
    return result;
}


/*
khint_t k;
k = kh_put(type, h, 55, &absent);
if (absent) {
  myval_t *cl1 = &kh_val(h, k);
  cl1->id = 55;
  cl1->len = 3;
  cl1->stream = strdup("abc");
}


#include "khash.h"
typedef struct {
  int x, y;
} myval_t;
KHASH_MAP_INIT_STR(str, myval_t)
int main(void) {
  khash_t(str) *h;
  khint_t k;
  int absent;
  h = kh_init(str);
  k = kh_put(str, h, "foo", &absent);
  kh_val(h, k).x = 1;
  kh_val(h, k).y = 2;
  k = kh_get(str, h, "foo");
  if (k != kh_end(h))
    printf("%d,%d\n", kh_val(h,k).x, kh_val(h,k).y);
  // if keys are allocated on heap, free them before calling kh_destroy()
  kh_destroy(str, h);
  return 0;
}

 */
