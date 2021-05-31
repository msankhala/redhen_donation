<?php

namespace Drupal\redhen_donation\Plugin\Field\FieldType;

use Drupal\Core\Field\FieldItemBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\TypedData\DataDefinition;
use Drupal\Core\Field\FieldStorageDefinitionInterface;

/**
 * Plugin implementation of the 'redhen_donation' field type.
 *
 * @FieldType(
 *   id = "redhen_donation",
 *   label = @Translation("Redhen Donation"),
 *   description = @Translation("Enables donations of a selected type for an entity."),
 *   category = @Translation("Redhen"),
 *   default_widget = "redhen_donation_select",
 *   default_formatter = "redhen_donation_default"
 * )
 */
class RedhenDonation extends FieldItemBase {

  /**
   * {@inheritdoc}
   */
  public static function schema(FieldStorageDefinitionInterface $field_definition) {
    return [
      'columns' => [
        'redhen_donation_type' => [
          'type' => 'varchar',
          'length' => 128,
          'not null' => FALSE,
        ],
      ],
      'indexes' => [
        'redhen_donation_type' => ['redhen_donation_type'],
      ],
    ];
  }

  /**
   * {@inheritdoc}
   */
  public static function propertyDefinitions(FieldStorageDefinitionInterface $field_definition) {
    $properties['redhen_donation_type'] = DataDefinition::create('string')
      ->setLabel(t('Redhen Donation type'));
    return $properties;
  }

  /**
   * {@inheritdoc}
   */
  public function isEmpty() {
    $value = $this->get('redhen_donation_type')->getValue();
    return $value === NULL || $value === '';
  }

  /**
   * {@inheritdoc}
   */
  public function fieldSettingsForm(array $form, FormStateInterface $form_state) {
    // kint(get_class_methods($this));
    // kint($this->getFieldDefinition()->getSettings());
    // kint($this->getFieldDefinition()->getTargetEntityTypeId());
    $entityType = $this->getFieldDefinition()->getTargetEntityTypeId();
    // kint($this->getFieldDefinition()->getTargetBundle());
    $bundle = $this->getFieldDefinition()->getTargetBundle();
    // $values = $form_state->getValues();
    // $settings = $this->getSettings();
    $default_settings = isset($values['settings']['default_redhen_donation_settings']) ? $values['settings']['default_redhen_donation_settings'] : [];
    $form['donate_tab'] = [
      '#type' => 'checkbox',
      '#title' => t('Enable Donate Tab'),
      '#default_value' => $form_state->getValue('donate_tab', TRUE),
      '#required' => FALSE,
      '#description' => t('Enable a tab on the content displaying the donation form.'),
    ];
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
    $form['default_redhen_donation_settings'] = [
      '#type' => 'fieldset',
      '#title' => t('Default Donation settings'),
      '#collapsible' => TRUE,
      '#description' => t("These settings will be applied when an entity with this field is saved and does not yet have it's own settings applied."),
    ];

    $settings_form = redhen_donation_entity_settings_form($form['default_redhen_donation_settings'], $form_state, $default_settings, $entityType, NULL, $bundle);
    // redhen_donation_entity_settings_form($form['default_redhen_donation_settings'], $form_state, $settings, $entity_type = NULL, $entity_id = NULL, $bundle = NULL, $field = NULL);.
    // Unset the save button just in case.
    unset($settings_form['save']);

    $form['default_redhen_donation_settings'] += $settings_form;

    // @todo validation
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  // Public function getConstraints() {
  //    $constraint_manager = \Drupal::typedDataManager()->getValidationConstraintManager();
  //    $constraints = parent::getConstraints();
  //    $constraints[] = $constraint_manager->create('ComplexData', [
  //      'value' => [
  //        'Length' => [
  //          'max' => static::COUNTRY_ISO2_MAXLENGTH,
  //          'maxMessage' => t('%name: the country iso-2 code may not be longer than @max characters.', ['%name' => $this->getFieldDefinition()->getLabel(), '@max' => static::COUNTRY_ISO2_MAXLENGTH]),
  //        ],
  //      ],
  //    ]);
  //    return $constraints;
  //  }.
}
