<?php

/**
 * @file
 * Hooks, functions, and callbacks related to Redhen Lead Form pages.
 */


/**
 * Page callback for block configuration pages of module RedHen Lead Form. This
 * ensures there's only one place to configure RedHen Lead Forms.
 */
function redhen_lead_form_block_configure_goto($lead_form_name) {
  // Preserve "destination" and prevent drupal_goto from redirecting there.
  $options = array();
  if (isset($_GET['destination'])) {
    $options = array('query' => array('destination' => $_GET['destination']));
    unset($_GET['destination']);
  }

  drupal_goto('admin/structure/redhen/lead-forms/manage/' . $lead_form_name, $options);
}
