/************************************************************************** \
*
* LIBRARY       uniques
*
* DESCRIPTION   Operations on ordered sets of unique values
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

#ifndef _UNIQUE_H
#define _UNIQUE_H

#include <stddef.h>
#include <stdlib.h>

typedef struct olist_s {   /* Element of a set of unique values */
  struct olist_s *next;
  int     val;
} olist_t;



/************************************************************************** \
*
* FUNCTION      insert_unique
*
* DESCRIPTION   Inserts an unique value to the set.
*
* ARGUMENTS     olist   Set root
*               val     Value to insert
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void insert_unique(olist_t **olist, int val);



/************************************************************************** \
*
* FUNCTION      delete_unique
*
* DESCRIPTION   Finds and deletes an unique value from the set.
*
* ARGUMENTS     olist   Set root
*               val     Value to delete
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void delete_unique(olist_t **olist, int val);


/************************************************************************** \
*
* FUNCTION      prune_uniques
*
* DESCRIPTION   Deletes the set forward from a given node.
*
* ARGUMENTS     p   Deletion start node
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         Can be used to delete all of the list if called at the root.
*
*               IMPORTANT: Setting the "next" of a last node is left to
*               the caller.
*
\**************************************************************************/

void prune_uniques(olist_t *p);



/************************************************************************** \
*
* FUNCTION      first_unique
*
* DESCRIPTION   Return the value of the first set element.
*
* ARGUMENTS     olist   Set root
*
* GLOBALS       -
*
* RETURNS       Node value.
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

int first_unique(olist_t *olist);



/************************************************************************** \
*
* FUNCTION      delete_first_unique
*
* DESCRIPTION   Deletes the first node from the set.
*
* ARGUMENTS     olist   Set root
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void delete_first_unique(olist_t **olist);



/************************************************************************** \
*
* FUNCTION      last_unique
*
* DESCRIPTION   Return the value of the last set element.
*
* ARGUMENTS     olist   Set root
*
* GLOBALS       -
*
* RETURNS       Node value.
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

int last_unique(olist_t *olist);



/************************************************************************** \
*
* FUNCTION      delete_last_unique
*
* DESCRIPTION   Deletes the last node from the set.
*
* ARGUMENTS     olist   Set root
*
* GLOBALS       -
*
* RETURNS       -
*
* HISTORY       2023 01 01   JPT   First implementation
*
* NOTES         -
*
\**************************************************************************/

void delete_last_unique(olist_t **olist);

#endif
