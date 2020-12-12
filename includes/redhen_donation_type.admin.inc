<?php

/**
 * @file
 * Donation type editing UI.
 */

/**
 * Generates the donation type editing form.
 */
function redhen_donation_type_form($form, &$form_state, $donation_type, $op = 'edit') {
  if ($op == 'clone') {
    $donation_type->label .= ' (cloned)';
    $donation_type->name = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $donation_type->label,
    '#description' => t('The human-readable name of this donation type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($donation_type->name) ? $donation_type->name : '',
    '#maxlength' => 32,
    '#disabled' => $donation_type->locked && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'redhen_donation_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this donation type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['settings'] = array(
    '#type' => 'item',
    '#tree' => TRUE,
  );

  $form['settings']['contact_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact Settings'),
    '#collapsible' => FALSE,
  );

  $form['settings']['contact_settings']['contact_type'] = array(
    '#type' => 'select',
    '#title' => t('Contact type'),
    '#default_value' => isset($donation_type->settings['contact_settings']['contact_type']) ? $donation_type->settings['contact_settings']['contact_type'] : NULL,
    '#options' => redhen_contact_type_options_list(),
    '#empty_value' => TRUE,
    '#required' => TRUE,
    '#description' => t('Select the contact type that will be created if a contact does not exist.'),
    '#ajax' => array(
      'callback' => 'redhen_donation_type_form_callback',
      'wrapper' => 'redhen-field-wrapper',
    ),
  );

  // If there are no contact types to select, warn the user before they start
  // filling out the form:
  if (empty(redhen_contact_type_options_list()) && !$form_state['rebuild']) {
    // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $message = t("You must add at least one !link before you can create a donation type.",
      array('!link' => l(t('contact type'), 'admin/structure/redhen/contact_types')));

    \Drupal::messenger()->addWarning($message);
  }

  $form['settings']['contact_settings']['update_fields'] = array(
    '#type' => 'checkbox',
    '#title' => t('Update contact fields'),
    '#description' => t('Should the contact fields be updated?'),
    '#default_value' => isset($donation_type->settings['contact_settings']['update_fields']) ? $donation_type->settings['contact_settings']['update_fields'] : NULL,
  );

  $form['settings']['contact_settings']['fields'] = array(
    '#type' => 'item',
    '#prefix' => '<div id="redhen-field-wrapper">',
    '#suffix' => '</div>',
  );

  $form['settings']['contact_settings']['profile_selector'] = array(
    '#type' => 'item',
    '#prefix' => '<div id="redhen-profile-selector-wrapper">',
    '#suffix' => '</div>',
  );

  $contact_type = NULL;
  if (!empty($form_state['values']['settings']['contact_settings']['contact_type'])) {
    $contact_type = $form_state['values']['settings']['contact_settings']['contact_type'];
  }
  elseif (!empty($donation_type->settings['contact_settings']['contact_type'])) {
    $contact_type = $donation_type->settings['contact_settings']['contact_type'];
  }

  if (!empty($contact_type)) {
    // Get fields for field selector and profile sync selector.
    $fields = field_info_instances('redhen_contact', $contact_type);

    $other_fields = array();
    $address_fields = array();
    foreach ($fields as $field_name => $field) {
      $field_info = field_info_field($field_name);
      if ($field_info['type'] == 'addressfield') {
        $address_fields[$field_name] = $field['label'];
      }
      else {
        if ($field_info['type'] != 'redhen_email') {
          $other_fields[$field_name] = $field['label'];
        }
      }
    }

    $other_fields['middle_name'] = 'Middle Name';
    $form['settings']['contact_settings']['fields'] = array(
      '#title' => t('Contact fields'),
      '#type' => 'checkboxes',
      '#options' => $other_fields,
      '#default_value' => isset($donation_type->settings['contact_settings']['fields']) ? $donation_type->settings['contact_settings']['fields'] : array(),
      '#description' => t('Select the contact fields to display on the donation form. If an existing contact linked to an authenticated user is loaded, and is of a different type, than some fields may be unavailable.'),
      '#prefix' => '<div id="redhen-field-wrapper">',
      '#suffix' => '</div>',
    );
    $address_default = array('billing' => t('None'));
    $address_fields = array_merge($address_default, $address_fields);
    $form['settings']['contact_settings']['profile_selector'] = array(
      '#type' => 'select',
      '#title' => t('Billing address'),
      '#options' => $address_fields,
      '#default_value' => isset($donation_type->settings['contact_settings']['profile_selector']) ? $donation_type->settings['contact_settings']['profile_selector'] : 'billing',
      '#description' => t(
        'Select a contact address field to sync with the billing profile. If "none" is selected, no Contact address fields will be updated with the information provided.'
      ),
      '#prefix' => '<div id="redhen-profile-selector-wrapper">',
      '#suffix' => '</div>',
    );
  }

  $form['settings']['commerce_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Commerce Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $product_types = commerce_product_type_options_list();
  // Recurring product types can be identified by having the recurring fields
  // attached to them.
  $rec_price_info = field_info_field('commerce_recurring_rec_price');
  $recurring_product_types = $rec_price_info['bundles']['commerce_product'];
  foreach ($recurring_product_types as $rec_type) {
    unset($product_types[$rec_type]);
  }
  $one_time_product_default = isset($donation_type->settings['commerce_settings']['one_time_product_type']) ? $donation_type->settings['commerce_settings']['one_time_product_type'] : NULL;
  if (empty($one_time_product_default) && count($product_types) == 1) {
    // If there is only one product type, assume we are going to use that so
    // product options show up without a seperate ajax request:
    $one_time_product_default = key($product_types);
    $form_state['values']['settings']['commerce_settings']['one_time_product_type'] = $one_time_product_default;
  }
  $form['settings']['commerce_settings']['one_time_product_type'] = array(
    '#title' => t('One time product type'),
    '#type' => 'select',
    '#options' => $product_types,
    '#default_value' => $one_time_product_default,
    '#required' => TRUE,
    '#empty_value' => TRUE,
    '#ajax' => array(
      'callback' => 'redhen_donation_type_form_callback',
      'wrapper' => 'product-options-wrapper',
    ),
  );

  $order_type_options = commerce_order_type_options_list();
  if (count($order_type_options) > 1) {
    $form['settings']['commerce_settings']['order_type'] = array(
      '#type' => 'select',
      '#title' => t('Order type.'),
      '#options' => $order_type_options,
      '#default_value' => isset($donation_type->settings['commerce_settings']['order_type']) ? $donation_type->settings['commerce_settings']['order_type'] : NULL,
      '#description' => t('Choose an order type.'),
    );
  }

  $line_item_type_options = commerce_line_item_type_options_list();
  if (count($line_item_type_options) >= 1) {
    $form['settings']['commerce_settings']['line_item_type'] = array(
      '#type' => 'select',
      '#title' => t('Line item type'),
      '#options' => $line_item_type_options,
      '#default_value' => isset($donation_type->settings['commerce_settings']['line_item_type']) ? $donation_type->settings['commerce_settings']['line_item_type'] : NULL,
      '#required' => TRUE,
      '#empty_value' => TRUE,
      '#description' => t('Choose a line item type.'),
    );
  }

  $form['settings']['commerce_settings']['product_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product options'),
    '#prefix' => '<div id="product-options-wrapper">',
    '#suffix' => '</div>',
  );

  $one_time_product_type = NULL;
  if (!empty($form_state['values']['settings']['commerce_settings']['one_time_product_type'])) {
    $one_time_product_type = $form_state['values']['settings']['commerce_settings']['one_time_product_type'];
  }
  elseif (!empty($donation_type->settings['commerce_settings']['one_time_product_type'])) {
    $one_time_product_type = $donation_type->settings['commerce_settings']['one_time_product_type'];
  }

  if (!empty($one_time_product_type)) {
    // Product options.
    $rec_product_options = array();
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'commerce_product')
      ->propertyCondition('status', 1)
      ->propertyCondition(
        'type',
        array_merge($recurring_product_types, array($one_time_product_type)),
        'IN'
      );
    $result = $query->execute();

    if (!isset($result['commerce_product'])) {
      // If there are no products to select, warn the user before they start
      // filling out the form:
      // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $message = t("You must add at least one !link before you can create a donation type.", array('!link' => l(t('product'), 'admin/commerce/products')));

      \Drupal::messenger()->addWarning($message);
    }
    else {
      $products = commerce_product_load_multiple(array_keys($result['commerce_product']));
      $rec_product_description = array();
      foreach ($products as $product) {
        if (in_array($product->type, $recurring_product_types)) {
          $product_wrapper = entity_metadata_wrapper('commerce_product', $product);
          $rec_product_options[$product->sku] = $product->title;
          $rec_product_description[$product->sku] = redhen_donation_product_description($product_wrapper);
        }
        else {
          $product_options[$product->sku] = $product->title;
        }
      }
    }

    $form['settings']['commerce_settings']['product_options']['product_select_label'] = array(
      '#type' => 'textfield',
      '#title' => t('Product selection label'),
      '#default_value' => isset($donation_type->settings['commerce_settings']['product_options']['product_select_label']) ? $donation_type->settings['commerce_settings']['product_options']['product_select_label'] : '',
      '#description' => t('The label to show above the product selections. Defaults to "Donation Type".'),
    );

    $form['settings']['commerce_settings']['product_options']['one_time_product'] = array(
      '#type' => 'select',
      '#title' => t('One Time Product'),
      '#options' => isset($product_options) ? $product_options : array(),
      '#description' => t('Choose a one time product.'),
      '#default_value' => isset($donation_type->settings['commerce_settings']['product_options']['one_time_product']) ? $donation_type->settings['commerce_settings']['product_options']['one_time_product'] : NULL,
      '#required' => TRUE,
      '#empty_value' => TRUE,
    );

    if (count($rec_product_options) >= 1) {
      $form['settings']['commerce_settings']['product_options']['recurring'] = array(
        '#type' => 'checkboxes',
        '#multiple' => TRUE,
        '#title' => t('Recurring Product'),
        '#options' => $rec_product_options,
        '#default_value' => $donation_type->settings['commerce_settings']['product_options']['recurring'],
        '#description' => t('Choose a recurring product.'),
      );
      foreach ($form['settings']['commerce_settings']['product_options']['recurring']['#options'] as $key => $label) {
        $form['settings']['commerce_settings']['product_options']['recurring'][$key]['#description'] = $rec_product_description[$key];
      }

      $form['settings']['commerce_settings']['product_options']['recurring_user_register_notify'] = array(
        '#type' => 'checkbox',
        '#title' => t('Send a welcome message to user created for recurring donation?'),
      );

      if (isset($donation_type->settings['commerce_settings']['product_options']['recurring_user_register_notify'])) {
        $form['settings']['commerce_settings']['product_options']['recurring_user_register_notify']['#default_value'] = $donation_type->settings['commerce_settings']['product_options']['recurring_user_register_notify'];
      }
    }
    else {
      $form['settings']['commerce_settings']['product_options']['recurring'] = array(
        '#type' => 'hidden',
        '#value' => array(),
      );
    }
  }

  // Get Payment Methods.
  $conditions = array(
    'event' => 'commerce_payment_methods',
    'plugin' => 'reaction rule',
    'active' => TRUE,
  );
  $payment_rules = rules_config_load_multiple(FALSE, $conditions);
  foreach ($payment_rules as $name => $rule) {
    // Todo Note: Submit patch to commerce_payment_example.module to have the payment_method store an array containing a method_id.
    foreach ($rule->actions() as $action) {
      if ($action->settings['payment_method'] == "commerce_payment_example") {
        $method = $action->settings['payment_method'];
      }
      elseif (isset($action->settings['payment_method']['method_id'])) {
        $method = $action->settings['payment_method']['method_id'];
      }
      else {
        $method = FALSE;
      }
    }
    if ($method) {
      $payment_options[$method . '|' . $name] = $rule->label;
    }
  }

  $methods = redhen_donation_get_payment_methods($form_state);

  if (empty($payment_options)) {
    // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $form['settings']['commerce_settings']['payment_options'] = array(
//       '#type' => 'fieldset',
//       '#title' => t('Payment Methods'),
//       '#collapsible' => FALSE,
//       '#collapsed' => FALSE,
//       '#tree' => TRUE,
//       '#id' => 'edit-redhen-donation-payment-options-wrapper',
//       '#description' => t('Please configure and/or enable a valid !link.',
//         array('!link' => l(t('payment method'), 'admin/commerce/config/payment-methods'))
//       ),
//     );

    // If there are no payment types, warn the user before they start filling
    // out the form:
    if (!$form_state['rebuild']) {
      // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $message = t("You must configure and/or enable at least one !link before you can create a donation type.",
        array('!link' => l(t('payment method'), 'admin/commerce/config/payment-methods')));

      \Drupal::messenger()->addWarning($message);
    }
  }
  else {
    $form['settings']['commerce_settings']['payment_options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Payment Methods'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#tree' => TRUE,
      '#id' => 'edit-redhen-donation-payment-options-wrapper',
      '#description' => t('Select the available payment methods. If a recurring product is used, ensure the method supports the Commerce Recurring framework.'),
    );

    $form['settings']['commerce_settings']['payment_options']['payment_methods'] = array(
      '#theme' => 'redhen_donation_payment_options_form_table',
      '#tree' => TRUE,
      '#header' => array(
        'payment_method' => t('Payment method'),
        'label' => t('Label'),
        'currency' => t('Currency Label'),
        'delete' => t('Delete'),
      ),
      '#attributes' => array(
        'id' => array('edit-redhen-donation-payment-options'),
      ),
    );

    foreach ($methods as $delta => $value) {
      $row_id = 'edit-payment-methods-' . $delta;
      $form['settings']['commerce_settings']['payment_options']['payment_methods'][$delta] = array(
        '#type' => 'container',
        '#attributes' => array(
          'id' => array($row_id),
        ),
      );
      $row = &$form['settings']['commerce_settings']['payment_options']['payment_methods'][$delta];

      $row['payment_method'] = array(
        '#type' => 'select',
        '#options' => $payment_options,
        '#default_value' => isset($form_state['methods'][$delta]['payment_method']) ? $form_state['methods'][$delta]['payment_method'] : isset($donation_type->settings['commerce_settings']['payment_methods'][0]['payment_method']) ? $donation_type->settings['commerce_settings']['payment_methods'][0]['payment_method'] : 0,
      );
      $row['label'] = array(
        '#type' => 'textfield',
        '#size' => 20,
        '#default_value' => isset($form_state['methods'][$delta]['label']) ? $form_state['methods'][$delta]['label'] : '',
      );
      $currency_options = array();
      $currencies = commerce_currencies(TRUE);
      foreach ($currencies as $currency) {
        $currency_options[$currency['code']] = $currency['name'] . ' (' . $currency['symbol'] . ')';
      }

      $row['currency'] = array(
        '#type' => 'select',
        '#options' => $currency_options,
        '#default_value' => isset($form_state['methods'][$delta]['currency']) ? $form_state['methods'][$delta]['currency'] : 0,
      );
      $row['delete'] = array(
        '#id' => 'edit-delete-' . $delta,
        '#type' => 'checkbox',
        '#value' => t('Delete'),
        '#name' => 'delete_method-' . $delta,
        '#ajax' => array(
          'callback' => 'redhen_donation_settings_form_callback',
          'wrapper' => 'edit-redhen-donation-payment-options-wrapper',
          'delta' => $delta,
        ),
      );
    }

    $form['settings']['commerce_settings']['payment_options']['add_field'] = array(
      '#value' => t('Add another payment method'),
      '#id' => 'edit-donation-add-payment-method',
      '#name' => 'donation_add_field',
      '#type' => 'button',
      '#description' => t('Add one or more payment methods.'),
      '#executes_submit_callback' => FALSE,
      '#limit_validation_errors' => array(),
      '#ajax' => array(
        'callback' => 'redhen_donation_type_form_callback',
        'wrapper' => 'edit-redhen-donation-payment-options-wrapper',
      ),
    );

  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save donation type'),
    '#weight' => 40,
  );

  if (!$donation_type->locked && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete donation type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('redhen_donation_type_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Donation settings form AJAX callback.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 *
 * @return array
 *   AJAX command array.
 */
function redhen_donation_type_form_callback($form, $form_state) {
  $trigger = $form_state['triggering_element']['#name'];
  $parents = $form_state['triggering_element']['#array_parents'];
  $delta = isset($parents[3]) ? $parents[3] : NULL;
  $commands = array();
  switch ($trigger) {
    case 'settings[contact_settings][contact_type]':
      $commands[] = ajax_command_replace('#redhen-field-wrapper', render($form['settings']['contact_settings']['fields']));
      $commands[] = ajax_command_replace('#redhen-profile-selector-wrapper', render($form['settings']['contact_settings']['profile_selector']));
      break;

    case 'settings[commerce_settings][one_time_product_type]':
      return $form['settings']['commerce_settings']['product_options'];

    case 'donation_add_field':
    case 'delete_method-' . $delta:
      // Replace the field map table.
      $commands = array(
        ajax_command_replace('#edit-redhen-donation-payment-options-wrapper', render($form['settings']['commerce_settings']['payment_options'])),
      );
      break;
  }
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Validation callback for redhen_donation_type_form().
 */
function redhen_donation_type_form_validate($form, &$form_state) {
  // Must select at least one product.
  $one_time_product = $form_state['values']['settings']['commerce_settings']['product_options']['one_time_product'];
  $recurring_products = array_filter($form_state['values']['settings']['commerce_settings']['product_options']['recurring']);
  if ($one_time_product == '' && empty($recurring_products)) {
    form_set_error('form', t('Must select at least one product.'));
  }

  // Flatten payment methods and validate.
  $form_state['values']['settings']['commerce_settings']['payment_methods'] = array_filter($form_state['values']['settings']['commerce_settings']['payment_options']['payment_methods']);
  unset($form_state['values']['settings']['commerce_settings']['payment_options']);

  // Validate payment methods:
  if (empty($form_state['values']['settings']['commerce_settings']['payment_methods'])) {
    form_set_error('settings[commerce_settings][payment_options][payment_methods]', t('Must select at least one payment method.'));
  }
}

/**
 * Form API submit callback for the type form.
 */
function redhen_donation_type_form_submit(&$form, &$form_state) {
  $donation_type = entity_ui_form_submit_build_entity($form, $form_state);
  $donation_type->save();
  $form_state['redirect'] = 'admin/structure/redhen/donation_types';
}

/**
 * Form API submit callback for the delete button.
 */
function redhen_donation_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/redhen/donation_types/manage/' . $form_state['redhen_donation_type']->name . '/delete';
}
