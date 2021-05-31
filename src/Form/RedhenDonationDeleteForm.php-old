<?php

namespace Drupal\redhen_donation\Form;

use Drupal\Core\Entity\ContentEntityDeleteForm;
use Drupal\redhen_donation\Entity\RedhenDonation;

/**
 * Provides a form for deleting Redhen Donation entities.
 *
 * @ingroup redhen_donation
 */
class RedhenDonationDeleteForm extends ContentEntityDeleteForm {

  /**
   * {@inheritdoc}
   */
  public function getQuestion() {
    return $this->t('Are you sure you want to delete the @redhen-donation-type %name?', [
      '@redhen-donation-type' => RedhenDonation::load($this->entity->bundle())->label(),
      '%name' => $this->entity->label(),
    ]);
  }

}
