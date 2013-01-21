<?php

/**
 * @file
 * Classes for Redhen Lead Generation forms.
 */


/**
 * Redhen Lead Generation form class.
 */
class RedhenLGForm extends Entity {
  public $name;
  public $label;
  public $weight = 0;

  public function __construct($values = array()) {
    parent::__construct($values, 'redhen_lg_form');
  }

  function isLocked() {
    return isset($this->status) && empty($this->is_new) && (($this->status & ENTITY_IN_CODE) || ($this->status & ENTITY_FIXED));
  }
}


/**
 * Controller for Redhen Lead Generation forms.
 */
class RedhenLGFormController extends EntityAPIControllerExportable {
   public function create(array $values = array()) {
    $values += array(
      'label' => '',
    );
    return parent::create($values);
  }

  /**
   * Save Redhen Lead Gen form.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    parent::save($entity, $transaction);

    // Load necessary incs.
    module_load_include('inc', 'redhen_lg', 'includes/redhen_lg.forms');
    module_load_include('inc', 'block', 'block.admin');

    // Build a list of submit handlers that need to be called.
    $fake_form = array('#submit' => array());
    $fake_form_state = array('values' => array('module' => 'redhen_lg', 'delta' => $entity->name));
    $fake_form = block_admin_configure($fake_form, $fake_form_state, 'redhen_lg', $entity->name);
    $fake_hooks = array('form', 'form_block_add_block_form');
    $fake_formid = 'block_admin_configure';
    drupal_alter($fake_hooks, $fake_form, $fake_form_state, $fake_formid);
    $fake_form['#submit'][] = 'block_admin_configure_submit';

    // Set the form_state values as it's expected by known submit handlers.
    $submit_handlers = $fake_form['#submit'];
    $fake_form = array();
    $fake_form_state = array(
      'values' => array(
        'module' => 'redhen_lg',
        'delta' => $entity->name,
      ),
    );
    _redhen_lg_set_block_config_form_state($fake_form_state, $entity->settings['block_config']);

    // Loop through all submit handlers.
    foreach ($submit_handlers as $handler) {
      $handler($fake_form, $fake_form_state);
    }

    // We'll need to rehash blocks. There's no better indirect way to call this.
    block_flush_caches();
  }

  /**
   * Delete redhen Lead Gen form.
   */
  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    parent::delete($ids, $transaction);

    // This also requires a block rehash.
    block_flush_caches();
  }
}

/**
 * UI controller for Redhen Lead Generation forms.
 */
class RedhenLGFormUIController extends EntityDefaultUIController {
  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['title'] = 'RedHen Lead Gen settings';
    $items[$this->path]['description'] = 'Manage Redhen Lead Gen forms.';
    return $items;
  }
}
