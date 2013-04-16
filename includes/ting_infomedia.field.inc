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

  $relation_types = ting_infomedia_get_ting_relations();
// dpm($relation_types); //     dbcaddi:hasOnlineAccess (String, 22 characters ) Article from Infomedia
// dpm($entity_type); // bibdkManifestation
dpm($entity);

  foreach ($items as $delta => $item) {
    if ( $entity->manifestation->main->type->{'$'} == 'Avisartikel' ) {
      $link = ting_infomedia_get_link($entity->ding_entity_id);
      $element[$delta]['#markup'] = theme('ting_infomedia_button', array('ting_infomedia_button' => $link));
    }
  }

/*
  foreach($items as $delta=>$items) {
    foreach ($entity->relations as $relation) {
      if( isset($relation_types[$relation->type]) && $entity->ac_source == 'Artikler' ) {
        $element[$delta] =
          array(
            '#markup' => l(t('Read article'), ting_infomedia_get_article_link($id), array('attributes' => array('class' => 'infomedia_group', 'name'=>$relation->type))),
          );
      }
    }
  }
*/

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

  $title = t('infomedia_link_text');

  $link = array(
    '#href' => ting_infomedia_get_article_link($id),
    '#title' => $title,
    '#data-pid' => 'infomedia:' . $id,
    '#attributes' => array('class' => 'infomedia_group', 'name'=>$relation->type),
  );
  // drupal_add_js(drupal_get_path('module', 'ting_infomedia') . '/js/ting_infomedia.js');
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
  return 'ting/infomedia/' . $ids[1];
}

