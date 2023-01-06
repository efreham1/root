#include "general_TUI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void clear_input_buffer ()
{
  int c;
  do
    {
      c = getchar ();
    }
  while (c != '\n' && c != EOF);
}

int read_string (char *buf, int buf_siz)
{
  int c;
  int i = -1;

  do
    {
      if (i < buf_siz - 1)
        {
          i += 1;
        }
      else
        {
          buf[i] = '\0';
          clear_input_buffer ();
          return i;
        }
      c = getchar ();
      buf[i] = c;
    }
  while (c != '\n');
  buf[i] = '\0';
  return i;
}

bool not_empty (char *str)
{
  return strlen (str) > 0;
}

bool not_empty_ (char *str, void *extra, heap_t *h)
{
  return not_empty (str);
}

bool is_number (char *str)
{
  if (strlen (str) == 0)
    {
      return false;
    }
  for (int i = 0; i < strlen (str); ++i)
    {
      if (str[i] == 45 && strlen (str) > 1)
        {
          if (i != 0)
            {
              return false;
            }
        }
      else if (!isdigit (str[i]))
        {
          return false;
        }
    }
  return true;
}

bool is_number_ (char *str, void *extra, heap_t *h)
{
  return is_number (str);
}

bool is_pos_number (char *str)
{
  if (strlen (str) == 0)
    {
      return false;
    }
  for (int i = 0; i < strlen (str); ++i)
    {
      if (str[i] == 45 && strlen (str) > 1)
        {
          if (i != 0)
            {
              return false;
            }
        }
      else if (!isdigit (str[i]))
        {
          return false;
        }
    }
  if (atoi (str) < 0)
    {
      return false;
    }

  return true;
}

bool is_pos_number_ (char *str, void *extra, heap_t *h)
{
  return is_pos_number (str);
}

answer_t
ask_question (char *question, check_func check, void *check_extra, convert_func convert, void *convert_extra, heap_t *h)
{
  int buf_siz = 255;
  char buf[buf_siz];
  printf ("%s\n", question);
  read_string (buf, buf_siz);
  while (!check (buf, check_extra, h))
    {
      printf ("Answer does not have correct format.\n");
      printf ("%s\n", question);
      read_string (buf, buf_siz);
    }
  return convert (buf, convert_extra, h);
}

answer_t str_to_int (char *str, void *extra, heap_t *h)
{
  return (answer_t) {.int_t = atoi (str)};
}

answer_t str_to_str (char *str, void *extra, heap_t *h)
{
  int len = strlen (str) + 1;
  char *str_cpy = h_alloc_data (h, len);
  memcpy (str_cpy, str, len);
  return (answer_t) str_cpy;
}

int ask_question_int (char *question, heap_t *h)
{
  return ask_question (question, is_number_, NULL, str_to_int, NULL, h).int_t;
}

char *ask_question_string (char *question, heap_t *h)
{
  return ask_question (question, not_empty_, NULL, str_to_str, NULL, h).str_t;
}

int ask_question_u_int (char *question, heap_t *h)
{
  return ask_question (question, is_pos_number_, NULL, str_to_int, NULL, h).int_t;
}