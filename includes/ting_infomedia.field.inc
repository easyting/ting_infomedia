<?php

/**
 * Field definition for ting_infomedia_manifestation_link is moved to
 * ting_infomedia_field module in order to be able to disable modules that
 * implements the field.
 * see http://drupal.org/node/1285540
 */

/**
 * Implements hook_field_formatter_info().
 */
function ting_infomedia_field_formatter_info() {
  return array(
    'ting_infomedia_link_default' => array(
      'label' => t('Default'),
      'field types' => array(
        'ting_infomedia_manifestation_link',
      ),
    )
  );
}


/**
 * Implements hook_field_formatter_view().
 */
function ting_infomedia_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  foreach ($items as $delta => $item) {
    foreach ($entity->manifestation->relationsData as $relation) {
      if ( $relation->relationType == 'dbcaddi:hasOnlineAccess' && $entity->manifestation->main->type->{'$'} == 'Avisartikel' ) {
        $link = ting_infomedia_get_link($entity->ding_entity_id);
        $element[$delta]['#markup'] = theme('ting_infomedia_button', array('ting_infomedia_button' => $link));
      }
    }
  }

  return $element;
}


/**
 * @param $id
 * @return array
 */
function ting_infomedia_get_link($id) {

  if ( empty($id) ) {
    return array();
  }

  $title = t('infomedia_link_text', array(), array('context' => 'ting_infomedia'));

  $link = array(
    '#href' => ting_infomedia_get_article_link($id),
    '#title' => $title,
    'data-pid' => $id,
    'user-message' => ting_infomedia_get_user_message(),
  );
  return $link;
}


/**
 * Parse the entityid and return an url
 *
 * @param String $entityid
 * @return String
 */
function ting_infomedia_get_article_link($entityid) {
  $ids = explode(':', $entityid);
  return '#ting/infomedia/' . $ids[1];
}


/**
 * Determine user message, depending on logged in status
 *
 * @return String
 */
function ting_infomedia_get_user_message() {
  global $user;

  if ( !ding_user_is_provider_user($user) ) {
    return l(t('user_is_not_provider_user', array(), array('context' => 'ting_infomedia')), 'user');
  }

  return t('user_is_provider_user', array(), array('context' => 'ting_infomedia'));

}

