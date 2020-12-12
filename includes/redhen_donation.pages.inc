<?php
/**
 * @file
 * Donation pages include.
 */

/**
 * Title callback. Also allows alteration of the title.
 */
function redhen_donation_donation_title($entity_type, $entity) {
  $title = "Donate";
  \Drupal::moduleHandler()->alter('redhen_donation_title', $title, $entity_type, $entity);
  return $title;
}

/**
 * Page callback: Add a new donation to a host entity.
 *
 * @param string $entity_type
 *   The host entity type.
 * @param object $entity
 *   The host entity.
 *
 * @return array
 *   A render array
 *
 * @see redhen_donation_donate_page_access()
 * @see redhen_donation_menu()
 */
function redhen_donation_donate_page($entity_type, $entity) {
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  if (redhen_donation_status($entity_type, $entity_id)) {
    $donation = redhen_donation_initialize_donation($entity, $entity_type, $entity_id);
    return \Drupal::formBuilder()->getForm('redhen_donation_form', $donation);
  }
  else {
    return t('Sorry, donations are no longer available for %name',
      array('%name' => $entity->label()));
  }
}

/**
 * Page callback for entity donation settings.
 *
 * @param string $entity_type
 *   Host entity type.
 * @param object $entity
 *   Host entity.
 *
 * @return array
 *   Donation entity settings form.
 */
function redhen_donation_entity_settings_page($entity_type, $entity, $field) {
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $settings = redhen_donation_entity_settings($entity_type, $entity_id);
  return \Drupal::formBuilder()->getForm('redhen_donation_entity_settings_form', $settings, $entity_type, $entity_id, NULL, $field);
}

/**
 * Page callback: Show a list of donations for a host entity.
 *
 * @param string $entity_type
 *   The host entity type.
 * @param object $entity
 *   The host entity.
 *
 * @return array
 *   A render array
 *
 * @see redhen_donation_administer_donations_access()
 * @see redhen_donation_menu()
 */
function redhen_donation_donations_page($entity_type, $entity) {
  $header = array(
    array(
      'data' => t('ID'),
      'field' => 'donation_id',
      'type' => 'property',
      'specifier' => 'donation_id',
    ),
    array(
      'data' => t('Author'),
    ),
    array(
      'data' => t('Contact'),
    ),
    array(
      'data' => t('Amount Pledged'),
    ),
    array(
      'data' => t('Amount Received'),
    ),
    array(
      'data' => t('Updated'),
      'field' => 'updated',
      'sort' => 'desc',
      'type' => 'property',
      'specifier' => 'updated',
    ),
    array(
      'data' => t('Transaction Entity'),
    ),
    array(
      'data' => t('Transaction Status'),
    ),
  );

  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $label = $entity->label();

  $query = new EntityFieldQuery();
  $result = $query
    ->entityCondition('entity_type', 'redhen_donation')
    ->propertyCondition('entity_id', $entity_id)
    ->propertyCondition('entity_type', $entity_type)
    ->pager(20)
    ->tableSort($header)
    ->execute();

  if (!empty($result['redhen_donation'])) {
    $donations = redhen_donation_load_multiple(array_keys($result['redhen_donation']));
    $rows = array();

    foreach ($donations as $donation) {
      $wrapper = entity_metadata_wrapper('redhen_donation', $donation);

      if ($donation->author_uid) {
        $user = $wrapper->author->value();
        $uri = entity_uri('user', $user);
        // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $user_col = l($user->name, $uri['path']);

      }
      else {
        $user_col = t('Anonymous');
      }

      if ($donation->transaction_entity_id) {
        $transaction_entity_wrapper = $wrapper->transaction_entity;
        $transaction_entity = $transaction_entity_wrapper->value();
        $transaction_entity_type = $wrapper->transaction_entity_type->value();
        $transaction_entity_uri = entity_uri($transaction_entity_type, $transaction_entity);
        $transaction_entity_label = $transaction_entity->label();
        if (isset($transaction_entity_uri['path'])) {
          // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $transaction_column = l(
//             $transaction_entity_label ?: t('Recurring') . ' ' .
//               $donation->transaction_entity_id,
//             $transaction_entity_uri['path']
//           );

        }
        else {
          $transaction_column = $transaction_entity_label ?: 'Recurring ' . $donation->transaction_entity_id;
        }
      }
      else {
        $transaction_column = 'Transaction Deleted';
      }
      $donation_uri = $donation->uri();
      if ($contact = $wrapper->contact->value()) {
        $contact_uri = $contact->uri();
        // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $contact_column = l($wrapper->contact->label(), $contact_uri['path']);

      }
      else {
        $contact_column = t('Contact Deleted');
      }
      // @FIXME
// l() expects a Url object, created from a route name or external URI.
// $rows[] = array(
//         l($donation->donation_id, $donation_uri['path']),
//         $user_col,
//         $contact_column,
//         $wrapper->pledged->value(),
//         $wrapper->received->value(),
//         format_date($donation->updated),
//         $transaction_column,
//         $wrapper->status->value(),
//       );

    }

    $table = array(
      'header' => $header,
      'rows' => $rows,
    );

    // @FIXME
// theme() has been renamed to _theme() and should NEVER be called directly.
// Calling _theme() directly can alter the expected output and potentially
// introduce security issues (see https://www.drupal.org/node/2195739). You
// should use renderable arrays instead.
// 
// 
// @see https://www.drupal.org/node/2195739
// $out = theme('table', $table) . theme('pager');

  }
  else {
    $out = t('There are no donations for %name', array('%name' => $label));
  }

  return $out;
}

/**
 * RedHen Donation view.
 */
function redhen_donation_view(RedhenDonation $donation, $view_mode = 'full') {
  return $donation->view($view_mode);
}
