<?php

/**
 * @file
 * Hooks, functions, and callbacks related to RedHen Lead Gen form blocks.
 */


/**
 * Primary callback for RedHen Lead Gen form blocks. Called directly from the
 * Redhen LG implementation of hook_block_view().
 */
function redhen_lg_get_block($delta = '') {
  $block = array();

  // Only provide the block if the lead gen form still exists.
  if ($config = redhen_lg_form_load($delta)) {
    // If the CTA is dynamic, ensure the field even exists on this page.
    if ($config->settings['cta']['type'] == 'dynamic') {
      $entity = redhen_lg_menu_get_item();
      $entity_type = redhen_lg_menu_get_item_type();
      $field = $config->settings['cta']['dynamic']['field'];

      if ($entity) {
        $wrapper = entity_metadata_wrapper($entity_type, $entity);
        // If the field doesn't exist, bail out.
        if (!isset($wrapper->$field)) {
          return $block;
        }
      }

      // If the primary menu object isn't an entity, bail out.
      else {
        return $block;
      }
    }

    $block['subject'] = $config->settings['block_config']['settings']['title'];
    $block['content'] = drupal_get_form('redhen_lg_block_form', $config);

  }

  return $block;
}


/**
 * Form callback for the main Lead Gen block form.
 */
function redhen_lg_block_form($form, &$form_state, $lg_form) {
  $contact = redhen_contact_create(array('type' => $lg_form->settings['contact']['contact_type']));

  // Use the proper CTA target.
  $form['#attributes']['target'] = $lg_form->settings['cta']['target'];

  // Provide first and last name fields.
  $form['name'] = array();
  $form['name']['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#default_value' => $contact->first_name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => -6,
    '#size' => 15,
  );
  $form['name']['last_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Last name'),
    '#default_value' => $contact->last_name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#weight' => 5,
    '#size' => 15,
  );  

  // Save the contact to the form state.
  $form_state['redhen_contact'] = $contact;

  // Attach fields.
  field_attach_form('redhen_contact', $contact, $form, $form_state);

  // Provide the submit/CTA.
  $form['actions'] = array(
    'submit' => array(
      '#type' => 'submit',
      '#value' => 'Submit',
    ),
    '#weight' => 10,
  );

  return $form;
}


/**
 * Validation handler for redhen_lg_block_form().
 */
function redhen_lg_block_form_validate($form, &$form_state) {
  $contact = $form_state['redhen_contact'];

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_contact', $contact, $form, $form_state);
}


/**
 * Submit handler for redhen_lg_block_form().
 */
function redhen_lg_block_form_submit($form, &$form_state) {
  $contact = &$form_state['redhen_contact'];

  // Whether or not the contact is new, alter the name to what is expected.
  $contact->first_name = $form_state['values']['first_name'];
  $contact->last_name = $form_state['values']['last_name'];

  // Notify field widgets.
  field_attach_submit('redhen_contact', $contact, $form, $form_state);

  // Save the contact.
  $contact = redhen_contact_save($contact);

  // Provide the CTA action based on the config settings.
  $form_state['redirect'] = redhen_lg_get_button_action($form_state['build_info']['args'][0]->settings);
}


/**
 * Returns the primary menu object for the current page, if one exists.
 */
function redhen_lg_menu_get_item() {
  $entity_type = &drupal_static(__FUNCTION__);

  if ($object = menu_get_object('node')) {
    $entity_type = 'node';
    return $object;
  }
  else {
    $item = menu_get_item();
    foreach ($item['page_arguments'] as $args) {
      if (is_object($args) && method_exists($args, 'entityType')) {
        $entity_type = $args->entityType();
        return $args;
      }
    }
  }

  return FALSE;
}


/**
 * Companion function to redhen_lg_menu_get_item(). Returns the type of entity
 * it returned.
 */
function redhen_lg_menu_get_item_type() {
  return drupal_static('redhen_lg_menu_get_item');
}


/**
 * Returns the lead gen form action URL (suitable for setting on form_state
 * redirect) based on the context of the current page request.
 */
function redhen_lg_get_button_action($settings) {
  $url = '';

  if ($settings['cta']['type'] == 'dynamic') {
    // Load the entity and a wrapper for it.
    $entity = redhen_lg_menu_get_item();
    $entity_type = redhen_lg_menu_get_item_type();
    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $field = $settings['cta']['dynamic']['field'];

    // Determine the type of field.
    $field_info = field_info_field($field);

    // Reactions based on the specified field type.
    switch ($field_info['type']) {
      // Handling for files.
      case 'file':
        $file = $wrapper->$field->value();
        $url = file_create_url($file['uri']);
        break;

      case 'link_field':
        $link = $wrapper->$field->value();
        $url = $link['url'];
        break;
    }
  }
  else {
    $url = $settings['cta']['static']['url'];
  }

  return $url;
}
