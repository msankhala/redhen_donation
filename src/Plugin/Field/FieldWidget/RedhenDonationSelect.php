<?php

namespace Drupal\redhen_donation\Plugin\Field\FieldWidget;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Field\FieldItemListInterface;

/**
 * @FieldWidget(
 *  id = "redhen_donation_select",
 *  label = @Translation("Donation type"),
 *  field_types = {"redhen_donation"}
 * )
 */
class RedhenDonationSelect extends WidgetBase {

  /**
   * @FIXME
   * Move all logic relating to the redhen_donation_select widget into this class.
   * For more information, see:
   *
   * https://www.drupal.org/node/1796000
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21WidgetInterface.php/interface/WidgetInterface/8
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21WidgetBase.php/class/WidgetBase/8
   */
  public static function defaultSettings() {
    return [
      'size' => '60',
      'autocomplete_route_name' => 'country.autocomplete',
      'placeholder' => '',
    ] + parent::defaultSettings();
  }

  /**
   *
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {
    // $countries = \Drupal::service('country_manager')->getList();
    //    $element['value'] = $element + array(
    //        '#type' => 'select',
    //        '#options' => $countries,
    //        '#empty_value' => '',
    //        '#default_value' => (isset($items[$delta]->value) && isset($countries[$items[$delta]->value])) ? $items[$delta]->value : NULL,
    //        '#description' => t('Select a country'),
    //      );
    $element['redhen_donation_type'] = $element + [
      '#type' => 'select',
    // $options,
      '#options' => [],
      '#default_value' => isset($items[$delta]) ? $items[$delta] : [],
    ];

    ;
    // Force some help text into the field, appending anything the user added.
    $element['#description'] .= ' ' . t('Select what type of donation should be enabled for this @type.', ['@type' => 'donation']);
    // $element['#description'] .= ' ' . t('Select what type of donation should be enabled for this @type.', ['@type' => $instance['bundle']]);.
    return $element;
  }

}
