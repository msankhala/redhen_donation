<?php
/**
 * @file
 * Form include.
 */

/**
 * Return a form for an entity's donation settings.
 *
 * @internal object $entity
 *   Donation Entity.
 *
 * @array $form
 *   Form array.
 * @array $form_state
 *   Form state array.
 * @array $settings
 *   Settings array.
 *
 * @string $entity_type
 *   Host entity type.
 *
 * @int null $entity_id
 *   Host entity id.
 *
 * @see hook_redhen_donation_entity_settings()
 *
 * @return array
 *   Form array.
 */
function redhen_donation_entity_settings_form($form, &$form_state, $settings, $entity_type = NULL, $entity_id = NULL, $bundle = NULL, $field = NULL) {
  if ($entity_id) {
    // We'll need this info when we submit the form:
    $form_state['entity'] = array(
      'entity_id' => $entity_id,
      'entity_type' => $entity_type,
    );
  }

  // Check to see if date_popup is installed so we can provide a friendlier UI.
  $date_popup_installed = FALSE;
  if (\Drupal::moduleHandler()->moduleExists('date_popup')) {
    $date_popup_installed = TRUE;
  }
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Check to enable donations.'),
    '#default_value' => isset($settings['status']) ? $settings['status'] : -1,
  );

  $form['scheduling'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scheduling'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['scheduling']['open'] = array(
    '#type' => 'textfield',
    '#title' => t('Open Date'),
    '#maxlength' => 25,
    '#description' => t('When to automatically open donations. Format: %time. The date format is YYYY-MM-DD and %timezone is the time zone offset from UTC. Leave blank to disable.',
      array(
        '%time' => !empty($settings['open']) ? date_format(date_create($settings['open']), 'Y-m-d H:i:s O') : format_date(time(), 'custom', 'Y-m-d H:i:s O'),
        '%timezone' => !empty($settings['open']) ? date_format(date_create($settings['open']), 'O') : format_date(time(), 'custom', 'O'),
      )
    ),
    '#default_value' => !empty($settings['open']) ? $settings['open'] : '',
    '#weight' => 2,
  );

  $form['scheduling']['close'] = array(
    '#type' => 'textfield',
    '#title' => t('Close Date'),
    '#maxlength' => 25,
    '#description' => t('When to automatically close donations. Format: %time. The date format is YYYY-MM-DD and %timezone is the time zone offset from UTC. Leave blank to disable.',
      array(
        '%time' => !empty($settings['close']) ? date_format(date_create($settings['close']), 'Y-m-d H:i') : format_date(time(), 'custom', 'Y-m-d H:i'),
        '%timezone' => !empty($settings['close']) ? date_format(date_create($settings['close']), 'O') : format_date(time(), 'custom', 'O'),
      )
    ),
    '#default_value' => !empty($settings['close']) ? $settings['close'] : '',
    '#weight' => 4,
  );
  if ($entity_id && !$bundle) {
    $entity = entity_load_single($entity_type, $entity_id);
    $wrapper = entity_metadata_wrapper($entity_type, $entity);
    $bundle = $wrapper->getBundle();
  }
  if ($bundle) {
    $options = redhen_donation_fieldmap_options($entity_type, $bundle);
    $form['scheduling']['open_source'] = array(
      '#type' => 'select',
      '#title' => 'Open Date',
      '#default_value' => isset($settings['open_source']) ? $settings['open_source'] : NULL,
      '#options' => $options,
      '#weight' => 1,
    );
    $form['scheduling']['close_source'] = array(
      '#type' => 'select',
      '#title' => 'Close Date',
      '#default_value' => isset($settings['close_source']) ? $settings['close_source'] : NULL,
      '#options' => $options,
      '#weight' => 3,
    );
    $form['scheduling']['open']['#title'] = 'Specify Open Date';
    $form['scheduling']['open']['#states'] = array(
      'visible' => array(
        ':input[name="scheduling[open_source]"]' => array('value' => ''),
      ),
    );
    $form['scheduling']['close']['#title'] = 'Specify Close Date';
    $form['scheduling']['close']['#states'] = array(
      'visible' => array(
        ':input[name="scheduling[close_source]"]' => array('value' => ''),
      ),
    );
  }
  if ($date_popup_installed) {
    // @FIXME
// // @FIXME
// // This looks like another module's variable. You'll need to rewrite this call
// // to ensure that it uses the correct configuration object.
// $zonename = str_replace('_', ' ', variable_get('date_default_timezone'));

    timezone_identifiers_list();
    $form['scheduling']['open']['#type'] = 'date_popup';
    $form['scheduling']['open']['#date_format'] = 'Y-m-d H:i';
    $form['scheduling']['open']['#description'] = t('When to automatically open donations. (This uses the !timezone, which is !zone.)', array(
      '!timezone' => \Drupal\Core\Link::fromTextAndUrl(t("site's default time zone"), \Drupal\Core\Url::fromRoute('system.regional_settings')),
      '!zone' => $zonename,
    ));
    unset($form['scheduling']['open']['#maxlength']);
    $form['scheduling']['close']['#type'] = 'date_popup';
    $form['scheduling']['close']['#date_format'] = 'Y-m-d H:i';
    $form['scheduling']['close']['#description'] = t('When to automatically close donations. (This uses the !timezone, which is !zone.)', array(
      '!timezone' => \Drupal\Core\Link::fromTextAndUrl(t("site's default time zone"), \Drupal\Core\Url::fromRoute('system.regional_settings')),
      '!zone' => $zonename,
    ));
    unset($form['scheduling']['close']['#maxlength']);
  }

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['settings']['confirmation_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Confirmation page'),
    '#default_value' => isset($settings['settings']['confirmation_page']) ? $settings['settings']['confirmation_page'] : '',
    '#description' => t('Set the URL of the confirmation page. If not set, the form will redirect to itself.'),
  );

  $form['settings']['confirmation_message'] = array(
    '#type' => 'textfield',
    '#size' => 120,
    '#title' => t('Confirmation message'),
    '#default_value' => isset($settings['settings']['confirmation_message']) ? $settings['settings']['confirmation_message'] : 'Thank you for your donation!',
    '#description' => t('Set the message to appear after a successful donation. Leave blank to display no message.'),
  );
  // Display the list of available placeholders if token module is installed.
  if (\Drupal::moduleHandler()->moduleExists('token')) {
    $form['settings']['confirmation_message']['#description'] .= ' ' . t('Tokens may be used.');
    $form['settings']['token_help'] = array(
      '#type' => 'fieldset',
      '#title' => t('Available Tokens'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#tree' => TRUE,
      'token_help_details' => array(
        '#theme' => 'token_tree',
        '#token_types' => array('redhen_donation', 'redhen_donation_type'),
      ),
    );
  }
  if (isset($field)) {
    $donation_type = redhen_donation_type_load($wrapper->{$field}->value());
    $products = redhen_donation_load_configured_skus($donation_type->settings['commerce_settings']['product_options']);
    $product_options = array('' => '-- None --');
    foreach ($products as $product) {
      $product_options[$product->sku] = $product->title;
    }
    $first_product = reset($products);
    $form['settings']['default_product'] = array(
      '#type' => 'select',
      '#title' => t('Default donation product'),
      '#default_value' => isset($settings['settings']['default_product']) ? $settings['settings']['default_product'] : $first_product->sku,
      '#options' => $product_options,
      '#description' => t('Select the product type to be selected by default.'),
    );
  }
  $form['settings']['donation_entry'] = array(
    '#type' => 'select',
    '#title' => t('Donation entry'),
    '#default_value' => isset($settings['settings']['donation_entry']) ? $settings['settings']['donation_entry'] : '',
    '#options' => array(
      'select' => t('Select list'),
      'select_other' => t("Select list (with 'other' option)"),
      'radios' => t("Radio buttons"),
      'radio_other' => t("Radio buttons (with 'other' option)"),
      'custom' => t('Text field (custom amount)'),
    ),
    '#description' => t('Select the type of options widget to use for donation amount.'),
  );

  $form['settings']['donation_amounts_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount selection label'),
    '#default_value' => isset($settings['settings']['donation_amounts_label']) ? $settings['settings']['donation_amounts_label'] : 'Donation Amounts (@cur)',
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          'value' => 'custom',
        ),
      ),
    ),
    '#description' => t('Label for the list of allowed donation amounts. Include "@cur" to produce a currency symbol.'),
  );

  $single_donation_options = isset($settings['settings']['donation_amounts']) ? $settings['settings']['donation_amounts'] : NULL;
  $form['settings']['donation_amounts'] = redhen_donation_options_form_field('One-time Donation Amounts', $single_donation_options, $form_state);

  $recurring_donation_options = isset($settings['settings']['recurring_donation_amounts']) ? $settings['settings']['recurring_donation_amounts'] : NULL;
  $form['settings']['recurring_donation_amounts'] = redhen_donation_options_form_field('Recurring Donation Amounts', $recurring_donation_options, $form_state);

  $form['settings']['other_label'] = array(
    '#type' => 'textfield',
    '#title' => t('"Other" option label'),
    '#default_value' => isset($settings['settings']['other_label']) ? $settings['settings']['other_label'] : 'Other',
    '#description' => t('Enter label for the "Other Amount" option.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          array('value' => 'select'),
          array('value' => 'radios'),
          array('value' => 'custom'),
        ),
      ),
    ),
  );

  $form['settings']['minimum_donation'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum donation'),
    '#default_value' => isset($settings['settings']['minimum_donation']) ? $settings['settings']['minimum_donation'] : '',
    '#description' => t('Enter the minimum allowed donation as an integer. Leave blank for no minimum.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          array('value' => 'select'),
          array('value' => 'radios'),
        ),
      ),
    ),
  );

  $form['settings']['maximum_donation'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum donation'),
    '#default_value' => isset($settings['settings']['maximum_donation']) ? $settings['settings']['maximum_donation'] : '',
    '#description' => t('Enter the maximum allowed donation as an integer. Leave blank for no maximum.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          array('value' => 'select'),
          array('value' => 'radios'),
        ),
      ),
    ),
  );

  $form['settings']['submit_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Submit button'),
    '#default_value' => isset($settings['settings']['submit_label']) ? $settings['settings']['submit_label'] : 'Donate',
    '#description' => t('The text on the form submission button.'),
  );

  // Allow other modules to add their own custom settings:
  $form['settings'] += \Drupal::moduleHandler()->invokeAll('redhen_donation_entity_settings', [$settings]);

  // Only show save if we're not on the field instance defaults.
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
    '#access' => !empty($entity_id),
  );

  return $form;
}

function redhen_donation_options_form_field($label, $options, &$form_state) {
  if (is_array($options)) {
    if (isset($options['options_text'])) {
      $donation_amounts = $options['options_text'];
    }
    else {
      $donation_amounts = redhen_allowed_amounts_string($options);
    }
  }
  else {
    $donation_amounts = $options;
  }
  $field = array(
    '#type' => 'textarea',
    '#title' => t($label),
    '#default_value' => $donation_amounts,
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          'value' => 'custom',
        ),
      ),
    ),
    '#description' => t('List of allowed donation amounts. Enter one value per line, in the format amount|label. The amount should be entered as an integer value.'),
  );

  // Use options_element style form if possible.
  if (\Drupal::moduleHandler()->moduleExists('options_element')) {
    // Everything inside this if clause was informed by
    // options_element_form_field_ui_field_settings_form_alter() and
    // _options_element_add_allowed_values_element().
    // Get option values or use default set.
    if (isset($form_state['input']['settings']['donation_amounts']['options_field'])) {
      $options = form_options_from_text($form_state['input']['settings']['donation_amounts']['options_field'], 'mixed');
    }
    else {
      $options = form_options_from_text($field['#default_value'], 'mixed');
    }

    $element_overwrite = array(
      '#type' => 'options',
      '#options' => $options,
      '#multiple' => FALSE,
      // Uncomment to expose the toggle allowing users to make "key" optional.
      // '#key_type_toggle' => t('Custom keys'),
      '#key_type_toggled' => TRUE,
      '#default_value_allowed' => TRUE,
      '#default_value' => isset($settings['settings']['donation_amounts_default']) ? $settings['settings']['donation_amounts_default'] : '',
      '#title' => $field['#title'],
      '#description' => t('List of allowed donation amounts. The key should be entered as an integer and the value is what will be displayed on the page.'),
      '#states' => $field['#states'],
    );

    $element = \Drupal\Component\Utility\NestedArray::mergeDeep($field, $element_overwrite);
    $field = $element;
  }
  return $field;
}

/**
 * Validation handler for redhen_donation_entity_settings_form().
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 */
function redhen_donation_entity_settings_form_validate($form, &$form_state) {
  // Extract allowed amounts.
  $selection_options = array('select', 'radios', 'select_other', 'radio_other');
  if (in_array($form_state['values']['settings']['donation_entry'], $selection_options)) {
    if (\Drupal::moduleHandler()->moduleExists('options_element')) {
      $form_state['values']['settings']['donation_amounts'] = form_options_from_text($form_state['values']['settings']['donation_amounts']['options_text'], 'mixed');
      $form_state['values']['settings']['donation_amounts_default'] = $form_state['input']['settings']['donation_amounts']['default_value_field'];
      $form_state['values']['settings']['recurring_donation_amounts'] = form_options_from_text($form_state['values']['settings']['recurring_donation_amounts']['options_text'], 'mixed');
      $form_state['values']['settings']['recurring_donation_amounts_default'] = $form_state['input']['settings']['recurring_donation_amounts']['default_value_field'];
    }
    else {
      $form_state['values']['settings']['donation_amounts'] = redhen_extract_allowed_amounts($form_state['values']['settings']['donation_amounts']);
      $form_state['values']['settings']['recurring_donation_amounts'] = redhen_extract_allowed_amounts($form_state['values']['settings']['recurring_donation_amounts']);
    }
    if (empty($form_state['values']['settings']['donation_amounts'])) {
      form_set_error('settings][donation_amounts', t('No amount(s) set.'));
    }
    if (empty($form_state['values']['settings']['recurring_donation_amounts'])) {
      form_set_error('settings][recurring_donation_amounts', t('No recurring amount(s) set.'));
    }
    // Validate allowed amounts.
    foreach ($form_state['values']['settings']['donation_amounts'] as $amount => $label) {
      if (!is_numeric($amount)) {
        form_set_error('settings][donation_amounts', t('Donation amounts are invalid.'));
      }
    }
    foreach ($form_state['values']['settings']['recurring_donation_amounts'] as $amount => $label) {
      if (!is_numeric($amount)) {
        form_set_error('settings][recurring_donation_amounts', t('Recurring donation amounts are invalid.'));
      }
    }
  }

  // Validate dates:
  $dates = array('open', 'close');
  foreach ($dates as $date) {
    if (!empty($form_state['values']['scheduling'][$date])) {
      if ($form['scheduling'][$date]['#type'] != 'date_popup' && strtotime($form_state['values']['scheduling'][$date]) === FALSE) {
        form_set_error('scheduling][' . $date, t('Date is invalid.'));
      }
    }
  }
}

/**
 * Submit handler for redhen_donation_entity_settings_form().
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 */
function redhen_donation_entity_settings_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];

  $fields = redhen_donation_convert_form_settings($form_state['values']);

  redhen_donation_update_entity_settings($entity['entity_type'], $entity['entity_id'], $fields);

  $uri = entity_uri($entity['entity_type'], entity_load_single($entity['entity_type'], $entity['entity_id']));

  $form_state['redirect'] = $uri['path'];
}

/**
 * Form callback: create or edit a donation.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 * @param RedhenDonation $donation
 *   The donation object to edit or for a create form and empty donation
 *   object with an attached entity defined.
 *
 * @return mixed
 *   Donation form array.
 */
function redhen_donation_form($form, &$form_state, RedhenDonation $donation, $use_cart = FALSE) {
  $user = \Drupal::currentUser();
  $form_state['donation'] = $donation;
  $form_state['use_cart'] = $use_cart;
  if (!isset($form_state['#donation_settings'])) {
    $form_state['donation_settings']
      = $settings
        = redhen_donation_entity_settings(
            $donation->entity_type,
            $donation->entity_id,
            TRUE
          );
  }

  $donation_type = redhen_donation_type_load($donation->bundle());
  $contact_settings = $donation_type->settings['contact_settings'];
  $commerce_settings = $donation_type->settings['commerce_settings'];

  $contact_object = redhen_contact_load_by_user($user);
  \Drupal::moduleHandler()->alter('redhen_donation_default_contact', $contact_object, $user, $donation_type);
  if (!$contact_object) {
    $contact_object = redhen_contact_create(array('type' => $contact_settings['contact_type']));
  }
  $contact_wrapper = entity_metadata_wrapper('redhen_contact', $contact_object);

  module_load_include('inc', 'redhen_contact', 'includes/redhen_contact.forms');

  // Get contact type form.
  $form['redhen_contact'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact Information'),
    '#weight' => -5,
    'form' => redhen_contact_contact_form(array(), $form_state, $contact_object),
  );

  // Unset the contact forms action, we will use the registration form.
  unset($form['redhen_contact']['form']['actions']);
  // Hide all fields by default.
  $form_fields = \Drupal\Core\Render\Element::children($form['redhen_contact']['form']);
  foreach ($form_fields as $form_field) {
    if (strpos($form_field, 'field_') !== FALSE || $form_field == 'redhen_contact_email') {
      $form['redhen_contact']['form'][$form_field]['#access'] = FALSE;
    }
  }

  // Remove fields as defined in settings.
  $fields = field_info_instances('redhen_contact', $contact_settings['contact_type']);
  foreach ($fields as $field) {
    $field_info = field_info_field($field['field_name']);
    // Check for field settings.
    $field_setting = isset($contact_settings['fields'][$field_info['field_name']])
      ? $contact_settings['fields'][$field_info['field_name']]
      : NULL;
    if ($field_info['type'] != 'addressfield' && $field_setting) {
      $form['redhen_contact']['form'][$field_info['field_name']]['#access'] = TRUE;
    }

    $middle_name = ($contact_settings['fields']['middle_name'] === 'middle_name');

    $form['redhen_contact']['form']['name']['middle_name']['#access'] = $middle_name;
    unset($form['redhen_contact']['form']['redirect']);
  }

  $form['redhen_contact']['form']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#required' => TRUE,
    '#default_value' => $contact_wrapper->email->value(),
  );

  field_attach_form('redhen_donation', $donation, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => isset($settings['settings']['submit_label']) ? $settings['settings']['submit_label'] : t('Donate'),
  );

  $payment_methods = array();
  foreach ($commerce_settings['payment_methods'] as $payment_method) {
    $payment_methods[$payment_method['payment_method']] = commerce_payment_method_instance_load($payment_method['payment_method']);
  }
  $options = array();

  $payment_methods_new = $commerce_settings['payment_methods'];
  foreach ($payment_methods_new as $payment_method_new) {
    $options[$payment_method_new['payment_method']] = $payment_method_new['label'];
  }
  $first_payment_option = current(current($payment_methods_new));

  // Add a radio select widget to specify the payment method.
  if (count($payment_methods) > 1) {
    $form['payment_method'] = array(
      '#title' => t('Payment Options'),
      '#type' => 'radios',
      '#options' => $options,
      '#default_value' => $first_payment_option,
      '#ajax' => array(
        'callback' => 'redhen_donation_form_refresh',
        'wrapper' => 'payment-details',
      ),
    );
  }
  else {
    $form['payment_method'] = array(
      '#type' => 'hidden',
      '#value' => $first_payment_option,
    );
  }

  // Handle currency mapping.
  $selected_payment_method = current($commerce_settings['payment_methods']);
  if (!empty($form_state['values']['payment_method'])) {
    foreach ($commerce_settings['payments_methods'] as $payment_method) {
      if ($payment_method['payment_method'] == $form_state['values']['payment_method']) {
        $selected_payment_method  = $payment_method;
        break;
      }
    }
  }

  // Load Commerce products based on the donation type settings.
  $products = redhen_donation_load_configured_skus($commerce_settings['product_options']);
  $descriptions = array();
  $product_list = array();
  if (isset($settings['settings']['default_product'])) {
    // Display default product first.
    $product_list[$settings['settings']['default_product']] = NULL;
  }
  foreach ($products as $product_sku => $product) {
    $product_wrapper = entity_metadata_wrapper('commerce_product', $product);
    $product_list[$product_sku] = $product->title;
    $descriptions[$product_sku] = redhen_donation_product_description($product_wrapper);
  }

  if (count($products) > 1) {
    $product_label = $commerce_settings['product_options']['product_select_label'] == "" ?
      t('Donation Type') :
      $commerce_settings['product_options']['product_select_label'];
    $form['product'] = array(
      '#type' => 'radios',
      '#title' => $product_label,
      '#options' => $product_list,
      '#default_value' => isset($settings['settings']['default_product']) ? $settings['settings']['default_product'] : NULL,
      '#weight' => $form['redhen_contact']['#weight'] + 1,
      '#required' => TRUE,
      '#ajax' => array(
        'callback' => 'redhen_donation_amount_options_callback',
        'wrapper' => 'donation-amount',
        'method' => 'replace',
      ),
    );
    foreach ($form['product']['#options'] as $key => $label) {
      $form['product'][$key]['#description'] = $descriptions[$key];
    }
  }
  else {
    $product = reset($products);
    $form['product'] = array(
      '#type' => 'hidden',
      '#value' => $product->sku,
    );
  }

  // Store the currency, we'll need it for validation.
  $form_state['currency'] = $selected_payment_method['currency'];

  $cur = commerce_currency_get_symbol($selected_payment_method['currency']);

  if (isset($form_state['values']['product'])) {
    $default_product = $products[$form_state['values']['product']];
  }
  elseif (isset($form['product']['#default_value'])) {
    $default_product = $products[$form['product']['#default_value']];
  }
  else {
    $default_product = reset($products);
  }
  if (commerce_recurring_product_is_recurring($default_product) && !empty($settings['settings']['recurring_donation_amounts'])) {
    $amount_options = $settings['settings']['recurring_donation_amounts'];
  }
  else {
    $amount_options = isset($settings['settings']['donation_amounts']) ? $settings['settings']['donation_amounts'] : NULL;
  }
  $donation_amounts = array();
  if (isset($amount_options)) {
    foreach ($amount_options as $amount => $description) {
      $donation_amounts[$amount] = $description;
    }
  }
  $donation_amounts_default = !empty($settings['settings']['donation_amounts_default']) ? $settings['settings']['donation_amounts_default'] : NULL;
  $amount_label = isset($settings['settings']['donation_amounts_label']) ? $settings['settings']['donation_amounts_label'] : 'Donation Amount (@cur)';
  $amount_title = format_string($amount_label, array('@cur' => $cur));
  switch ($settings['settings']['donation_entry']) {
    case 'select':
    case 'radios':
      $form['donation'] = array(
        '#type' => $settings['settings']['donation_entry'],
        '#title' => $amount_title,
        '#options' => $donation_amounts,
        '#default_value' => $donation_amounts_default,
        '#multiple' => FALSE,
      );
      break;

    case 'select_other':
    case 'radio_other':
      $form['donation'] = array(
        '#type' => 'select_or_other',
        '#title' => $amount_title,
        '#multiple' => FALSE,
        '#options' => $donation_amounts,
        '#default_value' => $donation_amounts_default,
        '#other' => isset($settings['settings']['other_label']) ? $settings['settings']['other_label'] : t('Other'),
        '#other_description' => t('Please enter an amount.'),
        '#other_unknown_defaults' => 'other',
        '#other_delimiter' => ', ',
        '#select_type' => $settings['settings']['donation_entry'] == 'select_other' ? 'select' : 'radios',
      );

      break;

    default:
      $form['donation'] = array(
        '#type' => 'textfield',
        '#title' => $amount_title,
        '#default_value' => '',
        '#description' => t('Please enter an amount.'),
      );
      break;
  }

  $form['donation']['#weight'] = $form['redhen_contact']['#weight'] + 3;
  $form['donation']['#prefix'] = '<div id="donation-amount">';
  $form['donation']['#suffix'] = '</div>';

  $form['line_item'] = array('#weight' => $form['redhen_contact']['#weight'] + 2);
  $line_item = commerce_line_item_new($donation_type->settings['commerce_settings']['line_item_type']);
  field_attach_form('commerce_line_item', $line_item, $form['line_item'], $form_state);
  $line_item_fields = field_info_instances('commerce_line_item', $donation_type->settings['commerce_settings']['line_item_type']);
  foreach ($line_item_fields as $li_field) {
    if (empty($li_field['commerce_cart_settings']['field_access'])) {
      unset($form['line_item'][$li_field['field_name']]);
    }
  }

  if (!$form_state['use_cart']) {
    $billing_user = !is_null($contact_object->uid) ? $contact_object->uid : $user->uid;
    // Set default contact address.
    if (!(is_null($contact_object->contact_id)) && !empty($contact_settings['profile_selector']) && 'billing' != $contact_settings['profile_selector']) {
      $billing = commerce_customer_profile_new('billing', $billing_user);
      $billing_address_field = $contact_settings['profile_selector'];
      $billing->commerce_customer_address = $contact_object->{$billing_address_field};
    }
    elseif (\Drupal::moduleHandler()->moduleExists('commerce_addressbook') && $billing_user) {
      if ($default_profile_id = commerce_addressbook_get_default_profile_id($billing_user, 'billing')) {
        $billing = commerce_customer_profile_load($default_profile_id);
      }
    }
    if (!isset($billing)) {
      $billing = commerce_customer_profile_new('billing', $billing_user);
    }
    $form['customer_profile'] = array('#weight' => 49);
    field_attach_form('commerce_customer_profile', $billing, $form['customer_profile'], $form_state);
    $form['customer_profile']['commerce_customer_address'][\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED][0]['name_block']['#access'] = FALSE;
    $form['customer_profile']['commerce_customer_address'][\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED][0]['#title'] = t('Billing address');
    $form['customer_profile']['commerce_customer_address']['#weight'] = 49;

    if (isset($form_state['values'])) {
      $default_method = $form_state['values']['payment_method'];
    }
    else {
      reset($options);
      $default_method = key($options);
    }

    // Set the default value for the payment method radios.
    $pane_form['payment_method']['#default_value'] = $default_method;

    $payment_method = $payment_methods[$default_method];

    $checkout_pane = array();
    $order = new stdClass();
    $pane_values = array();
    if ($callback = commerce_payment_method_callback($payment_method, 'submit_form')) {
      $pane_form['payment_details'] = $callback($payment_method, $pane_values, $checkout_pane, $order);
    }
    else {
      $pane_form['payment_details'] = array();
    }

    $pane_form['payment_details']['#tree'] = TRUE;
    $pane_form['payment_details']['#prefix'] = '<div id="payment-details">';
    $pane_form['payment_details']['#suffix'] = '</div>';

    $form['commerce_payment_form'] = array(
      '#type' => 'fieldset',
      '#weight' => 50,
      '#title' => t('Payment Information'),
    );

    $form['commerce_payment_form'] = array_merge($form['commerce_payment_form'], $pane_form);
  }

  $params = \Drupal\Component\Utility\UrlHelper::filterQueryParameters();

  if (isset($params['amount'])) {
    // Backwards-compatibility for query param donation amounts (100a9f11c6):
    $params['donation'] = $params['amount'];
  }

  redhen_donation_override_form_default_values($form, $params);

  return $form;
}

function redhen_donation_amount_options_callback($form, $form_state) {
  return $form['donation'];
}

/**
 * Set defaults based on URL query parameters.
 *
 * Takes an element array, and recursively sets form default_values on children
 * based on the specified parameters. (This logic carries with it the technical
 * assumption that query parameters passed to a donation form are *supposed* to
 * set corresponding values on donation forms. If this is not the case, we will
 * need to revisit this implementation.)
 *
 * Note: if there are fields with name collisions, this function will be greedy
 * and set *all* of their values.
 *
 * @array &$form element array
 * @array $params parameters
 */
function redhen_donation_override_form_default_values(&$form, $params) {
  $children = \Drupal\Core\Render\Element::children($form);

  foreach ($children as $key) {
    $form_field = &$form[$key];

    $field_keys = array(\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED, 0, 'value');
    foreach ($field_keys as $field_key) {
      if (isset($form_field[$field_key])) {
        $form_field = &$form_field[$field_key];
      }
    }

    if (is_array($form_field)) {
      if (isset($form_field['#type']) && isset($params[$key])) {
        $form_field['#default_value'] = $params[$key];
      }
      redhen_donation_override_form_default_values($form_field, $params);
    }
  }
}

/**
 * Validation callback for redhen_donation_form().
 */
function redhen_donation_form_validate($form, &$form_state) {
  $donation = $form_state['donation'];

  // Email validation.
  $valid_email = valid_email_address($form_state['values']['email']);
  if (!$valid_email) {
    $message = t('"%mail" is not a valid email address',
      array('%mail' => $form_state['values']['email'])
    );
    form_set_error('email', $message);
  }

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_donation', $donation, $form, $form_state);
  field_attach_form_validate('commerce_commerce_line_item', $donation, $form['line_item'], $form_state);
  if (!$form_state['use_cart']) {
    field_attach_form_validate('commerce_customer_profile', $donation, $form['customer_profile'], $form_state);
    // Validate the values entered for the credit card details:
    if (isset($form['commerce_payment_form']['payment_details']['credit_card'])) {
      module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
      commerce_payment_credit_card_validate($form_state['values']['payment_details']['credit_card'], array(
        'form_parents' => array(
          'payment_details',
          'credit_card',
        ),
      ));
    }
  }
  // Validate 'other' amounts.
  if (empty($form_state['values']['donation'])) {
    form_set_error('donation', t('A donation amount must be entered.'));
  }
  elseif (!is_numeric($form_state['values']['donation'])) {
    form_set_error('donation', t('Please enter a numeric donation amount without any text or currency symbols.'));
  }
  else {
    $form_state['values']['donation'] = commerce_currency_decimal_to_amount($form_state['values']['donation'], $form_state['currency']);
  }
}

/**
 * Submit callback for redhen_donation_form().
 */
function redhen_donation_form_submit($form, &$form_state) {
  $user = \Drupal::currentUser();
  $settings = $form_state['donation_settings'];
  $values = $form_state['values'];
  $donation = $form_state['donation'];
  $donation_type = redhen_donation_type_load($donation->bundle());

  // Contact and User handling.
  $contact = $form_state['redhen_contact'];
  $new_contact = isset($contact->is_new) && $contact->is_new === TRUE;

  // Does a contact already exist with that email address?
  if ($new_contact) {
    // Attempt to load contact based on given email.
    if ($contact_email_search = redhen_contact_load_by_mail($form_state['values']['email'])) {
      $contact = current($contact_email_search);
      $new_contact = FALSE;
    }
  }

  $contact_wrapper = entity_metadata_wrapper('redhen_contact', $contact);

  // If set to update fields, do so. Or if new contact create contact.
  if ($donation_type->settings['contact_settings']['update_fields'] == 1 || $new_contact) {
    // Save default parameters back into the $contact object.
    $contact->first_name = $form_state['values']['first_name'];
    $contact->last_name = $form_state['values']['last_name'];
    // Middle name can be excluded from the form whereas first and last cannot.
    // As such, we shouldn't update the middle name if the field isn't present
    // on the form.
    $contact->middle_name = empty($form_state['values']['middle_name']) ? $contact_wrapper->middle_name->value() : $form_state['values']['middle_name'];

    // Only update if new.
    $contact->author_uid = $new_contact ? $contact->author_uid : $user->uid;

    // Notify field widgets.
    // First name, last name, and email are mandatory - update them.
    $update_fields = array(
      'first_name',
      'last_name',
      'redhen_email'
    );
    // If a Contact address field is selected to sync with the billing profile,
    // update said field on the Redhen Contact with any changes.
    if (isset($donation_type->settings['contact_settings']['profile_selector'])) {
      $update_fields[] = $donation_type->settings['contact_settings']['profile_selector'];
    }
    // Only update fields shown on donation form.
    // This prevents data for all fields not shown on the form getting erased.
    foreach($donation_type->settings['contact_settings']['fields'] as $contact_field => $update) {
      if ($update) {
        $update_fields[] = $contact_field;
      }
    }

    // Run field_attach_submit once for each field we want to update rather
    // than allowing the default behavior of updating all fields in the
    // entity's bundle.
    foreach($update_fields as $update_field) {
      $options = array(
        'field_name' => $update_field,
      );
      field_attach_submit('redhen_contact', $contact, $form, $form_state, $options);
    }

    // Set Address field if needed.
    if (!empty($donation_type->settings['contact_settings']['profile_selector']) &&
      $donation_type->settings['contact_settings']['profile_selector'] != 'billing') {
      $sync_field = $donation_type->settings['contact_settings']['profile_selector'];
      $contact_address = array();
      if (!empty($contact_wrapper->{$sync_field})) {
        // The wrapper can be set, but the value equal to null in some cases.
        // Check for this specific case to avoid a notice. More verbose to
        // support older (< 5.5) PHP versions.
        $contact_address_value = $contact_wrapper->{$sync_field}->value();
        $contact_address = (empty($contact_address_value)) ? array() : $contact_address_value;
      }

      $billing_address = $form_state['values']['commerce_customer_address'][\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED][0];
      $updated_address = array_merge($contact_address, $billing_address);
      $contact_wrapper->{$sync_field}->set($updated_address);

    }

    // Set email address.
    $contact_wrapper->email->set($form_state['values']['email']);

    \Drupal::logger('redhen_donation')->notice('RedHen Contact %contact_id has been updated.', array('%contact_id' => $contact->contact_id));
  }

  // Load product.
  $product_sku = $values['product'];
  $product = commerce_product_load_by_sku($product_sku);
  $product_wrapper = entity_metadata_wrapper('commerce_product', $product);
  $recurring_product = commerce_recurring_product_is_recurring($product);

  $donation_user = $user;
  if ($donation_user->uid == 0) {
    $donation_user = user_load_by_mail($form_state['values']['email']);
  }

  // Link recurring products to the donation user.
  if ($recurring_product && $contact_wrapper->user->value() == NULL) {
    // We are not logged in and no linked user found create a new one.
    if (!$donation_user) {
      $password = user_password();
      $fields = array(
        'name' => preg_replace('/[^a-z0-9@_.\'-]/i', '', $form_state['values']['email']),
        'mail' => $form_state['values']['email'],
        'pass' => $password,
        'status' => 1,
        'init' => $form_state['values']['email'],
        'roles' => array(
          \Drupal\Core\Session\AccountInterface::AUTHENTICATED_RID => 'authenticated user',
        ),
      );
      // @FIXME
// user_save() is now a method of the user entity.
// $donation_user = user_save('', $fields);


      if (!empty($donation_type->settings['commerce_settings']['product_options']['recurring_user_register_notify'])) {
        _user_mail_notify('register_admin_created', $donation_user);
      }
    }
    $contact_wrapper->user->set($donation_user);
    \Drupal::logger('redhen_donation')->notice('User has been linked to the contact %name.', array('%name' => $contact->label()));
  }

  // Save the contact.
  redhen_contact_save($contact);

  // Commerce handling.
  $order_type
    = isset($donation_type->settings['commerce_settings']['order_type']) ?
      $donation_type->settings['commerce_settings']['order_type'] :
      'commerce_order';
  $line_item_type
    = isset($donation_type->settings['commerce_settings']['line_item_type']) ?
      $donation_type->settings['commerce_settings']['line_item_type'] :
      'product';
  $uid = isset($donation_user->uid) ? $donation_user->uid : 0;
  if ($form_state['use_cart']) {
    // Allow dictating of a specific order by donation creator:
    if (isset($form_state['redhen_donation_order_override'])) {
      $order = $form_state['redhen_donation_order_override'];
    }
    else {
      // Otherwise attempt to load the customer's shopping cart order.
      $order = commerce_cart_order_load($uid);
    }
  }
  // If no order existed, create one now.
  if (empty($order)) {
    $order = commerce_cart_order_new($uid, $order_type);
    $order->data['last_cart_refresh'] = REQUEST_TIME;
    if (empty($order->mail)) {
      $order->mail = $contact_wrapper->email->value();
    }
    commerce_order_save($order);
  }
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $line_item = commerce_product_line_item_new($product, 1, $order->order_id, array(), $line_item_type);
  field_attach_submit('commerce_line_item', $line_item, $form['line_item'], $form_state);
  $donation_amount = $form_state['values']['donation'];
  $currency = $form_state['currency'];

  redhen_donation_update_line_item_price($line_item, $donation_amount, $currency);
  // Save the line item to get the ID.
  commerce_line_item_save($line_item);
  // Add the item to the list of line items.
  $order_wrapper->commerce_line_items[] = $line_item;

  if (!$form_state['use_cart']) {
    $customer_profile = commerce_customer_profile_new('billing', $uid);
    $customer_profile->commerce_customer_address[\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED][0] = array();
    $billing_address =& $customer_profile->commerce_customer_address[\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED][0];
    $form_state['values']['commerce_customer_address']['und'][0]['name_line'] = $form_state['values']['first_name'] . ' ' . $form_state['values']['last_name'];
    $billing_address = array_merge($billing_address, $form_state['values']['commerce_customer_address'][\Drupal\Core\Language\Language::LANGCODE_NOT_SPECIFIED][0]);

    commerce_customer_profile_save($customer_profile);
    $order_wrapper->commerce_customer_billing = $customer_profile;
  }
  // Allow other modules to alter the order before it is saved.
  \Drupal::moduleHandler()->invokeAll('redhen_donation_order_alter', [$order_wrapper, $form, $form_state]);
  $order_wrapper->save();
  commerce_order_calculate_total($order);

  if (!$form_state['use_cart']) {
    $order_total = $order_wrapper->commerce_order_total->amount->value();

    // Now we can process the payment at last.
    $payment_method = commerce_payment_method_instance_load($form_state['values']['payment_method']);
    // Delegate submit to the payment method callback.
    if ($callback = commerce_payment_method_callback($payment_method, 'submit_form_submit')) {
      $charge = array(
        'amount' => $order_total,
        'currency_code' => $currency,
      );
      // Payment method form.
      $details_form = !empty($form['commerce_payment_form']['payment_details']) ? $form['commerce_payment_form']['payment_details'] : array();
      // Payment method values.
      $details_values = !empty($form_state['values']['payment_details']) ? $form_state['values']['payment_details'] : $form_state['input']['payment_details'];

      // If recurring, we need a stored card on file for future transactions.
      if ($recurring_product) {
        $details_values['cardonfile_instance_default'] = TRUE;
        $details_values['cardonfile'] = 'new';
        $details_values['credit_card']['cardonfile_store'] = TRUE;
        $details_values['credit_card']['default'] = TRUE;
      }
      commerce_line_item_save($line_item);

      $callback($payment_method, $details_form, $details_values, $order, $charge);
      $transaction = // @FIXME
// To reset the entity cache, use EntityStorageInterface::resetCache().
\Drupal::entityTypeManager()->getStorage('commerce_payment_transaction')->loadByProperties(array('order_id' => $order->order_id));
      $transaction = current($transaction);
      if ($transaction->status != 'success') {
        commerce_order_status_update($order, 'canceled', FALSE, TRUE, t('Payment declined: @message'), array('@message' => $transaction->message));
        \Drupal::messenger()->addError(t('There was a problem with your payment: @message', array('@message' => $transaction->message)));
        // This preserves the form state if a user wants to resubmit.
        $form_state['redirect'] = FALSE;
        return FALSE;
      }
      else {
        commerce_order_status_update($order, 'completed', FALSE, TRUE, t('Payment accepted successful.'));
      }
    }
  }
  // Donation entity handling.
  $donation->author_uid = $user->uid;
  $donation->contact_id = $contact->contact_id;
  $donation->order_id = $order->order_id;
  $donation->line_item_id = $line_item->line_item_id;

  if ($recurring_product) {
    $pledged = redhen_donation_pledged($order_total, $product_wrapper);
    $received = $order_total;
    $donation->transaction_entity_type = 'commerce_recurring';
    $recurring_entity = current(commerce_recurring_load_by_order($order));
    $donation->transaction_entity_id = $recurring_entity->id;
  }
  else {
    $pledged = $received = $order_total;
    $donation->transaction_entity_type = 'commerce_order';
    $donation->transaction_entity_id = $order_wrapper->getIdentifier();
  }
  $donation->pledged = $pledged;
  $donation->received = $received;

  // Notify field widgets.
  field_attach_submit('redhen_donation', $donation, $form, $form_state);

  // Save the donation and redirect.
  if (redhen_donation_save($donation)) {
    \Drupal::logger('redhen_donation')->notice('RedHen Donation %donation_id has been saved.', array('%donation_id' => $donation->donation_id));
    if (!empty($settings['settings']['confirmation_message'])) {
      if (\Drupal::moduleHandler()->moduleExists('token')) {
        $message = \Drupal::token()->replace($settings['settings']['confirmation_message'], array(
          'redhen_donation' => $donation,
          'redhen_contact' => $contact,
        ));
      }
      else {
        $message = $settings['settings']['confirmation_message'];
      }
      // Allow other modules to change the confirmation message.
      \Drupal::moduleHandler()->alter('redhen_donation_confirmation_message', $message);
      \Drupal::messenger()->addMessage($message);
    }
    if (!isset($form_state['redirect'])) {
      if (!empty($settings['settings']['confirmation_page'])) {
        $form_state['redirect'] = array($settings['settings']['confirmation_page']);
      }
    }
    if (!$form_state['use_cart']) {
      rules_invoke_event('redhen_donation_donation_completed', $donation, $transaction, $line_item, $product);
    }
  }
  else {
    \Drupal::messenger()->addMessage(t('There was a problem submitting your donation.'));
    \Drupal::logger('redhen_donation')->notice('There was a problem with submitting a RedHen Donation for RedHen Contact %contact_id', array('%contact_id' => $contact->contact_id));
  }
}

/**
 * Themes the payment methods on a fieldmap edit form into a table.
 */
function theme_redhen_donation_payment_options_form_table($variables) {
  $elements = $variables['elements'];

  // Build the rows array.
  $columns = isset($elements['#columns']) ? $elements['#columns'] : (isset($elements['#header']) ? array_keys($elements['#header']) : array());
  $rows = array();
  foreach (\Drupal\Core\Render\Element::children($elements) as $child_key) {
    $child = & $elements[$child_key];
    $data = array();
    $row_columns = empty($columns) ? \Drupal\Core\Render\Element::children($child) : $columns;
    foreach ($row_columns as $column) {
      if (isset($child[$column])) {
        $cell = array('data' => \Drupal::service("renderer")->render($child[$column]));
        if (isset($child[$column]['#attributes'])) {
          foreach ($child[$column]['#attributes'] as $key => $value) {
            $cell[$key] = ($key == 'id') ? (is_array($value) ? array($value[0] . '-cell') : $value . '-cell') : $value;
          }
        }
        $data[] = $cell;
      }
    }
    $row = array('data' => $data);
    if (isset($child['#attributes'])) {
      foreach ($child['#attributes'] as $key => $value) {
        $row[$key] = $value;
      }
    }
    $rows[] = $row;
  }

  $config = array(
    'rows' => $rows,
  );
  if (isset($elements['#header'])) {
    $config['header'] = $elements['#header'];
  }
  if (isset($elements['#attributes']) && is_array($elements['#attributes'])) {
    $config['attributes'] = $elements['#attributes'];
  }

  // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// return theme('table', $config);

}

/**
 * Payment method selector helper.
 *
 * @array $form_state
 *   Form state array.
 *
 * @return array|null
 *   Array of payment methods or null if no methods found.
 */
function redhen_donation_get_payment_methods(&$form_state) {
  $methods = NULL;
  if (isset($form_state['methods'])) {
    $methods = &$form_state['methods'];
  }

  if (empty($methods)) {
    $methods = array(array());
  }
  if (isset($form_state['triggering_element'])) {
    if ($form_state['triggering_element']['#name'] == 'donation_add_field') {
      $methods[] = array();
    }
    if (isset($form_state['triggering_element']['#ajax']['delta']) &&
      $form_state['triggering_element']['#name'] == 'delete_method-' . $form_state['triggering_element']['#ajax']['delta']) {
      $delta = $form_state['triggering_element']['#ajax']['delta'];
      unset($methods[$delta]);
    }
  }

  // Ensure one method exists.
  $empty  = array_filter($methods);
  $methods = empty($methods) && empty($empty) ? array(array()) : $methods;

  return $methods;
}

/**
 * Donation form AJAX callback.
 *
 * @array $form
 *   Form array.
 * @array $form_state
 *   Form state array.
 *
 * @return array
 *   AJAX command array.
 */
function redhen_donation_form_refresh(&$form, &$form_state) {
  $html1 = \Drupal::service("renderer")->render($form['commerce_payment_form']['payment_details']);
  $html2 = \Drupal::service("renderer")->render($form['donation']);
  $result = array(
    '#type' => 'ajax',
    '#commands' => array(
      ajax_command_replace('#payment-details', $html1),
      ajax_command_replace('.form-item-donation', $html2),
    ),
  );
  return $result;
}

/**
 * Donation line item update callback.
 *
 * @param object $line_item
 *   Commerce Line Item.
 * @param string $decimal_amount
 *   Item amount.
 * @param string $currency
 *   Item currency.
 */
function redhen_donation_update_line_item_price(&$line_item, $decimal_amount, $currency = 'USD') {
  $wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
  $wrapper->commerce_unit_price->currency_code = $currency;

  $wrapper->commerce_unit_price->amount->set($decimal_amount);
  $wrapper->commerce_unit_price->data = commerce_price_component_delete(0, 'base_price');

  $wrapper->commerce_unit_price->data = commerce_price_component_add(
    $wrapper->commerce_unit_price->value(),
    'base_price',
    array(
      'amount' => $decimal_amount,
      'currency_code' => $currency,
      'data' => array(),
    ),
    TRUE
  );
}

/**
 * Production description helper callback.
 *
 * @param object $product_wrapper
 *   Product Entity Metadata wrapper.
 *
 * @return null|string
 *   Product description.
 */
function redhen_donation_product_description($product_wrapper) {
  $description = '';
  if ($product_wrapper->type->value() == 'recurring') {
    $period = $product_wrapper->commerce_recurring_rec_period->period->value();
    $interval = $product_wrapper->commerce_recurring_rec_period->interval->value();
    $end_period = $product_wrapper->commerce_recurring_end_period->value() ?
      $product_wrapper->commerce_recurring_end_period->period->value() : '';
    $end_interval = $product_wrapper->commerce_recurring_end_period->value() ?
      $product_wrapper->commerce_recurring_end_period->interval->value() : '';
    $description = t('@interval recurrence per @period', array(
      '@interval' => $interval,
      '@period' => $period,
    ));
    if ($end_period != '') {
      $description .= ' ' . t('for @end_interval @end_period(s)', array(
        '@end_period' => $end_period,
        '@end_interval' => $end_interval,
      ));
    }

  }
  return $description;
}

/**
 * Helper function to determine amount pledged.
 *
 * @param string $order_total
 *   Order total.
 * @param object $product_wrapper
 *   Product Entity Metatdata wrapper.
 *
 * @return mixed
 *   Pledged amount.
 */
function redhen_donation_pledged($order_total, $product_wrapper) {
  $period = $product_wrapper->commerce_recurring_rec_period->period->value();
  $interval = $product_wrapper->commerce_recurring_rec_period->interval->value();
  $end_period = $product_wrapper->commerce_recurring_end_period->value() ?
    $product_wrapper->commerce_recurring_end_period->period->value() : '';
  $end_interval = $product_wrapper->commerce_recurring_end_period->value() ?
    $product_wrapper->commerce_recurring_end_period->interval->value() : '';

  $duration_array = array(
    'day' => 365,
    'week' => 52,
    'month' => 12,
    'year' => 1,
  );
  $interval_count = $duration_array[$period] / $interval;
  if ($end_period != '') {
    $interval_per_year = 1 / ($duration_array[$end_period] / $end_interval);
    $recurring_count = round($interval_count * $interval_per_year);
    $return = $order_total * $recurring_count;
  }
  else {
    $return = $order_total;
  }

  return $return;
}

/**
 * Get an array with all possible Drupal properties for a given entity type.
 *
 * @param string $entity_type
 *   Name of entity whose properties to list.
 * @param string $entity_bundle
 *   Optional bundle to limit available properties.
 * @param bool $required
 *   Set to TRUE if properties are required.
 * @param string $prefix
 *   Optional prefix for option IDs in the options list.
 * @param string $tree
 *   Optional name of the parent element if this options list is part of a tree.
 *
 * @return array
 *   List of properties that can be used as an #options list.
 */
function redhen_donation_fieldmap_options($entity_type, $entity_bundle = NULL, $required = FALSE, $prefix = NULL, $tree = NULL) {
  $options = array();
  if (!$prefix) {
    $options[''] = t('Specify Manually / None');
  }

  $properties = entity_get_all_property_info($entity_type);
  if (isset($entity_bundle)) {
    $info = entity_get_property_info($entity_type);
    $properties = $info['properties'];
    if (isset($info['bundles'][$entity_bundle])) {
      $properties += $info['bundles'][$entity_bundle]['properties'];
    }
  }

  foreach ($properties as $key => $property) {
    $keypath = $prefix ? $prefix . ':' . $key : $key;
    $type = isset($property['type']) ? entity_property_extract_innermost_type($property['type']) : 'text';
    if ($type == 'date' || $type == 'entity') {
      $is_entity = ($type == 'entity') || (bool) \Drupal::entityTypeManager()->getDefinition($type);
      $required_property = isset($property['required']) ? $property['required'] : FALSE;
      $computed_property = isset($property['computed']) ? $property['computed'] : FALSE;
      if ($is_entity) {
        // We offer fields on related entities (useful for field collections).
        // But we only offer 1 level of depth to avoid loops.
        if (!$prefix) {
          $bundle = isset($property['bundle']) ? $property['bundle'] : NULL;
          $options[$property['label']] = redhen_donation_fieldmap_options($type, $bundle, $required, $keypath, $property['label']);
        }
      }
      elseif (!$required || $required_property || $computed_property) {
        if (isset($property['field']) && $property['field'] && !empty($property['property info'])) {
          foreach ($property['property info'] as $sub_key => $sub_prop) {
            $label = isset($tree) ? $tree . ' - ' . $property['label'] : $property['label'];
            $options[$label][$keypath . ':' . $sub_key] = $sub_prop['label'];
          }
        }
        else {
          $options[$keypath] = $property['label'];
        }
      }
    }
  }
  return $options;
}

/**
 * Form callback: Create a new donation record outside of the checkout process.
 */
function redhen_donation_add_form($form, &$form_state, $entity_type, $entity) {

  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $donation = redhen_donation_initialize_donation($entity, $entity_type, $entity_id, \Drupal::currentUser()->uid);
  // Add the donation information edit form.
  $form += redhen_donation_edit_form($form, $form_state, $donation);

  // @TODO: Nearly the same logic as in redhen_donation_form().
  module_load_include('inc', 'redhen_contact', 'includes/redhen_contact.forms');
  $donation_type = redhen_donation_type_load($donation->bundle());
  $contact_settings = $donation_type->settings['contact_settings'];
  $contact_object = redhen_contact_create(array('type' => $contact_settings['contact_type']));

  $form['redhen_contact'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact Information'),
    '#weight' => -10,
  );
  // Add all the non-# fields from the redhen contact form.
  $contact_form = redhen_contact_contact_form(array(), $form_state, $contact_object);
  foreach (\Drupal\Core\Render\Element::children($contact_form) as $key) {
    if ($key != 'actions') {
      $form['redhen_contact'][$key] = $contact_form[$key];
    }
  }

  return $form;
}

/**
 * Validation callback for redhen_donation_add_form().
 */
function redhen_donation_add_form_validate($form, &$form_state) {
  // Allow the edit form validate the fields that it added.
  redhen_donation_edit_form_validate($form, $form_state);
}

/**
 * Submit callback for redhen_donation_add_form().
 */
function redhen_donation_add_form_submit($form, &$form_state) {

  // Upsert the redhen contact and include its id in the donation object.
  $contact = $form_state['redhen_contact'];
  $contact->author_uid = \Drupal::currentUser()->uid;
  $contact->first_name = $form_state['values']['first_name'];
  $contact->middle_name = $form_state['values']['middle_name'];
  $contact->last_name = $form_state['values']['last_name'];
  field_attach_submit('redhen_contact', $contact, $form, $form_state);
  $contact->upsert();
  $form_state['donation']->contact_id = $form_state['redhen_contact']->contact_id;

  // Allow the edit form to process the fields it added.
  redhen_donation_edit_form_submit($form, $form_state);

  // Redirect back to the donation list after the donation is added.
  $form_state['redirect'] = str_replace("donations/add", "donations", \Drupal\Core\Url::fromRoute("<current>")->toString());
}

/**
 * Form callback: edit the field for a donation.
 *
 * @param array $form
 *   Form array.
 * @param array $form_state
 *   Form state array.
 * @param RedhenDonation $donation
 *   The donation object to edit.
 *
 * @return mixed
 *   Donation form array.
 */
function redhen_donation_edit_form($form, &$form_state, RedhenDonation $donation) {
  // Save a reference to the donation.
  $form_state['donation'] = $donation;

  $form['pledged'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount Pledged'),
    '#size' => 10,
    '#required' => TRUE,
    '#default_value' => number_format($donation->pledged / 100, 2, '.', ''),
  );
  $form['received'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount Received'),
    '#size' => 10,
    '#required' => TRUE,
    '#default_value' => number_format($donation->received / 100, 2, '.', ''),
  );

  // Attach the field api fields for the entity.
  field_attach_form('redhen_donation', $donation, $form, $form_state);

  // Add the action buttons to the bottom of the form.
  $form['actions']['#weight'] = 100;
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save donation'),
  );
  return $form;
}

/**
 * Validation callback for redhen_donation_edit_form().
 */
function redhen_donation_edit_form_validate($form, &$form_state) {
  $donation = $form_state['donation'];

  if (!is_numeric($form_state['values']['pledged'])) {
    form_set_error('pledged', t('Amount pledged must be a decimal number with no currency symbols or commas.'));
  }
  if (!is_numeric($form_state['values']['received'])) {
    form_set_error('received', t('Amount received must be a decimal number with no currency symbols or commas.'));
  }

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_donation', $donation, $form, $form_state);
}

/**
 * Submit callback for redhen_donation_edit_form().
 */
function redhen_donation_edit_form_submit($form, &$form_state) {
  $donation = $form_state['donation'];

  // Multiply the donation amount by 100 to store as total cents.
  $donation->pledged = $form_state['values']['pledged'] * 100;
  $donation->received = $form_state['values']['received'] * 100;

  // Notify field widgets.
  field_attach_submit('redhen_donation', $donation, $form, $form_state);

  if (redhen_donation_save($donation)) {
    \Drupal::messenger()->addMessage("The donation details has been saved.");
  }
}

/**
 * Helper function to load products based on a donation type config.
 */
function redhen_donation_load_configured_skus($option_settings) {
  if (isset($option_settings['recurring'])) {
    $product_skus = array_merge(
      array(
        $option_settings['one_time_product'],
      ),
      $option_settings['recurring']
    );
  }
  else {
    $product_skus = array($option_settings['one_time_product']);
  }
  $products = array();
  foreach ($product_skus as $sku) {
    if ($sku) {
      $products[$sku] = commerce_product_load_by_sku($sku);
    }
  }
  return $products;
}
