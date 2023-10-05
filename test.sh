#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 '0;'
assert 42 '42;'
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10+20;'
assert 0 '+0;'
assert 0 '-0;'
assert 1 '1==1;'
assert 0 '1==2;'
assert 1 '1!=2;'
assert 0 '1!=1;'
assert 1 '1<2;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '1<=2;'
assert 1 '1<=1;'
assert 0 '2<=1;'
assert 1 '2>1;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '2>=1;'
assert 1 '1>=1;'
assert 0 '1>=2;'
assert 10 'a=10;'
assert 20 'a=15; b=5; a+b;'
assert 6 'foo = 1; bar = 2 + 3; foo + bar;'
assert 1 '_ = 1; _;'
assert 1 '_123 = 1; _123;'
assert 1 '_123abc = 1; _123abc;'
assert 1 'ABC123 = 1; ABC123;'
assert 1 'return 1;'
assert 1 'return 1; return 2;'
assert 2 'a = 2; return a;'
assert 3 'a = 1; b = 2; return a + b;'
assert 1 'if (1) return 1; return 2;'
assert 2 'if (0) return 1; return 2;'
assert 3 'if (0) return 1; else return 3;'
assert 10 'i = 0; while (i < 10) i = i + 1; return i;'
assert 55 'j = 0; for (i = 0; i <= 10; i = i + 1) j = j + i; return j;'
assert 3 'i = 0; for (; i < 10; i = i + 1) if (i == 3) return i;'
assert 3 'for (;i < 10;) if (i == 3) return i; else i = i + 1;'
assert 3 'for (;;) if (i == 3) return i; else i = i + 1;'
assert 1 'if (1) if (1) if (1) if (1) if (1) if (1) if (1) if (1) if (1) if (1) if (1) return 1;'
assert 1 'if (1); return 1;'
assert 10 'for (i = 0; i < 10; i = i + 1); return i;'
assert 1 '{ return 1; }'
assert 1 '{ i = 1; return i; }'
assert 1 'for (;;) return 1;'
assert 1 'for (;;) if (1) return 1;'
assert 1 'for (;;) { if (1) return 1; }'
assert 1 'for (;;) { i = i + 1; if (i == 1) return i; }'
assert 2 'for (;;) { i = i + 1; if (i == 2) return i; }'
assert 5 'for (;;) { i = i + 1; if (i == 5) return i; }'
assert 10 'for (;;) { i = i + 1; if (i == 10) return i; }'
assert 10 'while (1) { i = i + 1; if (i == 10) return i; }'

echo OK
