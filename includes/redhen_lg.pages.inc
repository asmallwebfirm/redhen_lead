<?php

/**
 * @file
 * Hooks, functions, and callbacks related to Redhen Lead Gen pages.
 */


/**
 * Page callback for block configuration pages of module RedHen Lead Gen. This
 * ensures there's only one place to configure RedHen Lead Gen forms.
 */
function redhen_lg_block_configure_goto($lg_form_name) {
  drupal_goto('admin/config/redhen/lead-gen/manage/' . $lg_form_name);
}
