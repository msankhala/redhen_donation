<?php /**
 * @file
 * Contains \Drupal\redhen_donation\Plugin\Field\FieldFormatter\RedhenDonationDefault.
 */

namespace Drupal\redhen_donation\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * @FieldFormatter(
 *  id = "redhen_donation_default",
 *  label = @Translation("Default"),
 *  field_types = {"redhen_donation"}
 * )
 */
class RedhenDonationDefault extends FormatterBase {

  /**
   * @FIXME
   * Move all logic relating to the redhen_donation_default formatter into this
   * class. For more information, see:
   *
   * https://www.drupal.org/node/1805846
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterInterface.php/interface/FormatterInterface/8
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterBase.php/class/FormatterBase/8
   */

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode = NULL) {
//      $elements = array();
//      $countries = \Drupal::service('country_manager')->getList();
//      foreach ($items as $delta => $item) {
//        if (isset($countries[$item->value])) {
//          $elements[$delta] = array('#markup' => $countries[$item->value]);
//        }
//      }
//      return $elements;
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
            $element[0] = array(
              '#markup' => theme('redhen_donation_link',
                array(
                  'label' => $label,
                  'path' => $uri['path'] . '/donate',
                )
              ),
            );
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
            $element[0] = drupal_get_form('redhen_donation_form', $donation, $use_cart);
          }
          break;
      }
    }

    return $element;

  }
  /**
   * {@inheritdoc}
   */
  public function settingsSummary() {
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
   * {@inheritdoc}
   */
  public function settingsForm(array $form, FormStateInterface $form_state) {
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
}
