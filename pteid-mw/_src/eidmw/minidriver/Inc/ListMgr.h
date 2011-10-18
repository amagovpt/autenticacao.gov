/* ****************************************************************************

 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.

**************************************************************************** */
#ifndef __LISTMGR__H__
#define __LISTMGR__H__

/********************/
/* Constants        */
/********************/

#define LAST                     -1                                    /* last item in list    */
#define CURR_POS(h_ptr)          ((h_ptr)->curr_pos)         /* current pos in list  */
#define CURR_PTR(h_ptr)          ((h_ptr)->curr_ptr)         /* current ptr in list  */
#define ITEM_CNT(h_ptr)          ((h_ptr)->item_count)       /* nbr of items in list */
#define LIST_EMPTY(h_ptr)        ((h_ptr)->item_count==0)
#define LIST_NOT_EMPTY(h_ptr)    ((h_ptr)->item_count>0)

typedef enum bool { false=0, true=1 }; 

#define LM_OK                    0
#define LM_NOK                   -1

/********************/
/* Type definitions */
/********************/

/*
+-------------------------------------------------------------------
| head_type is used as an overlay for the list list head.
| When used in the head, next_ptr points to the first and prev_ptr
| points to the last item.
+-------------------------------------------------------------------
*/

typedef int    boolean_type;

typedef struct head_type
{
   struct head_type  *next_ptr ;
   struct head_type  *prev_ptr ;
   struct head_type  *curr_ptr ;
   int                curr_pos  ;
   int                item_count ;
} head_type ;


typedef head_type    *item_link_type [2] ;

/*
+-------------------------------------------------------------------
| Item_type is the universel type used for an item in the linked
| list.  All casting of appl_item_type to the list_mngr should
| use item_type.
+-------------------------------------------------------------------
*/

typedef struct item_type
{
   struct head_type  *next_ptr ;
   struct head_type  *prev_ptr ;
} item_type ;

/************************/
/* Prototypes functions */
/************************/      

extern int Init_list 
   (
      head_type      *head_ptr
   );

extern int Prev_item_in_list
   (
      head_type      *head_ptr
   );

extern int Next_item_in_list
   (
      head_type      *head_ptr
   );

extern int Goto_item_in_list
   (
      head_type      *head_ptr,
      int             new_pos
   );

//extern boolean_type Insert_item_in_list 
//   (
//      head_type       *head_ptr,
//      head_type       *new_ptr,
//      boolean_type    (*insert_f) ()
//   );

extern int Append_item_in_list
   (
      head_type  *head_ptr,
      head_type  *new_ptr
   );

extern head_type *Remove_item_from_list
   (
      head_type  *head_ptr
   );

extern char *Allocate_memory_for_list
   (
   int iMemSize
   ); 

extern int Destroy_list
   (
      head_type *head_ptr
   );

//extern int Move_list
//   (
//      head_type      *source_head_ptr,
//      head_type      *target_head_ptr
//   );


/*extern boolean_type Find_item_in_list
   (
      head_type  *head_ptr,
      char       *key_ptr,
      char       *(*field_f) ()
   );
*/
//extern int Merge_lists
//   (
//      head_type    *first_list_head_ptr ,
//      head_type    *second_list_head_ptr
//   );

/*extern int Sort_list
   (
      head_type   *head_ptr,
      int          (*sort_f)()
   );
  */       
/************/
/* Clean up */
/************/

#endif /* __LISTMGR__H__ */