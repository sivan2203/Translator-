#ifndef TRANSLATOR_H
#define TRANSLATOR_H
#include <ctype.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

struct Knots {
  wchar_t *untranslateWord;
  wchar_t *translateWord;
  struct Knots *Left;
  struct Knots *Right;
};

struct Knots *createKnote(wchar_t *x, wchar_t *y, struct Knots *tree);
struct Knots *scanDict(FILE *in, struct Knots *Root);
int rusUpp(int ch);
int engUpp(int ch);
int engLow(int ch);
int comparison(wchar_t *a, wchar_t *b);
int moveKnots(FILE *out, struct Knots *p, wchar_t *untransWord, wchar_t c);
void Transfer(FILE *in, FILE *out, struct Knots *Root);
void printMenu();
int get_variant();
#endif
