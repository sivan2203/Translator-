// #include "Translator.h"
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

struct Knots *createKnote(wchar_t *x, wchar_t *y, struct Knots *tree) {

  if (tree == NULL) {
    tree = (struct Knots *)malloc(sizeof(struct Knots));
    tree->untranslateWord = (wchar_t *)calloc(30, sizeof(wchar_t));
    tree->translateWord = (wchar_t *)calloc(30, sizeof(wchar_t));
    wcsncpy(tree->untranslateWord, x, 30);
    wcsncpy(tree->translateWord, y, 30);
    tree->Left = NULL;
    tree->Right = NULL;
  } else if (wcscmp(x, tree->untranslateWord) < 0) {
    tree->Left = createKnote(x, y, tree->Left);
  } else {
    tree->Right = createKnote(x, y, tree->Right);
  }
  return tree;
}

void printDict(struct Knots *tree) {
  if (tree != NULL) {
    printDict(tree->Left);
    wprintf(L"%ls - %ls\n", tree->untranslateWord, tree->translateWord);
    printDict(tree->Right);
  }
}

struct Knots *scanDict(FILE *in, struct Knots *Root) {
  wchar_t c;
  int i = 0, space = 0, dash = 1;
  wchar_t *untranslateWord, *translateWord;
  untranslateWord = (wchar_t *)calloc(30, sizeof(wchar_t));
  translateWord = (wchar_t *)calloc(30, sizeof(wchar_t));
  while ((c = fgetwc(in)) != ((wchar_t)WEOF)) {

    if (c != L' ' && c != L'-' && space != 1) {

      untranslateWord[i] = (wchar_t)c;

      i++;
      dash = 1;
    } else if ((c == L' ') || (c == L'-')) {

      space = 1;
      i = 0;
      dash = 0;
    }
    if (c != L' ' && c != L'-' && dash != 1 && c != L'\n') {
      translateWord[i] = (wchar_t)c;
      i++;
      space = 1;
    } else if (c == L'\n' || c == ((wchar_t)WEOF)) {

      Root = createKnote(untranslateWord, translateWord, Root);
      free(untranslateWord);
      free(translateWord);
      untranslateWord = (wchar_t *)calloc(30, sizeof(wchar_t));
      translateWord = (wchar_t *)calloc(30, sizeof(wchar_t));
      i = 0;
      space = 0;
      dash = 1;
    }
  }

  fclose(in);
  free(untranslateWord);
  free(translateWord);
  printf("Словарь успешно обновлён!\n");
  return Root;
}

int rusUpp(int ch) {
  if (1071 < ch && ch < 1104) {

    return ch - 32;
  } else {
    return ch;
  }
}

int engUpp(int ch) {
  if (96 < ch && ch < 123) {

    return ch - 32;
  } else {
    return ch;
  }
}
int engLow(int ch) {
  if (64 < ch && ch < 91) {

    return ch + 32;
  } else {
    return ch;
  }
}

int comparison(wchar_t *a, wchar_t *b) {
  if (rusUpp(a[0]) < rusUpp(b[0])) {
    return -1;
  }
  if (rusUpp(a[0]) > rusUpp(b[0])) {
    return 1;
  }
  if (rusUpp(a[0]) == rusUpp(b[0])) {
    for (int i = 1; a[i] != L'\0' || b[i] != L'\0'; i++) {
      if (a[i] < b[i])
        return -1;
      else if (a[i] > b[i])
        return 1;
    }
    return 0;
  }
  return 0;
}

int moveKnots(FILE *out, struct Knots *p, wchar_t *untransWord, wchar_t c) {

  while (p != NULL) {

    if (comparison(untransWord, p->untranslateWord) < 0) {

      p = p->Left;

    } else if (comparison(untransWord, p->untranslateWord) > 0) {
      p = p->Right;
    } else {
      wchar_t *transWord = (wchar_t *)calloc(30, sizeof(wchar_t));
      wcsncpy(transWord, p->translateWord, 30);

      if (((1071 < untransWord[0] && untransWord[0] < 1104) &&
           (96 < transWord[0] && transWord[0] < 123)) ||
          ((1039 < untransWord[0] && untransWord[0] < 1072) &&
           (64 < transWord[0] && transWord[0] < 91))) {

        fwprintf(out, L"%ls%lc", transWord, c);
      } else if ((1071 < untransWord[0] && untransWord[0] < 1104) &&
                 (64 < transWord[0] && transWord[0] < 91)) {
        transWord[0] = engLow(transWord[0]);
        fwprintf(out, L"%ls%lc", transWord, c);
      } else if ((1039 < untransWord[0] && untransWord[0] < 1072) &&
                 (96 < transWord[0] && transWord[0] < 123)) {
        transWord[0] = engUpp(transWord[0]);

        fwprintf(out, L"%ls%lc", transWord, c);
      }
      free(transWord);
      return 1;
    }
  }
  fwprintf(out, L"%ls%lc", untransWord, c);
  return 0;
}

void Transfer(FILE *in, FILE *out, struct Knots *Root) {
  wint_t c;

  wchar_t *word;
  word = (wchar_t *)calloc(30, sizeof(wchar_t));
  int i = 0;
  while ((c = fgetwc(in)) != EOF) {

    if (wcschr(L".?, -()[]\n", c)) {

      if (wcscmp(L"", word) == 0) {

        fwprintf(out, L"%lc", (wchar_t)c);
      } else {
        moveKnots(out, Root, word, (wchar_t)c);

        free(word);
        word = (wchar_t *)calloc(30, sizeof(wchar_t));
        i = 0;
      }
    } else {
      word[i] = (wchar_t)c;
      i++;
    }
  }
  printf("Текст успешно переведён!\n");
}

void printMenu() {
  printf("+------------------------+\n");
  printf("|     Что вы хотите?     |\n");
  printf("|  1) Перевести текст    |\n");
  printf("|  2) Показать словарь   |\n");
  printf("|  3) Обновить словарь   |\n");
  printf("|  4) Выход              |\n");
  printf("+------------------------+\n");
}
int get_variant() {
  int variant;
  scanf("%d", &variant);
  return variant;
}

int main(int argc, char *argv[]) {
  setlocale(LC_CTYPE, "");

  int variant = 0;
  FILE *dir, *in, *out;

  dir = fopen(argv[2], "rt");
  if (dir == NULL) {
    printf("Ошибка! Не удалось открыть файл-словарь!\n");
    return -1;
  }

  in = fopen(argv[1], "rt");
  if (in == NULL) {
    printf("Ошибка! Не удалось открыть входной файл!");
    return 0;
  }
  out = fopen(argv[3], "wt");

  struct Knots *Root;
  Root = NULL;
  Root = scanDict(dir, Root);

  do {
    system("clear");
    printMenu();
    variant = get_variant();
    switch (variant) {
    case 1:
      system("clear");
      in = fopen(argv[1], "rt");
      out = fopen(argv[3], "wt");
      Transfer(in, out, Root);
      fclose(in);
      fclose(out);
      break;
    case 2:
      system("clear");
      printDict(Root);
      break;
    case 3:
      system("clear");
      dir = fopen(argv[2], "rt");
      free(Root);
      Root = NULL;
      Root = scanDict(dir, Root);
      fclose(dir);
      break;
    }
    variant = get_variant();

  } while (variant != 4);

  return 0;
}
