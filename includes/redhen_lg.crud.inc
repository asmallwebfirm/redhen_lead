<?php

/**
 * @file
 * Hooks, functions, and callbacks for Redhen Lead Generation CRUD operations.
 */


/**
 * Load lead gen form configuration.
 */
function redhen_lg_form_load($lg_form) {
  return redhen_lg_forms($lg_form);
}


/**
 * List of gumroad product types.
 */
function redhen_lg_forms($name = NULL) {
  $form_configs = entity_load_multiple_by_name('redhen_lg_form', isset($name) ? array($name) : FALSE);
  return isset($name) ? reset($form_configs) : $form_configs;
}


/**
 * Save gumroad product type entity.
 */
function redhen_lg_form_save($lg_form) {
  entity_save('redhen_lg_form', $lg_form);
}


/**
 * Delete single gumroad product type.
 */
function redhen_lg_form_delete($lg_form) {
  entity_delete('redhen_lg_form', entity_id('redhen_lg_form' ,$lg_form));
}


/**
 * Delete multiple gumroad product types.
 */
function redhen_lg_form_delete_multiple($lg_form) {
  entity_delete_multiple('redhen_lg_form', $lg_form);
}
