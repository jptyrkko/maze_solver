/************************************************************************** \
*
* LIBRARY       uniques
*
* DESCRIPTION   Operations on ordered sets of unique values
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         SEE uniques.h FOR DETAILS.
*
\**************************************************************************/

#include "../../include/uniques.h"



void insert_unique(olist_t **olist, int val)
{
  olist_t *p, *n;
  if (*olist == NULL) {
    *olist = (olist_t *) malloc(sizeof (olist_t));
    (*olist) -> next = NULL;
    (*olist) -> val = val;
    return;
  }
  if ((*olist) -> val > val) {
    n = (olist_t *) malloc(sizeof (olist_t));
    n -> next = (*olist);
    *olist = n;
    n -> val = val;
    return;
  }
  for (p = *olist; p; p = p -> next) {
    if (p -> val == val) return;
    if (p -> val < val) {
      if ((p -> next == NULL) || (p -> next -> val > val)) {
        n = (olist_t *) malloc(sizeof (olist_t));
        n -> next = p -> next;
        p -> next = n;
        n -> val = val;
        return;
      }
    }    
  }
}



void delete_unique(olist_t **olist, int val)
{
  olist_t *p, *n, *prev;
  if (*olist == NULL) return;
  if ((*olist) -> val == val) {
    if ((*olist) -> next == NULL) {
      free(*olist);
      *olist = NULL;
      return;
    }
    else {
      n = (*olist) -> next;
      free(*olist);
      *olist = n;
      return;
    }
  }
  for (prev = *olist, p = (*olist) -> next; p; prev = p, p = p -> next) {
    if (p -> val == val) {
      if (p -> next != NULL) prev -> next = p -> next;
      else prev -> next = NULL;
      free(p);
      return;
    }
  }
}



void prune_uniques(olist_t *p)   /* See header file notes */
{
  if (p == NULL) return;
  if (p -> next) prune_uniques(p -> next);
  free(p);
  return;
}



int first_unique(olist_t *olist) {return (olist -> val);}



void delete_first_unique(olist_t **olist)
{
  olist_t *p;
  if ((*olist) -> next == NULL) {
    free(*olist);
    *olist = NULL;
    return;
  }
  p = (*olist) -> next;
  free(*olist);
  *olist = p;
}



int last_unique(olist_t *olist)
{
  olist_t *p;
  for (p = olist; p -> next; p = p -> next);
  return (p -> val);
}



void delete_last_unique(olist_t **olist)
{
  olist_t *p, *prev;
  if ((*olist) -> next == NULL) {
    free(*olist);
    *olist = NULL;
    return;
  }
  for (prev = *olist, p = (*olist) -> next; p -> next; prev = p, p = p -> next) {
    free(p);
    prev -> next = NULL;
  }
}
