<?php
namespace Drupal\redhen_donation\Entity;

use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\redhen_donation\RedhenDonationInterface;

/**
 * Defines the Redhen Donation entity.
 *
 * @ingroup redhen_donation
 *
 * @ContentEntityType(
 *   id = "redhen_donation",
 *   label = @Translation("RedhenDonation"),
 *   label_singular = @Translation("RedhenDonation"),
 *   label_plural = @Translation("RedhenDonations"),
 *   label_count = @PluralTranslation(
 *     singular = "@count RedhenDonation",
 *     plural = "@count RedhenDonation",
 *   ),
 *   bundle_label = @Translation("RedhenDonation type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\redhen_donation\RedhenDonationListBuilder",
 *     "views_data" = "Drupal\views\EntityViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\redhen_donation\Form\RedhenDonationForm",
 *       "add" = "Drupal\redhen_donation\Form\RedhenDonationForm",
 *       "edit" = "Drupal\redhen_donation\Form\RedhenDonationForm",
 *       "delete" = "Drupal\redhen_donation\Form\RedhenDonationDeleteForm",
 *     },
 *     "access" = "Drupal\redhen_donation\DonationAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_donation\DonationHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "redhen_donation",
 *   revision_table = "redhen_donation_revision",
 *   admin_permission = "administer redhen donation entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "revision" = "revision_id",
 *     "bundle" = "type",
 *     "uuid" = "uuid",
 *     "langcode" = "langcode",
 *     "status" = "status",
 *   },
 *   links = {
 *     "canonical" = "/redhen/donation/{redhen_donation}",
 *     "add-form" = "/redhen/donation/add/{redhen_donation_type}",
 *     "edit-form" = "/redhen/donation/{redhen_donation}/edit",
 *     "delete-form" = "/redhen/donation/{redhen_donation}/delete",
 *     "collection" = "/redhen/donation",
 *   },
 *   bundle_entity_type = "redhen_donation_type",
 *   field_ui_base_route = "entity.redhen_donation_type.edit_form"
 * )
 */
class RedhenDonation extends ContentEntityBase implements RedhenDonationInterface {
  use EntityChangedTrait;

  /**
   * {@inheritdoc}
   */
  public function label()
  {
    return $this->getName();
  }

  /**
   * {@inheritdoc}
   */
  public function getName()
  {
    $name = $this->get('name')->value;
    // Allow other modules to alter the name of the org.
    \Drupal::moduleHandler()->alter('redhen_org_name', $name, $this);
    return $name;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name)
  {
    $this->set('name', $name);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getType()
  {
    return $this->bundle();
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime()
  {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp)
  {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function isActive()
  {
    return (bool) $this->getEntityKey('status');
  }

  /**
   * {@inheritdoc}
   */
  public function setActive($active)
  {
    $this->set('status', $active ? REDHEN_ORG_ACTIVE : REDHEN_ORG_INACTIVE);
    return $this;
  }

}
class RedhenDonation extends Entity {

  // @codingStandardsIgnoreStart
  public $donation_id;
  public $type;
  public $entity_id;
  public $entity_type;
  public $author_uid;
  public $contact_id;
  public $order_id;
  public $line_item_id;
  public $status;
  public $transaction_entity_type;
  public $transaction_entity_id;
  public $pledged;
  public $received;
  public $created;
  public $updated;
  // @codingStandardsIgnoreEnd

  /**
   * Override parent constructor with entity type.
   *
   * @param array $values
   *   Entity values to populate object.
   */
  // public function __construct(array $values = array()) {
  //   parent::__construct($values, 'redhen_donation');
  // }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  // protected function defaultLabel() {
  //   $wrapper = entity_metadata_wrapper('redhen_donation', $this);
  //   return t('Donation for !title', array('!title' => $wrapper->entity->label()));
  // }

  /**
   * Add content for RedHen donation custom properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content = parent::buildContent($view_mode, $langcode);
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);
    $author = $wrapper->author->value();
    $transaction_entity_wrapper = $wrapper->transaction_entity;
    $host_entity = $wrapper->entity->value();

    $host_label = entity_label($this->entity_type, $host_entity);

    $host_uri = $host_entity ? entity_uri($this->entity_type, $host_entity) : NULL;

    // Link to host entity.
    $content['host_entity_link'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Campaign'),
      '#items' => array(
        array(
          '#markup' => l($host_label, $host_uri['path']),
        ),
      ),
      '#weight' => -10,
      '#classes' => 'field field-label-inline clearfix',
    );

    $contact_uri = entity_uri('redhen_contact', $wrapper->contact->value());
    $content['contact'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Donor'),
      '#items' => array(
        array(
          '#markup' => l($wrapper->contact->label(), $contact_uri['path']),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $author_uri = entity_uri('user', $author);
    $content['author'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('User'),
      '#items' => array(
        array(
          '#markup' => $author->uid == 0 ? t('Anonymous') : l($author->name, $author_uri['path']),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#access' => user_access('administer users'),
    );

    $content['pledged'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Pledged'),
      '#items' => array(
        array(
          '#markup' => $wrapper->pledged->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['received'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Received'),
      '#items' => array(
        array(
          '#markup' => $wrapper->received->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['updated'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Updated'),
      '#items' => array(
        array(
          '#markup' => format_date($this->updated),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['status'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Status'),
      '#items' => array(
        array(
          '#markup' => $wrapper->status->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    if (isset($this->transaction_entity_type) && isset($this->transaction_entity_id)) {
      $content['transaction'] = array(
        '#type' => 'fieldset',
        '#title' => t('Transaction(s)'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
      );

      $rows = array();
      $header = array(t('Order ID'), t('Amount'), t('Order Date'), t('Status'));
      $order_status = commerce_order_statuses();
      switch ($this->transaction_entity_type) {
        case 'commerce_recurring':
          $transaction_entity = $transaction_entity_wrapper->value();
          $transaction_entity_uri = entity_uri($this->transaction_entity_type, $transaction_entity);
          foreach ($transaction_entity_wrapper->commerce_recurring_order->value() as $order) {
            $rows[] = $this->transactionRow($order, $order_status);
          }
          $product_wrapper = $transaction_entity_wrapper->commerce_recurring_ref_product;
          $transactions = array(
            'recurring_donation' => array(
              '#type' => 'fieldset',
              '#title' => l(
                t('Recurring Donation @rec_id',
                  array(
                    '@rec_id' => $transaction_entity->id,
                  )
                ),
                $transaction_entity_uri['path']
              ),
              'amount' => array(
                '#type' => 'item',
                '#title' => t('Amount'),
                '#markup' => commerce_currency_format(
                  $transaction_entity_wrapper->commerce_recurring_fixed_price->amount->value(),
                  $transaction_entity_wrapper->commerce_recurring_fixed_price->currency_code->value()
                ),
              ),
              'frequency' => array(
                '#type' => 'item',
                '#title' => t('Frequency'),
                '#markup' => redhen_donation_product_description($product_wrapper),
              ),
              'start_date' => array(
                '#type' => 'item',
                '#title' => t('Start Date'),
                '#markup' => date('m-d-Y H:m',
                  $transaction_entity_wrapper->start_date->value()),
              ),
              'end_date' => array(
                '#type' => 'item',
                '#title' => t('End Date'),
                '#markup' => $transaction_entity_wrapper->end_date->value() == 0 ? t('None') : date('m-d-Y H:m', $transaction_entity_wrapper->end_date->value()),
              ),
              'due_date' => array(
                '#type' => 'item',
                '#title' => t('Due Date'),
                '#markup' => date('m-d-Y H:m', $transaction_entity_wrapper->due_date->value()),
              ),
              'status' => array(
                '#type' => 'item',
                '#title' => t('Status'),
                '#markup' => $transaction_entity_wrapper->status->value() ? t('Enabled') : t('Disabled'),
              ),
            ),
            'orders' => array(
              '#theme' => 'table',
              '#header' => $header,
              '#rows' => $rows,
            ),
          );
          break;

        default:
          $transaction_entity = $transaction_entity_wrapper->value();
          $rows[] = $this->transactionRow($transaction_entity, $order_status);
          $transactions = array(
            'var' => array(
              '#markup' => t('One Time Donation'),
            ),
            'stuff' => array(
              '#theme' => 'table',
              '#header' => $header,
              '#rows' => $rows,
            ),
          );
          break;
      }
      $content['transaction']['list'] = $transactions;
    }

    return entity_get_controller($this->entityType)->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Save donation.
   *
   * @see entity_save()
   */
  public function save() {
    $this->updated = REQUEST_TIME;

    if (!$this->donation_id && empty($this->created)) {
      $this->created = REQUEST_TIME;
    }
    return parent::save();
  }

  /**
   * Specify URI.
   */
  protected function defaultUri() {
    return array('path' => 'redhen/donation/' . $this->internalIdentifier());
  }

  /**
   * Helper to build a row in the donation transactions table.
   */
  protected function transactionRow($order, $order_status) {
    $order_uri = entity_uri('commerce_order', $order);

    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    $order_total = $order_wrapper->commerce_order_total->value();
    $amount = commerce_currency_format(
      $order_total['amount'],
      $order_total['currency_code']
    );
    return array(
      l( $order->order_id, 'user/' . $order->uid . '/orders/' . $order->order_id),
      $amount,
      date('m-d-Y H:m', $order->changed),
      $order_status[$order->status]['title'],
    );
  }

}
