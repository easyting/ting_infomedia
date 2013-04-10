<?php
/**
 * @file
 * An example field using the Field API.
 *
 * This is updated from Barry Jaspan's presentation at Drupalcon Paris,
 * @link http://acquia.com/community/resources/acquia-tv/intro-field-api-module-developers Video Presentation @endlink
 *
 * Providing a field requires:
 * - Defining a field
 *   - hook_field_info()
 *   - hook_field_schema()
 *   - hook_field_validate()
 *   - hook_field_is_empty()
 *
 * - Defining a formatter for the field (the portion that outputs the field for
 *   display)
 *   - hook_field_formatter_info()
 *   - hook_field_formatter_view()
 *
 * - Defining a widget for the edit form
 *   - hook_field_widget_info()
 *   - hook_field_widget_form()
 *
 * *


/**
 * Implements hook_field_formatter_info
 *
 * Notes; 'field types' are passed on to hook_field_formatter_view
 */
function ting_infomedia_field_formatter_info() {
  return array(
    'ting_infomedia_default' => array(
      'label' => t('Default'),
      'field types' => array(
        'ting_infomedia',
      ),
    )
  );
}

/**
 * Implements hook_field_formatter_view()
 */
function ting_infomedia_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
// dpm($entity_type); // bibdkManifestation
// dpm($entity); //  (Object) manifestation
// dpm($field);
// dpm($instance);
// dpm($langcode); // und
// dpm($items); // 0 => id (String, 20 characters ) 870971-avis:89724244
// dpm($display);

  $element = array();

  $id = $entity->ding_entity_id;

  $relation_types = ting_infomedia_get_ting_relations();
  // dpm($relation_types); // dbcaddi:hasOnlineAccess (String, 22 characters ) Article from Infomedia

  foreach ($items as $delta => $item) {
    $element[$delta]['#markup'] = l(t('Read article'), ting_infomedia_get_article_link($id), array('attributes' => array('class' => 'infomedia_group','name'=>$relation->type)));
  }

dpm($element);

  return $element;

}

/**
 * Parse the entityid and return an url
 *
 * @param String $entityid
 * @return String
 */
function ting_infomedia_get_article_link($entityid) {
  $ids = explode(':', $entityid);
  return 'ting/infomedia/' . $ids[1];
}

/**
 * Implements hook_field_validate
 * TODO is some kind of validation needed?
 */
function ting_infomedia_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  // do nothing
}

/**
 * Implements hook_field_is_empty
 * TODO; check
 */
function ting_infomedia_is_empty($item, $field) {
  return false;
}
