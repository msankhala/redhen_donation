<?php

/**
 * @file
 * API documentation for Redhen Dontations module.
 */

/**
 * Override redhen_donation_access with custom access control logic.
 *
 * @param string $op
 *   Access operation.
 * @param RedhenDonation $donation
 *   RedHen Donation object.
 * @param object $account
 *   User account.
 *
 * @return bool
 *   Access boolean.
 */
function hook_redhen_donation_access($op, $donation, $account = NULL) {
  if ($donation->user_uid == $account->uid) {
    return TRUE;
  }
}


/**
 * Provide a form API element exposed as a Donation entity setting.
 *
 * @param array $settings
 *   Existing settings values.
 *
 * @return array
 *   A FAPI array for a donation setting.
 */
function hook_redhen_donation_entity_settings($settings) {
  return array(
    'redhen_donation_entity_access_roles' => array(
      '#type' => 'checkboxes',
      '#title' => t('Roles'),
      '#description' => t('Override default access control settings by selecting which roles can donate.'),
      '#options' => user_roles(),
      '#default_value' => isset($settings['settings']['redhen_donation_entity_access_roles']) ? $settings['settings']['redhen_donation_entity_access_roles'] : NULL,
    ),
  );
}

/**
 * Allow modules to customize the title of donation pages.
 */
function hook_redhen_donation_title_alter(&$title, $entity_type, $entity) {
}

/**
 * Provide access to a donation after each save.
 *
 * Useful for calculating running totals.
 *
 * @param RedhenDonation $donation
 *   Redhen Donation Object.
 */
function hook_redhen_donation_save($donation) {
}

/**
 * Provides a way to alter access to the donation status.
 *
 * @param string $status
 *   The current status.
 * @param array $context
 *   Contextual information about the item being altered:
 *   - 'entity_type': The host entity type.
 *   - 'entity_id': The host entity ID.
 *   - 'errors'(optional) An array of error message strings.
 */
function hook_redhen_donation_status_alter($status, $context) {

}

/**
 * Allow overriding of the default contact associated with a Redhen Donation.
 *
 * @param RedhenContact $contact
 *   The default contact as loaded by Redhen Donation.
 * @param User $user
 *   The current user (used to load the relevant contact).
 * @param RedhenDonationType $donation_type
 *   The type of Donation being made.
 */
function hook_redhen_donation_default_contact_alter(&$contact, $user, $donation_type) {
}

/**
 * Allow altering the order created by Redhen Donation.
 *
 * @param EntityMetadataWrapper $order_wrapper
 *   The commerce order as an EntityMetadataWrapper.
 * @param $form
 *   The Redhen donation form
 * @param $form_state
 *   The submitted Redhen donation form
 */
function hook_redhen_donation_order_alter($order_wrapper, $form, $form_state) {
  // Add a item to the order.
  $order_wrapper->commerce_line_items[] = $line_item;
}

/**
 * Allow other modules to change the confirmation message.
 *
 * @param string $message
 */
function hook_redhen_donation_confirmation_message_alter(&$message) {
}
