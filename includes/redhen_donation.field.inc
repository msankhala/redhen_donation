<?php

/**
 * @file
 * Field hooks.
 */

/**
 * Implements hook_field_info().
 */
function redhen_donation_field_info() {
  return array(
    'redhen_donation' => array(
      'label' => t('Donation'),
      'description' => t('Enables donations of a selected type for an entity.'),
      'settings' => array(),
      'instance_settings' => array(
        'default_redhen_donation_settings' => array(
          'status' => 0,
          'scheduling' => array(
            'open_source' => NULL,
            'open' => NULL,
            'close_source' => NULL,
            'close' => NULL,
          ),
          'settings' => array(),
        ),
      ),
      'default_widget' => 'redhen_donation_select',
      'default_formatter' => 'redhen_donation_form',
      'no_ui' => FALSE,
      'property_type' => 'text',
      'property_callbacks' => array('entity_metadata_field_text_property_callback'),
    ),
  );
}

/**
 * Implements hook_field_instance_settings_form().
 *
 * Add default donation field instance settings.
 */
function redhen_donation_field_instance_settings_form($field, $instance) {
  $form = $form_state = array();

  $default_settings = isset($instance['settings']['default_redhen_donation_settings']) ? $instance['settings']['default_redhen_donation_settings'] : array();

  $form['donate_tab'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Donate Tab'),
    '#default_value' => isset($settings['donate_tab']) ? $settings['donate_tab'] : TRUE,
    '#required' => FALSE,
    '#description' => t('Enable a tab on the content displaying the donation form.'),
  );

  // Flatten scheduling and reminder settings since this form is in tree mode.
  foreach ($default_settings as $key => $val) {
    if ($key != 'settings' and is_array($val)) {
      foreach ($val as $key1 => $val1) {
        if (is_array($val1)) {
          foreach ($val1 as $key2 => $val2) {
            $default_settings[$key2] = $val2;
          }
        }
        else {
          $default_settings[$key1] = $val1;
        }
      }
      unset($default_settings[$key]);
    }
  }

  $form['default_redhen_donation_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default Donation settings'),
    '#collapsible' => TRUE,
    '#description' => t("These settings will be applied when an entity with this field is saved and does not yet have it's own settings applied."),
  );

  $settings_form = redhen_donation_entity_settings_form($form['default_redhen_donation_settings'], $form_state, $default_settings, $instance['entity_type'], NULL, $instance['bundle']);

  // Unset the save button just in case.
  unset($settings_form['save']);

  $form['default_redhen_donation_settings'] += $settings_form;

  // @todo: validation
  return $form;
}

/**
 * Implements hook_field_is_empty().
 */
function redhen_donation_field_is_empty($item, $field) {
  if (empty($item['redhen_donation_type']) && (string) $item['redhen_donation_type'] !== '') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_field_widget_info().
 */
function redhen_donation_field_widget_info() {
  return array(
    'redhen_donation_select' => array(
      'label' => t('Donation type'),
      'field types' => array('redhen_donation'),
      'settings' => array(),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function redhen_donation_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $options = array('' => t('-- Disable Donations --'));
  foreach (redhen_donation_get_types() as $type) {
    $options[$type->name] = $type->label;
  }
  $element += array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($items[$delta]) ? $items[$delta] : array(),
  );

  // Force some help text into the field, appending anything the user added.
  $element['#description'] .= ' ' . t('Select what type of donation should be enabled for this @type.', array('@type' => $instance['bundle']));

  return array('redhen_donation_type' => $element);
}

/**
 * Implements hook_field_formatter_info().
 */
function redhen_donation_field_formatter_info() {
  return array(
    'redhen_donation_default' => array(
      'label' => t('Default'),
      'field types' => array('redhen_donation'),
      'settings' => array(
        'label' => '',
      ),
    ),
    'redhen_donation_link' => array(
      'label' => t('Donation Link'),
      'field types' => array('redhen_donation'),
      'settings' => array(
        'label' => '',
      ),
    ),
    'redhen_donation_form' => array(
      'label' => t('Donation Form'),
      'field types' => array('redhen_donation'),
      'settings' => array(
        'use_cart' => FALSE,
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function redhen_donation_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $element = array();

  if ($display['type'] == 'redhen_donation_default' || $display['type'] == 'redhen_donation_link') {
    $element['label'] = array(
      '#title' => t('Label'),
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => $settings['label'],
      '#required' => FALSE,
      '#description' => t("Optional label to use when displaying the donation title or link. Leave blank to use the parent item's label."),
    );
  }

  if ($display['type'] == 'redhen_donation_form') {
    $element['use_cart'] = array(
      '#title' => t('Use Cart'),
      '#type' => 'checkbox',
      '#default_value' => $settings['use_cart'],
      '#description' => t("Enabling this option will remove payment information from the form, and instead of immediately processing the donation it will be added to the user's cart."),
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function redhen_donation_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  $summary = '';

  if ($display['type'] == 'redhen_donation_default' || $display['type'] == 'redhen_donation_link') {
    if (!empty($settings['label'])) {
      $summary .= t('Donation label: @label.', array('@label' => $settings['label']));
    }
    else {
      $summary .= t('Donation label: Parent label.');
    }
  }
  if ($display['type'] == 'redhen_donation_form') {
    if (!empty($settings['use_cart'])) {
      $summary .= t('Use Cart');
    }
    else {
      $summary .= t('Single-page Processing');
    }
  }

  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function redhen_donation_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  // We know we should only have a single item.
  if (isset($items[0]['redhen_donation_type']) && !empty($items[0]['redhen_donation_type'])) {
    $donation_type = redhen_donation_type_load($items[0]['redhen_donation_type']);
    $settings = $display['settings'];
    $label = !empty($settings['label']) ? $settings['label'] : $donation_type->label;

    switch ($display['type']) {
      case 'redhen_donation_default':
        $element[0] = array('#markup' => $label);
        break;

      case 'redhen_donation_link':
        // Enable donation link if accessible.
        list($entity_id) = entity_extract_ids($entity_type, $entity);
        if (redhen_donation_donate_page_access($entity_type, $entity) && redhen_donation_status($entity_type, $entity_id)) {
          $uri = entity_uri($entity_type, $entity);
          // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $element[0] = array(
//             '#markup' => theme('redhen_donation_link',
//               array(
//                 'label' => $label,
//                 'path' => $uri['path'] . '/donate',
//               )
//             ),
//           );

        }
        break;

      case 'redhen_donation_form':
        // Enable donation form if accessible.
        list($entity_id) = entity_extract_ids($entity_type, $entity);
        if (redhen_donation_donate_page_access($entity_type, $entity) && redhen_donation_status($entity_type, $entity_id)) {
          $donation = entity_get_controller('redhen_donation')->create(array(
            'entity_type' => $entity_type,
            'entity_id' => $entity_id,
            'type' => $donation_type->name,
          ));
          $use_cart = isset($display['settings']['use_cart']) ? $display['settings']['use_cart'] : FALSE;
          $element[0] = \Drupal::formBuilder()->getForm('redhen_donation_form', $donation, $use_cart);
        }
        break;
    }
  }

  return $element;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Hide the cardinality setting on the field settings for donation fields.
 */
function redhen_donation_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {
  if ($form['#field']['type'] == 'redhen_donation') {
    $form['field']['#access'] = FALSE;
    $form['field']['cardinality']['#default_value'] = 1;
    $form['field']['cardinality']['#access'] = FALSE;
    $form['#validate'][] = 'redhen_donation_form_field_ui_field_edit_form_validate';
  }
}

/**
 * Validation handler for redhen_donation.
 *
 * Ensure cardinality is set to 1 on donation fields.
 */
function redhen_donation_form_field_ui_field_edit_form_validate(&$form, &$form_state) {
  if ($form['#field']['type'] == 'redhen_donation') {
    if ($form_state['values']['field']['cardinality'] !== 1) {
      form_set_error('cardinality', t('Cardinality on donation fields must be set to one.'));
    }

    $donation_amounts = &$form_state['values']['instance']['settings']['default_redhen_donation_settings']['settings']['donation_amounts'];
    if (\Drupal::moduleHandler()->moduleExists('options_element')) {
      $donation_amounts = form_options_from_text($donation_amounts['options_text'], 'mixed');
    }
    else {
      $donation_amounts = redhen_extract_allowed_amounts($donation_amounts);
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add a validation handler for
 * redhen_donation_form_field_ui_field_overview_form().
 */
function redhen_donation_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {
  $form['#validate'][] = 'redhen_donation_form_field_ui_field_overview_form_validate';
}

/**
 * Validation callback for redhen_donation_form_field_ui_field_overview_form().
 *
 * Ensure only one donation field is added per entity.
 */
function redhen_donation_form_field_ui_field_overview_form_validate(&$form, &$form_state) {
  $fields = $form_state['values']['fields'];
  if ($fields['_add_new_field']['type'] == 'redhen_donation') {
    foreach ($form['#fields'] as $field_name) {
      $field = field_info_field($field_name);
      if ($field['type'] == 'redhen_donation') {
        form_set_error('_add_new_field', t('An entity can only have one donation field.'));
      }
    }
  }
}

/**
 * Implements hook_field_create_instance().
 */
function redhen_donation_field_create_instance($instance) {
  // Rebuild menu to recognize donation paths.
  _redhen_donation_menu_rebuild($instance);
}

/**
 * Implements hook_field_delete_instance().
 */
function redhen_donation_field_delete_instance($instance) {
  // Remove donation paths from menu router.
  _redhen_donation_menu_rebuild($instance);

  // @TODO: should we delete all donations at this point?
}

/**
 * Rebuild the menu for a given donation field instance.
 *
 * @param string $instance
 *   Field instance being added or deleted.
 */
function _redhen_donation_menu_rebuild($instance) {
  $donation_fields = field_read_fields(array('type' => 'redhen_donation'));
  if (in_array($instance['field_name'], array_keys($donation_fields))) {
    menu_rebuild();
  }
}
