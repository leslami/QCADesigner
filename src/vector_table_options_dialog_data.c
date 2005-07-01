//////////////////////////////////////////////////////////
// QCADesigner                                          //
// Copyright 2002 Konrad Walus                          //
// All Rights Reserved                                  //
// Author: Konrad Walus                                 //
// Email: qcadesigner@gmail.com                         //
// WEB: http://qcadesigner.ca/                          //
//////////////////////////////////////////////////////////
//******************************************************//
//*********** PLEASE DO NOT REFORMAT THIS CODE *********//
//******************************************************//
// If your editor wraps long lines disable it or don't  //
// save the core files that way. Any independent files  //
// you generate format as you wish.                     //
//////////////////////////////////////////////////////////
// Please use complete names in variables and fucntions //
// This will reduce ramp up time for new people trying  //
// to contribute to the project.                        //
//////////////////////////////////////////////////////////
// This file was contributed by Gabriel Schulhof        //
// (schulhof@atips.ca).                                 //
//////////////////////////////////////////////////////////
// Contents:                                            //
//                                                      //
//                                                      //
//////////////////////////////////////////////////////////

#include "global_consts.h"
#include "bus_layout_dialog.h"
#include "vector_table_options_dialog_data.h"
#include "vector_table_options_dialog_callbacks.h"

void VectorTableToDialog (vector_table_options_D *dialog, BUS_LAYOUT *bus_layout, int *sim_type, VectorTable *pvt)
  {
  GtkTreeModel *model = NULL ;
  GtkWidget *tbtn = NULL ;
  if (NULL == dialog || NULL == sim_type || NULL == pvt) return ;

  g_object_set_data (G_OBJECT (dialog->dialog), "user_sim_type", sim_type) ;
  g_object_set_data (G_OBJECT (dialog->dialog), "user_pvt", pvt) ;

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (tbtn = (VECTOR_TABLE == (*sim_type) ? dialog->tbtnVT : dialog->tbtnExhaustive)), TRUE) ;

  g_object_set_data (G_OBJECT (dialog->crActive), "pvt", pvt) ;

  if (NULL != (model = GTK_TREE_MODEL (create_bus_layout_tree_store (bus_layout, ROW_TYPE_INPUT, 1, G_TYPE_BOOLEAN))))
    {
    gboolean bActive = FALSE ;
    GtkTreeIter itr, itrChild ;
    int row_type = -1, idx = -1 ;
    QCADCell *cell = NULL ;
    gboolean bBusActive = FALSE ;

    // First reflect the active_flag of current cell inputs
    if (gtk_tree_model_get_iter_first (model, &itr))
      while (TRUE)
        {
        gtk_tree_model_get (model, &itr, 
          BUS_LAYOUT_MODEL_COLUMN_TYPE, &row_type,
          BUS_LAYOUT_MODEL_COLUMN_CELL, &cell, -1) ;
        if (ROW_TYPE_CELL_INPUT == row_type && NULL != cell)
          if (-1 != (idx = VectorTable_find_input_idx (pvt, cell)))
            gtk_tree_store_set (GTK_TREE_STORE (model), &itr, 
              VECTOR_TABLE_MODEL_COLUMN_ACTIVE, exp_array_index_1d (pvt->inputs, VT_INPUT, idx).active_flag, -1) ;
        if (!gtk_tree_model_iter_next_dfs (model, &itr)) break ;
        }

    // For any given bus, if any of its cells are active, then the bus is active. Reflect this.
    if (gtk_tree_model_get_iter_first (model, &itr))
      while (TRUE)
        {
        bBusActive = FALSE ;
        if (gtk_tree_model_iter_children (model, &itrChild, &itr))
          {
          while (TRUE)
            {
            gtk_tree_model_get (model, &itrChild, VECTOR_TABLE_MODEL_COLUMN_ACTIVE, &bActive, -1) ;
            if ((bBusActive = bActive)) break ;
            if (!gtk_tree_model_iter_next (model, &itrChild)) break ;
            }
          gtk_tree_store_set (GTK_TREE_STORE (model), &itr, VECTOR_TABLE_MODEL_COLUMN_ACTIVE, bBusActive, -1) ;
          }
        if (!gtk_tree_model_iter_next (model, &itr)) break ;
        }

    gtk_tree_view_set_model (GTK_TREE_VIEW (dialog->tv), model) ;
    }

  gtk_tree_view_expand_all (GTK_TREE_VIEW (dialog->tv)) ;

  vector_table_options_dialog_btnSimType_clicked (tbtn, dialog) ;
  }

void DialogToVectorTable (vector_table_options_D *dialog)
  {
  int *sim_type = NULL ;
  VectorTable *pvt = NULL ;

  if (NULL == dialog) return ;

  sim_type = g_object_get_data (G_OBJECT (dialog->dialog), "user_sim_type") ;
  pvt = g_object_get_data (G_OBJECT (dialog->dialog), "user_pvt") ;

  if (NULL == sim_type || NULL == pvt) return ;

  (*sim_type) = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dialog->tbtnVT)) ? VECTOR_TABLE : EXHAUSTIVE_VERIFICATION ;
  }